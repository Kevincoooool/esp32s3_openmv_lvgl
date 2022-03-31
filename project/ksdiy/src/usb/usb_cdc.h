/* 
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Ha Thach (tinyusb.org)
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
 *
 */
#ifndef _USB_CDC_H
#define _USB_CDC_H
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "tusb.h"
#include "py/ringbuf.h"

#if CONFIG_USB_ENABLED
extern volatile uint8_t  dbg_mode_enabled;

int usb_cdc_init(void);
void usb_msc_init(void);
bool is_dbg_mode_enabled(void);
void cdc_task_debug_mode(void);
void cdc_task_serial_mode(void);
uint32_t usb_cdc_get_buf(uint8_t *buf, uint32_t len);
#endif // CONFIG_USB_ENABLED
#endif // _USB_CDC_H