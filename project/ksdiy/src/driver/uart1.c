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
#include "esp_err.h"
#include "esp_log.h"
#include "uart1.h"
#include "stdlib.h"
int ir_val;
int ir_cmd;
#define CMD_BUFSIZE      256
uint8_t rx_buf[CMD_BUFSIZE] = {0};

void parseCmd(char * cmd);

void parseGcode(char * cmd)
{
    char * tmp;
    char * str;
    //char g_code_cmd;

    str = strtok_r(cmd, " ", &tmp);
    //g_code_cmd = str[0];
    
    int cmd_id = atoi(str);

    cmd_id = cmd_id & 0x00ff;
    
    if( cmd_id == 0x11)
    {
        printf("cmd_id = %d\n",cmd_id);
    }
    else if (cmd_id == 0x12)
    {
        printf("cmd_id = %d\n",cmd_id);
    }
    else if (cmd_id == 0x13)
    {
        printf("cmd_id = %d\n",cmd_id);
    }
    else if (cmd_id == 0x14)
    {
        printf("cmd_id = %d\n",cmd_id);
    }
    else if (cmd_id == 0x80)
    {
        //printf("cmd_id = %d\n",cmd_id);   
    }
    else if (cmd_id == 0x81)
    {
        //printf("cmd_id = %d\n",cmd_id);
    }
    else if (cmd_id == 0xff)
    {
        printf("cmd_id = %d\n",cmd_id);
    }
    else 
    {
        //printf("num_id id error\n");
    }

    //printf("tmp %s\n",tmp);
    while(tmp!=NULL)
    {
        str = strtok_r(0, " ", &tmp);

        if((str[0]=='M') || (str[0]=='m'))
        {


        }
        else if((str[0]=='E') || (str[0]=='e'))
        {
            cmd_id = atoi(str+1);
            printf("cmd_id = E%d\n",cmd_id);

        }else if((str[0]=='P') || (str[0]=='p'))
        {
            cmd_id = atoi(str+1);
            printf("cmd_id = P%d\n",cmd_id);

        }
        else if((str[0]=='D') || (str[0]=='d'))
        {
            ir_cmd = atoi(str+1);
            //printf("ir_cmd = %d\n",cmd_id);

        }
        else if((str[0]=='V') || (str[0]=='v'))
        {
            ir_val = atoi(str+1);
            printf("cmd ir_val = %d\n",ir_val);
            
        }
        else if((str[0]=='U') || (str[0]=='u'))
        {

        }
        
        
    }

}

void parseCmd(char * cmd)
{
    if((cmd[0]=='g') || (cmd[0]=='G'))
    { 
        parseGcode(cmd+1);
    }
}

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
#define PACKET_READ_TICS        (5/ portTICK_RATE_MS)
#define BUF_SIZE (256)
#define ECHO_READ_TOUT          (3) // 3.5T * 8 = 28 ticks, TOUT=3 -> ~24..33 ticks


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
//int gcode_process()
int uart1_read_data()
{   
    
    static char gcode_buf[64];
    static bool gcode_start = false;
    static bool gcode_finish = false;
    static uint8_t gcode_index = 0;
    static bool send_data_flag;
    static double now_time = 0;
    static double last_time = 0;
    if(send_data_flag ==1)
    {
        send_data_flag = 0;
        uart1_send_data("G128 M4\n",8);
    }
    else if(send_data_flag == 0)
    {   
        now_time = mp_hal_ticks_ms(); 
        if(now_time - last_time > 4)
        {
            last_time = now_time;
            send_data_flag = 1;
        } 
    }
    int len = uart_read_bytes(UART_NUM_1, rx_buf, BUF_SIZE, PACKET_READ_TICS);
    for(int i = 0;i<len;i++)
    {
        if(rx_buf[i] == 'G')
        {
            gcode_start = true;
        }
        if(gcode_start == true)
        {
            gcode_buf[gcode_index++] = rx_buf[i];
            if(gcode_index > 63)
            {
                memset(gcode_buf, 0x00, 64);
                gcode_start = false;
                gcode_index = 0;
                return;
            }
            if(rx_buf[i] =='\n')
            {
                gcode_finish = true;
                gcode_buf[gcode_index++] = '\0';
            }
        }
    }
    if(gcode_finish == true)
    {
        send_data_flag = 1;
        //printf("gcode_buf = %s\n",gcode_buf);
        //gcodeprocess(gcode_buf);
        parseCmd(&gcode_buf);
        ir_control1();
        gcode_finish = false;
        gcode_start = false;
        gcode_index = 0;
    }
    return 0;
}



int read_ir_cmd()
{   
    
    static char gcode_buf[64];
    static bool gcode_start = false;
    static bool gcode_finish = false;
    static uint8_t gcode_index = 0;
    static bool send_data_flag;
    static double now_time = 0;
    static double last_time = 0;
    static int time_out;
    send_data_flag = 0;
    now_time = mp_hal_ticks_ms(); 
    uart1_send_data("G48 M1\n",7);
    while(send_data_flag == 0)
    {
        if(mp_hal_ticks_ms() - now_time > 20)
        {
            return 0;
        }
        int len = uart_read_bytes(UART_NUM_1, rx_buf, BUF_SIZE, PACKET_READ_TICS);
        for(int i = 0;i<len;i++)
        {
            if(rx_buf[i] == 'G')
            {
                gcode_start = true;
            }
            if(gcode_start == true)
            {
                gcode_buf[gcode_index++] = rx_buf[i];
                if(gcode_index > 63)
                {
                    memset(gcode_buf, 0x00, 64);
                    gcode_start = false;
                    gcode_index = 0;
                    return 0;
                }
                if(rx_buf[i] =='\n')
                {
                    gcode_finish = true;
                    gcode_buf[gcode_index++] = '\0';
                }
            }
        }
        if(gcode_finish == true)
        {
            send_data_flag = 1;
            //printf("gcode_buf = %s\n",gcode_buf);
            //gcodeprocess(gcode_buf);
            parseCmd(&gcode_buf);
            //ir_control1();
            gcode_finish = false;
            gcode_start = false;
            gcode_index = 0;
            break;
        }

    }
    return ir_cmd;
}


void uart1_send_data(void *send_data,size_t length)
{
    uart_write_bytes(UART_NUM_1, send_data, length);
    
}


uint8_t get_ir_cmd(void)
{
    printf("ir_cmd = %d \n",ir_cmd);
    return ir_cmd;
}

uint8_t get_ir_data(void)
{
    printf("ir_val = %d \n",ir_val);
    return ir_val;
}
#include "drv_motor.h"

#define S1_IN_S2_IN         0
#define S1_IN_S2_OUT        2
#define S1_OUT_S2_IN        4
#define S1_OUT_S2_OUT       6
int LineFollowFlag = 0;



void ir_control1(void)
{

    ir_val &= 0x06;
    printf("ir_val = %d\n",ir_val);
    switch(ir_val)
    {
    case S1_IN_S2_IN:
        forward("400");
        LineFollowFlag = 10;
        break;

    case S1_IN_S2_OUT:

        forward("400");
        if (LineFollowFlag > 1)
        {
            LineFollowFlag--;
        }
        break;

    case S1_OUT_S2_IN:
        forward("400");
        if (LineFollowFlag<20)
        {
            LineFollowFlag++;
        }
        break;

    case S1_OUT_S2_OUT:
        if(LineFollowFlag == 10) 
        {
            backward("400");
        }
        if(LineFollowFlag < 10)
        {
            //uart1_send_data("G129 M1 R-400 L-700\n",19);
            turn_right("500");
        }
        if(LineFollowFlag > 10)
        {
            turn_left("500");
            //uart1_send_data("G129 M1 R700 L400\n",19);

        }
        break;
    }

}
#if 0 
//1111
#define S1_S2_IN        12
#define ONLY_S2_IN      13
#define S1_S2_S3_IN     8
#define S2_S3_IN        9
#define ONLY_S3_IN      11
#define S3_S4_IN        3
#define S2_S3_S4_IN     1
#define ALL_OUT         15
//12,2,123,23,3,34,234,0

void ir_control2(void)
{
    
    //ir_val &= 0x06;
    //printf("ir_val = %d\n",ir_val);
    //12,123,turn left.   2,turn turn left_little .23,forward    34,234,turn  left  3 turn left little
    if(ir_val < 3){  if(ir_val >2); turn_left("500"); }
    switch(ir_val)
    {
    case S1_IN_S2_IN:
        forward("400");
        LineFollowFlag = 10;
        break;

    case S1_IN_S2_OUT:

        forward("400");
        if (LineFollowFlag > 1)
        {
            LineFollowFlag--;
        }
        break;

    case S1_OUT_S2_IN:
        forward("400");
        if (LineFollowFlag<20)
        {
            LineFollowFlag++;
        }
        break;

    case S1_OUT_S2_OUT:
        if(LineFollowFlag == 10) 
        {
            backward("400");
        }
        if(LineFollowFlag < 10)
        {
            //uart1_send_data("G129 M1 R-400 L-700\n",19);
            turn_right("900");
        }
        if(LineFollowFlag > 10)
        {
            turn_left("900");
            //uart1_send_data("G129 M1 R700 L400\n",19);
            
        }
        break;
    }

}
#endif