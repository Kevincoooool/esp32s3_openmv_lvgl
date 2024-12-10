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
#include "esp_err.h"
#include "esp_log.h"
#include "uart1.h"
#include "drv_motor.h"
#if 0
typedef struct
{
    uint8_t  device_id;       //device id
    uint8_t  mcode;           //command id
    uint8_t  data_type;       //command id
    union
    {
        uint8_t int8Val[8];
        uint8_t uint8Val[8];
        uint8_t charVal[8];
        int16_t int16Val[4];
        uint16_t uint16Val[4];
        float floatVal[2];
        int32_t int32Val[2];
        uint32_t uint32Val[2];
        double doubleVal;
    }response;
} command_response_type;

#define CMD_BUFSIZE      24//
uint8_t rx_buf[CMD_BUFSIZE] = {0};
/*
#define DATA_RX_BUFSIZE      64
RING_BUF_DEF_STRUCT s_rx_ring_buf;
uint8_t s_link_rx_buf[DATA_RX_BUFSIZE];
*/
uint8_t ir_val;


void parseCmd(char * cmd);

void parseGcode(char * cmd)
{
    char * tmp;
    char * str;
    //char g_code_cmd;

    str = strtok_r(cmd, " ", &tmp);
    //g_code_cmd = str[0];
    
    int cmd_id = atoi(str);

    cmd_id = cmd_id | 0xff00;
    
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
        printf("cmd_id = %d\n",cmd_id);   
    }
    else if (cmd_id == 0x81)
    {
        printf("cmd_id = %d\n",cmd_id);
    }
    else if (cmd_id == 0xff)
    {
        printf("cmd_id = %d\n",cmd_id);
    }
    else 
    {
        printf("num_id id error\n");
    }

    //printf("tmp %s\n",tmp);
    while(tmp!=NULL)
    {
        str = strtok_r(0, " ", &tmp);

        if((str[0]=='M') || (str[0]=='m'))
        {
            cmd_id = atoi(str+1);
            printf("cmd_id = M%d\n",cmd_id);

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
            cmd_id = atoi(str+1);
            printf("cmd_id = D%d\n",cmd_id);

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


#include "drv_motor.h"

#define S1_IN_S2_IN 6
#define S1_IN_S2_OUT 2
#define S1_OUT_S2_IN 4
#define S1_OUT_S2_OUT 0
int LineFollowFlag = 0;
void ir_control()
{
    read_hub_data();
    //ir_val = 2;
    printf("ir_val = %d\n",ir_val);
    switch(ir_val)
    {
    case S1_IN_S2_IN:
        forward("300");
        LineFollowFlag = 10;
        break;

    case S1_IN_S2_OUT:
        forward("300");
        if (LineFollowFlag > 1)
        {
        LineFollowFlag--;
        }
        break;

    case S1_OUT_S2_IN:
        forward("300");
        if (LineFollowFlag<20)
        {
        LineFollowFlag++;
        }
        break;

    case S1_OUT_S2_OUT:
        if(LineFollowFlag == 10) 
        {
        backward("300");
        }
        if(LineFollowFlag < 10)
        {
        turn_left("300");
        }
        if(LineFollowFlag > 10)
        {
        turn_right("300");
        }
        break;
    }
}

void parseCmd(char * cmd)
{
    if((cmd[0]=='g') || (cmd[0]=='G'))
    { 
        parseGcode(cmd+1);
    }
}



void read_hub_data(void)
{
    void *read_data;
    uart1_read_data(&read_data);
    drv_ringbuf_read((RING_BUF_DEF_STRUCT*)&s_rx_ring_buf, CMD_BUFSIZE, rx_buf);
    printf("rx_buf = %s\n",rx_buf);
    parseCmd(&rx_buf);

    drv_ringbuf_flush((RING_BUF_DEF_STRUCT*)&s_rx_ring_buf);
    ringbuff_rx_reset((RING_BUF_DEF_STRUCT*)&s_rx_ring_buf,CMD_BUFSIZE);
}
#endif
/*
#define S1_IN_S2_IN 6
#define S1_IN_S2_OUT 2
#define S1_OUT_S2_IN 4
#define S1_OUT_S2_OUT 0
int LineFollowFlag = 0;
*/
void ir_control(void)
{
    /*
    uint8_t irdata =  get_ir_data();
    printf("irdata = %d\n",irdata);
    
    switch(irdata)
    {
    case S1_IN_S2_IN:
        forward("300");
        LineFollowFlag = 10;
        break;

    case S1_IN_S2_OUT:
        forward("300");
        if (LineFollowFlag > 1)
        {
        LineFollowFlag--;
        }
        break;

    case S1_OUT_S2_IN:
        forward("300");
        if (LineFollowFlag<20)
        {
        LineFollowFlag++;
        }
        break;

    case S1_OUT_S2_OUT:
        if(LineFollowFlag == 10) 
        {
        backward("300");
        }
        if(LineFollowFlag < 10)
        {
        turn_left("300");
        }
        if(LineFollowFlag > 10)
        {
        turn_right("300");
        }
        break;
        default:break;
    }
    */
}
