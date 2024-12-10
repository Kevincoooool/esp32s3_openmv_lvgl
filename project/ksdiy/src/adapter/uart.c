/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * Development of the code in this file was sponsored by Microbric Pty Ltd
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Damien P. George
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <stdio.h>

#include "driver/uart.h"
#include "soc/uart_periph.h"

#include "py/runtime.h"
#include "py/mphal.h"
#include "string.h"
#include "drv_ring_buf.h"
#define UART1_TEST     0

#if UART1_TEST
#define DATA_TX_BUFSIZE      64
#define DATA_RX_BUFSIZE      64

#define CMD_BUFSIZE      24
uint8_t rx_buf[CMD_BUFSIZE] = {0};
static RING_BUF_DEF_STRUCT s_tx_ring_buf;
volatile uint8_t txcount = 0; 
static uint8_t s_link_tx_buf[DATA_TX_BUFSIZE];

RING_BUF_DEF_STRUCT s_rx_ring_buf;
static uint8_t s_link_rx_buf[DATA_RX_BUFSIZE];


void uart_ringbuf_init(void)
{
    drv_ringbuf_init((RING_BUF_DEF_STRUCT*)&s_tx_ring_buf, s_link_tx_buf, DATA_TX_BUFSIZE);
    drv_ringbuf_init((RING_BUF_DEF_STRUCT*)&s_rx_ring_buf, s_link_rx_buf, DATA_RX_BUFSIZE);
}


#define ECHO_TEST_TXD (46)
#define ECHO_TEST_RXD (48)
#define ECHO_TEST_RTS (UART_PIN_NO_CHANGE)
#define ECHO_TEST_CTS (UART_PIN_NO_CHANGE)

#define ECHO_UART_PORT_NUM      (UART_NUM_1)
#define ECHO_UART_BAUD_RATE     (115200)
#define ECHO_TASK_STACK_SIZE    (CONFIG_EXAMPLE_TASK_STACK_SIZE)
#define PACKET_READ_TICS        (100 / portTICK_RATE_MS)
#define BUF_SIZE (1024)
#define ECHO_READ_TOUT          (3) // 3.5T * 8 = 28 ticks, TOUT=3 -> ~24..33 ticks

#endif

STATIC void uart_irq_handler(void *arg);

void uart_init(void) {
          uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB,
    };
    int intr_alloc_flags = 0;

#if CONFIG_UART_ISR_IN_IRAM
    intr_alloc_flags = ESP_INTR_FLAG_IRAM;
#endif

    ESP_ERROR_CHECK(uart_driver_install(UART_NUM_0, 1024 * 2, 0, 0, NULL, intr_alloc_flags));
    ESP_ERROR_CHECK(uart_param_config(UART_NUM_0, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(UART_NUM_0, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));

    uart_isr_handle_t handle;
    uart_isr_free(UART_NUM_0);
    uart_isr_register(UART_NUM_0, uart_irq_handler, NULL, ESP_INTR_FLAG_LOWMED | ESP_INTR_FLAG_IRAM, &handle);
    uart_enable_rx_intr(UART_NUM_0);
    // uart_isr_handle_t handle;
    // uart_isr_register(UART_NUM_0, uart_irq_handler, NULL, ESP_INTR_FLAG_LOWMED | ESP_INTR_FLAG_IRAM, &handle);
    // uart_enable_rx_intr(UART_NUM_0);
}

void uart_puts(const char *str) {for(int i=0;str[i];i++) {uart_tx_one_char(str[i]);uart_tx_wait_idle(UART_NUM_0);}}
void uart_printf(const char *fmt, ...)
{
  va_list ap;
  char p_buf[256]; 
  int p_len;

  //vTaskSuspendAll();    
  va_start(ap, fmt);
  p_len = vsprintf(p_buf, fmt, ap);
  va_end(ap);
  
  uart_puts(p_buf);
}
// all code executed in ISR must be in IRAM, and any const data must be in DRAM
STATIC void IRAM_ATTR uart_irq_handler(void *arg) {
    volatile uart_dev_t *uart = &UART0;
    #if CONFIG_IDF_TARGET_ESP32S3
    uart->int_clr.rxfifo_full_int_clr = 1;
    uart->int_clr.rxfifo_tout_int_clr = 1;
    #else
    uart->int_clr.rxfifo_full = 1;
    uart->int_clr.rxfifo_tout = 1;
    uart->int_clr.frm_err = 1;
    #endif
    while (uart->status.rxfifo_cnt) {
        #if CONFIG_IDF_TARGET_ESP32
        uint8_t c = uart->fifo.rw_byte;
        #elif CONFIG_IDF_TARGET_ESP32C3 || CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32S3
        uint8_t c = READ_PERI_REG(UART_FIFO_AHB_REG(0)); // UART0
        #endif
        if (c == mp_interrupt_char) {
            mp_sched_keyboard_interrupt();
        } else {
            // this is an inline function so will be in IRAM
            ringbuf_put(&stdin_ringbuf, c);
        }
    }
}

#if UART1_TEST
void uart1_init(void)
{
    uart_ringbuf_init();
    uart_config_t uart_config = {
        .baud_rate = ECHO_UART_BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB,
    };
    int intr_alloc_flags = 0;

    ESP_ERROR_CHECK(uart_driver_install(UART_NUM_1, BUF_SIZE * 2, 0, 0, NULL, intr_alloc_flags));
    ESP_ERROR_CHECK(uart_param_config(UART_NUM_1, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(UART_NUM_1, ECHO_TEST_TXD, ECHO_TEST_RXD, ECHO_TEST_RTS, ECHO_TEST_CTS));
    ESP_ERROR_CHECK(uart_set_mode(UART_NUM_1, UART_MODE_RS485_HALF_DUPLEX));
    ESP_ERROR_CHECK(uart_set_rx_timeout(UART_NUM_1, ECHO_READ_TOUT));

}

int uart1_read_data(void *read_data)
{
    int len = uart_read_bytes(UART_NUM_1, read_data, BUF_SIZE, PACKET_READ_TICS);
    drv_ringbuf_write((RING_BUF_DEF_STRUCT*)&s_rx_ring_buf, read_data, len);
    drv_ringbuf_read((RING_BUF_DEF_STRUCT*)&s_rx_ring_buf, CMD_BUFSIZE, rx_buf);
    printf("rx_buf = %s\r\n",rx_buf);
    drv_ringbuf_flush((RING_BUF_DEF_STRUCT*)&s_rx_ring_buf);
    ringbuff_rx_reset((RING_BUF_DEF_STRUCT*)&s_rx_ring_buf,CMD_BUFSIZE);
    return len;
}

void uart1_send_data(void *send_data,size_t length)
{
    uart_write_bytes(UART_NUM_1, send_data, length);
    
}
#endif