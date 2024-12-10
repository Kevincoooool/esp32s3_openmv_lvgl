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
#ifndef MICROPY_INCLUDED_ESP32_UART1_H
#define MICROPY_INCLUDED_ESP32_UART1_H
#include <stdio.h>
#include "driver/uart.h"
#include "string.h"
#include "drv_ring_buf.h"

#define DATA_TX_BUFSIZE      64
#define DATA_RX_BUFSIZE      64


RING_BUF_DEF_STRUCT s_tx_ring_buf;
uint8_t s_link_tx_buf[DATA_TX_BUFSIZE];

RING_BUF_DEF_STRUCT s_rx_ring_buf;
uint8_t s_link_rx_buf[DATA_RX_BUFSIZE];
uint8_t get_ir_data(void);
void ir_control1(void);
void uart1_init(void);
int uart1_read_data();
void uart1_send_data(void *send_data,size_t length);
uint8_t get_ir_cmd(void);
int read_ir_cmd();
#endif // MICROPY_INCLUDED_ESP32_UART_H
