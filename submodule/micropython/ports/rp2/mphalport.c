/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020-2021 Damien P. George
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

#include "py/runtime.h"
#include "py/stream.h"
#include "py/mphal.h"
#include "shared/timeutils/timeutils.h"
#include "tusb.h"
#include "uart.h"
#include "hardware/rtc.h"
#include "hardware/irq.h"
#include "hardware/regs/intctrl.h"
#include "pendsv.h"
#include "usbdbg.h"
#if MICROPY_HW_USB_CDC_1200BPS_TOUCH
#include "pico/bootrom.h"
#endif

#if MICROPY_HW_ENABLE_UART_REPL

#ifndef UART_BUFFER_LEN
// reasonably big so we can paste
#define UART_BUFFER_LEN 256
#endif

STATIC uint8_t stdin_ringbuf_array[UART_BUFFER_LEN];
ringbuf_t stdin_ringbuf = { stdin_ringbuf_array, sizeof(stdin_ringbuf_array) };

#endif

#define DBG_MAX_PACKET      (64)
#define IDE_BAUDRATE_SLOW   (921600)
#define IDE_BAUDRATE_FAST   (12000000)
static volatile uint8_t  dbg_mode_enabled;

static uint8_t tx_ringbuf_array[1024];
static volatile ringbuf_t tx_ringbuf;

extern void __fatal_error();

#if MICROPY_KBD_EXCEPTION

int mp_interrupt_char = -1;

void tud_cdc_rx_wanted_cb(uint8_t itf, char wanted_char) {
    if (dbg_mode_enabled == false) {
        (void)itf;
        (void)wanted_char;
        tud_cdc_read_char(); // discard interrupt char
        mp_sched_keyboard_interrupt();
    }
}

void mp_hal_set_interrupt_char(int c) {
    if (dbg_mode_enabled == false) {
        mp_interrupt_char = c;
        tud_cdc_set_wanted_char(c);
    }
}

#endif

uint32_t usb_cdc_buf_len()
{
    return ringbuf_avail((ringbuf_t*)&tx_ringbuf);
}

uint32_t usb_cdc_get_buf(uint8_t *buf, uint32_t len)
{
    for (int i=0; i<len; i++) {
        buf[i] = ringbuf_get((ringbuf_t*)&tx_ringbuf);
    }
    return len;
}

typedef struct __attribute__((packed)) {
    uint8_t cmd;
    uint8_t request;
    uint32_t xfer_length;
} usbdbg_cmd_t;

static void usb_cdc_task(void)
{
    tud_task();

    uint8_t dbg_buf[DBG_MAX_PACKET];
    if (tud_cdc_connected() && tud_cdc_available() >= 6) {
        uint32_t count = tud_cdc_read(dbg_buf, 6);
        if (count < 6 || dbg_buf[0] != 0x30) {
            //This shouldn't happen
            __fatal_error();
            usbdbg_control(NULL, USBDBG_NONE, 0);
            return;
        }
        usbdbg_cmd_t *cmd = (usbdbg_cmd_t *) dbg_buf;
        uint8_t request = cmd->request;
        uint32_t xfer_length = cmd->xfer_length;
        usbdbg_control(NULL, request, xfer_length);

        while (xfer_length) {// && tud_cdc_connected()) {
            if (tud_task_event_ready()) {
                tud_task();
            }
            if (request & 0x80) {
                // Device-to-host data phase
                int bytes = MIN(xfer_length, DBG_MAX_PACKET);
                if (bytes <= tud_cdc_write_available()) {
                    xfer_length -= bytes;
                    usbdbg_data_in(dbg_buf, bytes);
                    tud_cdc_write(dbg_buf, bytes);
                }
                tud_cdc_write_flush();
            } else {
                // Host-to-device data phase
                int bytes = MIN(xfer_length, DBG_MAX_PACKET);
                uint32_t count = tud_cdc_read(dbg_buf, bytes);
                if (count == bytes) {
                    xfer_length -= count;
                    usbdbg_data_out(dbg_buf, count);
                }
            }
        }
    }
}

uintptr_t mp_hal_stdio_poll(uintptr_t poll_flags) {
    uintptr_t ret = 0;
    if (dbg_mode_enabled) {
        return ret;
    }

    #if MICROPY_HW_ENABLE_UART_REPL
    if ((poll_flags & MP_STREAM_POLL_RD) && ringbuf_peek(&stdin_ringbuf) != -1) {
        ret |= MP_STREAM_POLL_RD;
    }
    #endif
    #if MICROPY_HW_ENABLE_USBDEV
    if (tud_cdc_connected() && tud_cdc_available()) {
        ret |= MP_STREAM_POLL_RD;
    }
    #endif
    return ret;
}

// Receive single character
int mp_hal_stdin_rx_chr(void) {
    if (dbg_mode_enabled) {
        for (;;) {
            MICROPY_EVENT_POLL_HOOK
        }
    }

    for (;;) {
        #if MICROPY_HW_ENABLE_UART_REPL
        int c = ringbuf_get(&stdin_ringbuf);
        if (c != -1) {
            return c;
        }
        #endif
        #if MICROPY_HW_ENABLE_USBDEV
        if (tud_cdc_connected() && tud_cdc_available()) {
            uint8_t buf[1];
            uint32_t count = tud_cdc_read(buf, sizeof(buf));
            if (count) {
                return buf[0];
            }
        }
        #endif
        MICROPY_EVENT_POLL_HOOK
    }
}

// Send string of given length
void mp_hal_stdout_tx_strn(const char *str, mp_uint_t len) {
    if (dbg_mode_enabled) {
        if (tud_cdc_connected()) {
            NVIC_DisableIRQ(PendSV_IRQn);
            for (int i=0; i<len; i++) {
                ringbuf_put((ringbuf_t*)&tx_ringbuf, str[i]);
            }
            NVIC_EnableIRQ(PendSV_IRQn);
        }
        return;
    }

    #if MICROPY_HW_ENABLE_UART_REPL
    mp_uart_write_strn(str, len);
    #endif

    #if MICROPY_HW_ENABLE_USBDEV
    if (tud_cdc_connected()) {
        for (size_t i = 0; i < len;) {
            uint32_t n = len - i;
            if (n > CFG_TUD_CDC_EP_BUFSIZE) {
                n = CFG_TUD_CDC_EP_BUFSIZE;
            }
            while (n > tud_cdc_write_available()) {
                tud_task();
                tud_cdc_write_flush();
            }
            uint32_t n2 = tud_cdc_write(str + i, n);
            tud_task();
            tud_cdc_write_flush();
            i += n2;
        }
    }
    #endif

}

void mp_hal_delay_ms(mp_uint_t ms) {
    absolute_time_t t = make_timeout_time_ms(ms);
    while (!time_reached(t)) {
        mp_handle_pending(true);
        best_effort_wfe_or_timeout(t);
        MICROPY_HW_USBDEV_TASK_HOOK
    }
}

uint64_t mp_hal_time_ns(void) {
    datetime_t t;
    rtc_get_datetime(&t);
    uint64_t s = timeutils_seconds_since_epoch(t.year, t.month, t.day, t.hour, t.min, t.sec);
    return s * 1000000000ULL;
}

static void usb_irq_handler(void) {
    dcd_int_handler(0);
    // If there are any events to process, schedule a call to cdc loop.
    if (dbg_mode_enabled) {
        pendsv_schedule_dispatch(PENDSV_DISPATCH_CDC, usb_cdc_task);
    }
}

void tud_cdc_line_coding_cb(uint8_t itf, cdc_line_coding_t const* p_line_coding)
{
    if (0) {
    #if MICROPY_HW_USB_CDC_1200BPS_TOUCH
    } else if (p_line_coding->bit_rate == 1200) {
        reset_usb_boot(0, 0);
    #endif
    } else if (p_line_coding->bit_rate == IDE_BAUDRATE_SLOW ||
            p_line_coding->bit_rate == IDE_BAUDRATE_FAST) {
        dbg_mode_enabled = true;
    } else {
        dbg_mode_enabled = false;
    }
    tx_ringbuf.iget = 0;
    tx_ringbuf.iput = 0;
}

int mp_hal_init(void)
{
    static bool initialized = false;
    if (!initialized) {
        initialized = true;
        dbg_mode_enabled = false;

        tx_ringbuf.iget = 0;
        tx_ringbuf.iput = 0;
        tx_ringbuf.buf = tx_ringbuf_array;
        tx_ringbuf.size = sizeof(tx_ringbuf_array);
        tusb_init();
        irq_set_enabled(USBCTRL_IRQ, false);
        irq_remove_handler(USBCTRL_IRQ, irq_get_exclusive_handler(USBCTRL_IRQ));
        irq_set_exclusive_handler(USBCTRL_IRQ, usb_irq_handler);
        irq_set_enabled(USBCTRL_IRQ, true);
    }

    return 0;
}

uint32_t HAL_GetHalVersion()
{
    // Hard-coded becasue it's not defined in SDK
    return ((1<<24) | (3<<16) | (0<<8) | (0<<0));
}
