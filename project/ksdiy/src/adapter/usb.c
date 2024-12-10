/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Damien P. George
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
#include "py/mphal.h"
#include "usb.h"

#if CONFIG_USB_ENABLED

#include "tinyusb.h"
#include "tusb_cdc_acm.h"

#define CDC_ITF TINYUSB_CDC_ACM_0

static uint8_t usb_rx_buf[4096];

static void usb_callback_rx(int itf, cdcacm_event_t *event) {
    // TODO: what happens if more chars come in during this function, are they lost?
    for (;;) {
        size_t len = 0;
        esp_err_t ret = tinyusb_cdcacm_read(itf, usb_rx_buf, sizeof(usb_rx_buf), &len);
        if (ret != ESP_OK) {
            break;
        }
        if (len == 0) {
            break;
        }
        for (size_t i = 0; i < len; i++) {
            if (usb_rx_buf[i] == mp_interrupt_char) {
                mp_sched_keyboard_interrupt();printf("keyboard_interrupt 0x%X\n", usb_rx_buf[i]);
            } else {
                ringbuf_put(&stdin_ringbuf, usb_rx_buf[i]);
            }
            printf("%c", usb_rx_buf[i]);
        }
        printf(" avail %d, put %d bytes\n", ringbuf_avail(&stdin_ringbuf), len);
    }
}

void usb_cdc_init(void) {
    // Initialise the USB with defaults.
    tinyusb_config_t tusb_cfg = {0};
    ESP_ERROR_CHECK(tinyusb_driver_install(&tusb_cfg));

    // Initialise the USB serial interface.
    tinyusb_config_cdcacm_t amc_cfg = {
        .usb_dev = TINYUSB_USBDEV_0,
        .cdc_port = CDC_ITF,
        .rx_unread_buf_sz = 4096,
        .callback_rx = &usb_callback_rx,
        .callback_rx_wanted_char = NULL,
        .callback_line_state_changed = NULL,
        .callback_line_coding_changed = NULL
    };
    ESP_ERROR_CHECK(tusb_cdc_acm_init(&amc_cfg));
}

void usb_tx_strn(const char *str, size_t len) {
    while (len) {
        size_t l = len;
        if (l > CONFIG_USB_CDC_TX_BUFSIZE) {
            l = CONFIG_USB_CDC_TX_BUFSIZE;
        }
        tinyusb_cdcacm_write_queue(CDC_ITF, (uint8_t *)str, l);
        tinyusb_cdcacm_write_flush(CDC_ITF, pdMS_TO_TICKS(1000));
        str += l;
        len -= l;
    }
}

void cdc_printf(const char *fmt, ...)
{
}

void cdc_task_serial_mode(void)
{
}

void cdc_task_debug_mode(void)
{
}

bool is_dbg_mode_enabled(void)
{
    return false;
}


#endif // CONFIG_USB_ENABLED
