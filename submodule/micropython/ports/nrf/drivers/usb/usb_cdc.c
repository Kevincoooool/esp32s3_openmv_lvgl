/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Ha Thach (tinyusb.org)
 * Copyright (c) 2019 Glenn Ruben Bakke
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
 * This file is part of the TinyUSB stack.
 */

#include "py/mphal.h"

#if MICROPY_HW_USB_CDC

#include "tusb.h"
#include "nrfx.h"
#include "nrfx_power.h"
#include "nrfx_uart.h"
#include "py/ringbuf.h"

#ifdef BLUETOOTH_SD
#include "nrf_sdm.h"
#include "nrf_soc.h"
#include "ble_drv.h"
#endif

#include "pendsv.h"

#define DBG_MAX_PACKET      (64)
#define IDE_BAUDRATE_SLOW   (921600)
#define IDE_BAUDRATE_FAST   (12000000)
volatile uint8_t  dbg_mode_enabled;

extern void usbdbg_data_in(void *buffer, int length);
extern void usbdbg_data_out(void *buffer, int length);
extern void usbdbg_control(void *buffer, uint8_t brequest, uint32_t wlength);

extern void tusb_hal_nrf_power_event(uint32_t event);

static void cdc_task(void);

static uint8_t rx_ringbuf_array[1024];
static uint8_t tx_ringbuf_array[1024];
static volatile ringbuf_t rx_ringbuf;
static volatile ringbuf_t tx_ringbuf;

static void board_init(void) {
    // Config clock source.
#ifndef BLUETOOTH_SD
    NRF_CLOCK->LFCLKSRC = (uint32_t)((CLOCK_LFCLKSRC_SRC_Xtal << CLOCK_LFCLKSRC_SRC_Pos) & CLOCK_LFCLKSRC_SRC_Msk);
    NRF_CLOCK->TASKS_LFCLKSTART = 1UL;
#endif

    // Priorities 0, 1, 4 (nRF52) are reserved for SoftDevice
    // 2 is highest for application
    NRFX_IRQ_PRIORITY_SET(USBD_IRQn, 2);

    // USB power may already be ready at this time -> no event generated
    // We need to invoke the handler based on the status initially
    uint32_t usb_reg;

#ifdef BLUETOOTH_SD
    uint8_t sd_en = false;
    sd_softdevice_is_enabled(&sd_en);

    if (sd_en) {
        sd_power_usbdetected_enable(true);
        sd_power_usbpwrrdy_enable(true);
        sd_power_usbremoved_enable(true);

        sd_power_usbregstatus_get(&usb_reg);
    } else
#endif
    {
        // Power module init
        const nrfx_power_config_t pwr_cfg = { 0 };
        nrfx_power_init(&pwr_cfg);

        // Register tusb function as USB power handler
        const nrfx_power_usbevt_config_t config = { .handler = (nrfx_power_usb_event_handler_t) tusb_hal_nrf_power_event };
        nrfx_power_usbevt_init(&config);

        nrfx_power_usbevt_enable();

        usb_reg = NRF_POWER->USBREGSTATUS;
    }

    if (usb_reg & POWER_USBREGSTATUS_VBUSDETECT_Msk) {
        tusb_hal_nrf_power_event(NRFX_POWER_USB_EVT_DETECTED);
    }

#ifndef BLUETOOTH_SD
    if (usb_reg & POWER_USBREGSTATUS_OUTPUTRDY_Msk) {
        tusb_hal_nrf_power_event(NRFX_POWER_USB_EVT_READY);
    }
#endif
}

static bool cdc_rx_any(void) {
    return rx_ringbuf.iput != rx_ringbuf.iget;
}

static int cdc_rx_char(void) {
    return ringbuf_get((ringbuf_t*)&rx_ringbuf);
}

bool cdc_tx_any(void) {
    return tx_ringbuf.iput != tx_ringbuf.iget;
}

static int cdc_tx_char(void) {
    return ringbuf_get((ringbuf_t*)&tx_ringbuf);
}

uint32_t usb_cdc_buf_len()
{
    return ringbuf_avail((ringbuf_t*)&tx_ringbuf);
}

uint32_t usb_cdc_get_buf(uint8_t *buf, uint32_t len)
{
    int i=0;
    for (; i<len; i++) {
        buf[i] = ringbuf_get((ringbuf_t*)&tx_ringbuf);
        if (buf[i] == -1) {
            break;
        }
    }
    return i;
}

static void cdc_task(void)
{
    if ( tud_cdc_connected() ) {
        // connected and there are data available
        while (tud_cdc_available()) {
            int c;
            uint32_t count = tud_cdc_read(&c, 1);
            (void)count;
            ringbuf_put((ringbuf_t*)&rx_ringbuf, c);
        }

        int chars = 0;
        while (cdc_tx_any()) {
            if (chars < 64) {
                tud_cdc_write_char(cdc_tx_char());
                chars++;
            } else {
                chars = 0;
                tud_cdc_write_flush();
            }
        }

        tud_cdc_write_flush();
    }
}

static void cdc_task_debug_mode(void)
{
    if ( tud_cdc_connected() && tud_cdc_available() ) {
        uint8_t buf[DBG_MAX_PACKET];
        uint32_t count = tud_cdc_read(buf, 6);
        if (count < 6) {
            //Shouldn't happen
            return;
        }
        // assert buf[0] == '\x30';
        uint8_t request = buf[1];
        uint32_t xfer_length = *((uint32_t*)(buf+2));
        usbdbg_control(buf+6, request, xfer_length);

        while (xfer_length) {
            if (tud_task_event_ready()) {
                tud_task();
            }
            if (request & 0x80) {
                // Device-to-host data phase
                int bytes = MIN(xfer_length, DBG_MAX_PACKET);
                if (bytes <= tud_cdc_write_available()) {
                    xfer_length -= bytes;
                    usbdbg_data_in(buf, bytes);
                    tud_cdc_write(buf, bytes);
                }
                tud_cdc_write_flush();
            } else {
                // Host-to-device data phase
                int bytes = MIN(xfer_length, DBG_MAX_PACKET);
                uint32_t count = tud_cdc_read(buf, bytes);
                if (count == bytes) {
                    xfer_length -= count;
                    usbdbg_data_out(buf, count);
                }
            }
        }
    }
}


void usb_cdc_loop(void) {
    tud_task();
    if (dbg_mode_enabled == true) {
        cdc_task_debug_mode();
    } else {
        cdc_task();
    }
}

int usb_cdc_init(void)
{
    static bool initialized = false;
    if (!initialized) {

#if BLUETOOTH_SD
        // Initialize the clock and BLE stack.
        ble_drv_stack_enable();
#endif

        board_init();
        initialized = true;

        rx_ringbuf.buf = rx_ringbuf_array;
        rx_ringbuf.size = sizeof(rx_ringbuf_array);
        rx_ringbuf.iget = 0;
        rx_ringbuf.iput = 0;

        tx_ringbuf.buf = tx_ringbuf_array;
        tx_ringbuf.size = sizeof(tx_ringbuf_array);
        tx_ringbuf.iget = 0;
        tx_ringbuf.iput = 0;

        tusb_init();
    }

    return 0;
}

#ifdef BLUETOOTH_SD
// process SOC event from SD
void usb_cdc_sd_event_handler(uint32_t soc_evt) {
    /*------------- usb power event handler -------------*/
    int32_t usbevt = (soc_evt == NRF_EVT_POWER_USB_DETECTED   ) ? NRFX_POWER_USB_EVT_DETECTED:
                     (soc_evt == NRF_EVT_POWER_USB_POWER_READY) ? NRFX_POWER_USB_EVT_READY   :
                     (soc_evt == NRF_EVT_POWER_USB_REMOVED    ) ? NRFX_POWER_USB_EVT_REMOVED : -1;

    if (usbevt >= 0) {
        tusb_hal_nrf_power_event(usbevt);
    }
}
#endif

#define CDC_CRITICAL_SECTION_ENTER() NVIC_DisableIRQ(USBD_IRQn)
#define CDC_CRITICAL_SECTION_EXIT()  NVIC_EnableIRQ(USBD_IRQn)

int mp_hal_stdin_rx_chr(void) {
    for (;;) {
        if (dbg_mode_enabled) {
            __WFI();
        } else if (cdc_rx_any()) {
            return cdc_rx_char();
        }
    }
    return 0;
}

void mp_hal_stdout_tx_strn(const char *str, mp_uint_t len) {
    CDC_CRITICAL_SECTION_ENTER();
    for (const char *top = str + len; str < top; str++) {
        ringbuf_put((ringbuf_t*)&tx_ringbuf, *str);
    }
    CDC_CRITICAL_SECTION_EXIT();
}

void mp_hal_stdout_tx_strn_cooked(const char *str, mp_uint_t len) {
    CDC_CRITICAL_SECTION_ENTER();
    for (const char *top = str + len; str < top; str++) {
        if (*str == '\n') {
            ringbuf_put((ringbuf_t*)&tx_ringbuf, '\r');
        }
        ringbuf_put((ringbuf_t*)&tx_ringbuf, *str);
    }
    CDC_CRITICAL_SECTION_EXIT();
}

void tud_cdc_line_coding_cb(uint8_t itf, cdc_line_coding_t const* p_line_coding)
{
    if (0) {
    #if MICROPY_HW_USB_CDC_1200BPS_TOUCH
    } else if (p_line_coding->bit_rate == 1200) {
        MICROPY_RESET_TO_BOOTLOADER();
    #endif
    } else if (p_line_coding->bit_rate == IDE_BAUDRATE_SLOW ||
            p_line_coding->bit_rate == IDE_BAUDRATE_FAST) {
        dbg_mode_enabled = 1;
    } else {
        dbg_mode_enabled = 0;
    }
    tx_ringbuf.iget = 0;
    tx_ringbuf.iput = 0;

    rx_ringbuf.iget = 0;
    rx_ringbuf.iput = 0;
}

void USBD_IRQHandler(void) {
    dcd_int_handler(0);
    // If there are any event to process, schedule a call to cdc loop.
    if (cdc_tx_any() || tud_task_event_ready()) {
        pendsv_schedule_dispatch(PENDSV_DISPATCH_CDC, usb_cdc_loop);
    }
}

#endif // MICROPY_HW_USB_CDC
