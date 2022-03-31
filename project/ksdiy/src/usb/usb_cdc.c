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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "queue.h"
#include "semphr.h"
#include "esp_task.h"
#include "py/mpthread.h"

#include "tusb.h"
#include "usb_cdc.h"
#include "tinyusb.h"
#include "esp_log.h"
#include "tusb_msc.h"
#include "lib/oofatfs/ff.h"
#include "lib/oofatfs/diskio.h"
#include "extmod/vfs_fat.h"
#if CONFIG_USB_ENABLED
#define DBG_MAX_PACKET      (64)
#define IDE_BAUDRATE_SLOW   (921600)
#define IDE_BAUDRATE_FAST   (12000000)

// MicroPython runs as a task under FreeRTOS
#define USB_CDC_TASK_PRIORITY        (ESP_TASK_PRIO_MIN + 2)
#define USB_CDC_TASK_STACK_SIZE      (16 * 1024)

#ifndef MIN
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif

volatile uint8_t  dbg_mode_enabled = 1;
static bool linecode_set = false, userial_open = false;
#define debug(fmt, ...)   if(0) printf(fmt, ##__VA_ARGS__)

extern void usbdbg_data_in(void *buffer, int length);
extern void usbdbg_data_out(void *buffer, int length);
extern void usbdbg_control(void *buffer, uint8_t brequest, uint32_t wlength);

 uint8_t tx_ringbuf_array[1024*100];
volatile ringbuf_t tx_ringbuf;

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

#include "py/runtime.h"
#include "py/mphal.h"
void cdc_task_serial_mode(void)
{
    if (tud_ready() && userial_open) {
        // connected and there are data available
        if (tud_cdc_available()) {
            uint8_t usb_rx_buf[CONFIG_USB_CDC_RX_BUFSIZE];
            uint32_t len = tud_cdc_read(usb_rx_buf, CONFIG_USB_CDC_RX_BUFSIZE);
            for (int i = 0; i < len; i++) {
                if (usb_rx_buf[i] == mp_interrupt_char) {
                    debug("keyboard_interrupt. ");mp_sched_keyboard_interrupt();
                } else {
                    ringbuf_put(&stdin_ringbuf, usb_rx_buf[i]);
                }
                debug("%c", usb_rx_buf[i]);
            }
            debug(" avail %d, put %d bytes\n", ringbuf_avail(&stdin_ringbuf), len);
        } else {
          vTaskDelay(pdMS_TO_TICKS(1));
        }
    } else {
      vTaskDelay(pdMS_TO_TICKS(200));
    }
}

#define CDC_WRDAT(w_buf, w_len) do {  \
  int pos = 0, left = w_len;  \
  while(left > 0) { \
    int count = tud_cdc_write(&w_buf[pos], left); \
    pos+=count; \
    left = w_len-pos; \
  } \
  tud_cdc_write_flush();  \
} while(0)

void usb_tx_strn(const char *str, size_t len) {
    debug("usb_tx_strn %d bytes\n", len);
    if(dbg_mode_enabled == false) {
      CDC_WRDAT(str, len);vTaskDelay(pdMS_TO_TICKS(10));
    } else {
       for (int i=0;i<len;i++) {
        while(ringbuf_put(&tx_ringbuf, (uint8_t )str[i]) < 0) {
          printf("Error: usb_cdc tx_ringbuf overflow!\n");
          vTaskDelay(pdMS_TO_TICKS(5));
        }
      }
    }	  
}

void cdc_task_debug_mode(void)
{
    if (tud_cdc_connected()) {
      if(tud_cdc_available()) {
        uint8_t buf[DBG_MAX_PACKET];
        uint32_t count = tud_cdc_read(buf, 6);
        if (count < 6) {
            //Shouldn't happen
            return;
        }
        // assert buf[0] == '\x30';
        uint8_t request = buf[1];
        uint32_t xfer_length = *((uint32_t*)(buf + 2));
        usbdbg_control(buf + 6, request, xfer_length);

        while (xfer_length) {
            if (request & 0x80) {
                // Device-to-host data phase
                int bytes = MIN(xfer_length, DBG_MAX_PACKET);
                if (bytes <= tud_cdc_write_available()) {
                    xfer_length -= bytes;
                    usbdbg_data_in(buf, bytes);
                    tud_cdc_write(buf, bytes);
                    
                }
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
        tud_cdc_write_flush();
      }
    } else {
      vTaskDelay(pdMS_TO_TICKS(1));
    }
}

void tud_cdc_line_coding_cb(uint8_t itf, cdc_line_coding_t const* p_line_coding)
{
    if (p_line_coding->bit_rate == IDE_BAUDRATE_SLOW ||
            p_line_coding->bit_rate == IDE_BAUDRATE_FAST) {
        dbg_mode_enabled = 1;
    } else {
        dbg_mode_enabled = 0;
    }
    tx_ringbuf.iget = 0;
    tx_ringbuf.iput = 0;

    linecode_set = true;
    debug("tud_cdc_line_coding_cb dbg_mode_enabled %d\n", dbg_mode_enabled);
}

void tud_cdc_line_state_cb(uint8_t itf, bool dtr, bool rts)
{  
  if(!dtr) {
    if(linecode_set && !userial_open)
      userial_open = true;
    else if(userial_open && !linecode_set) {
      userial_open = false;   
    }
    
    if(linecode_set)
      linecode_set = false;
      
  } else {
      userial_open = true;
  }

  debug("tud_cdc_line_state_cb userial_open %d, line_state %d\n", userial_open, (dtr|(rts<<1)));
}


bool is_dbg_mode_enabled(void)
{
    return dbg_mode_enabled;
}

void cdc_loop() {
    while(true) {
        if(is_dbg_mode_enabled()) {
            cdc_task_debug_mode();
        } else {
            cdc_task_serial_mode();
        }
    }
}

#if CONFIG_IDF_TARGET_ESP32S3
static void usb_otg_router_to_internal_phy()
{
  uint32_t *usb_phy_sel_reg = (uint32_t *)(0x60008000 + 0x120);
  *usb_phy_sel_reg |= BIT(19) | BIT(20);
}
#endif
esp_err_t tusb_msc_init(const tinyusb_config_msc_t *cfg);
int usb_cdc_init(void)
{
    vTaskDelay(100 / portTICK_PERIOD_MS);
    static bool initialized = false;
    if (!initialized) {
        initialized = true;

        tx_ringbuf.buf = tx_ringbuf_array;
        tx_ringbuf.size = sizeof(tx_ringbuf_array);
        tx_ringbuf.iget = 0;
        tx_ringbuf.iput = 0;
        //tusb_init();
        tinyusb_config_t tusb_cfg = {
            .descriptor = NULL,
            .string_descriptor = NULL,
            .external_phy = false // In the most cases you need to use a `false` value
        };
#if CONFIG_IDF_TARGET_ESP32S3
        usb_otg_router_to_internal_phy();
#endif
        ESP_ERROR_CHECK(tinyusb_driver_install(&tusb_cfg));
        
        //         tinyusb_config_t tusb_cfg = {
//             .descriptor = NULL,
//             .string_descriptor = NULL,
//             .external_phy = false // In the most cases you need to use a `false` value
//         };
// #if CONFIG_IDF_TARGET_ESP32S3
//         usb_otg_router_to_internal_phy();
// #endif
//         ESP_ERROR_CHECK(tinyusb_driver_install(&tusb_cfg));
    //     tinyusb_config_msc_t msc_cfg = {
    //     .pdrv = 0,
    //   };

    //   ESP_ERROR_CHECK(tusb_msc_init(&msc_cfg));
        xTaskCreatePinnedToCore(cdc_loop, "cdc_loop", USB_CDC_TASK_STACK_SIZE / sizeof(StackType_t), NULL, USB_CDC_TASK_PRIORITY, NULL, 1);
    }
    return 0;
}

void cdc_printf2(const char *fmt, ...)
{  
  va_list ap;
  char p_buf[1024]; 
  int p_len;

  va_start(ap, fmt);
  p_len = vsprintf(p_buf, fmt, ap);
  va_end(ap);//debug("cdc_printf size %d, cdc open %d\n", p_len, tud_cdc_connected());
  p_buf[p_len] = '\r';  
  p_buf[p_len+1] = '\n';
  p_buf[p_len+2] = 0;
  if(!dbg_mode_enabled && tud_ready() && userial_open)
    CDC_WRDAT(p_buf, p_len+2);
  else
    printf("%s", p_buf);
}

void cdc_printf(const char *fmt, ...)
{  
  if(!dbg_mode_enabled && tud_ready() && userial_open) {  
    va_list ap;
    char p_buf[256]; 
    int p_len;
   
    va_start(ap, fmt);
    p_len = vsprintf(p_buf, fmt, ap);
    va_end(ap);//debug("cdc_printf size %d, cdc open %d\n", p_len, tud_cdc_connected());
    CDC_WRDAT(p_buf, p_len);    
  }
}


// #if 1

#define LOGICAL_DISK_NUM 1

static uint32_t s_pdrv[LOGICAL_DISK_NUM] = {0};
static tusb_msc_callback_t cb_mount[LOGICAL_DISK_NUM] = {NULL};
static tusb_msc_callback_t cb_unmount[LOGICAL_DISK_NUM] = {NULL};
static int s_disk_block_size[LOGICAL_DISK_NUM] = {0};
static bool s_ejected[LOGICAL_DISK_NUM] = {true};

esp_err_t tusb_msc_init(const tinyusb_config_msc_t *cfg)
{
    if (cfg == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
  printf("tusb_msc_init");
    cb_mount[0] = cfg->cb_mount;
    cb_unmount[0] = cfg->cb_unmount;
    s_pdrv[0] = cfg->pdrv;
    //s_pdrv[1] = 1;
    return ESP_OK;
}

// //--------------------------------------------------------------------+
// // tinyusb callbacks
// //--------------------------------------------------------------------+

// // Invoked when device is mounted
// void tud_mount_cb(void)
// {
//     // Reset the ejection tracking every time we're plugged into USB. This allows for us to battery
//     // power the device, eject, unplug and plug it back in to get the drive.
//     for (uint8_t i = 0; i < LOGICAL_DISK_NUM; i++) {
//         s_ejected[i] = false;
//     }

//     if (cb_mount[0])
//     {
//         cb_mount[0](0, NULL);
//     }
    
//     ESP_LOGI(__func__, "");
// }

// // Invoked when device is unmounted
// void tud_umount_cb(void)
// {
//     if (cb_unmount[0])
//     {
//         cb_unmount[0](0, NULL);
//     }

//     ESP_LOGW(__func__, "");
// }

// // Invoked when usb bus is suspended
// // remote_wakeup_en : if host allows us to perform remote wakeup
// // USB Specs: Within 7ms, device must draw an average current less than 2.5 mA from bus
// void tud_suspend_cb(bool remote_wakeup_en)
// {
//     if (cb_unmount[0])
//     {
//         cb_unmount[0](0, NULL);
//     }
//     ESP_LOGW(__func__, "");
// }

// // Invoked when usb bus is resumed
// void tud_resume_cb(void)
// {
//     ESP_LOGW(__func__, "");
// }

// // Invoked to determine max LUN
// uint8_t tud_msc_get_maxlun_cb(void)
// {
//   return LOGICAL_DISK_NUM; // dual LUN
// }

// // Callback invoked when WRITE10 command is completed (status received and accepted by host).
// // used to flush any pending cache.
// void tud_msc_write10_complete_cb(uint8_t lun)
// {
//     if (unlikely(lun >= LOGICAL_DISK_NUM)) {
//         ESP_LOGE(__func__, "invalid lun number %u", lun);
//         return;
//     }

//     // This write is complete, start the autoreload clock.
//     ESP_LOGD(__func__, "");
// }

// static bool _logical_disk_ejected(void)
// {
//     bool all_ejected = true;

//     for (uint8_t i = 0; i < LOGICAL_DISK_NUM; i++) {
//         all_ejected &= s_ejected[i];
//     }

//     return all_ejected;
// }

// // Invoked when received SCSI_CMD_INQUIRY
// // Application fill vendor id, product id and revision with string up to 8, 16, 4 characters respectively
// void tud_msc_inquiry_cb(uint8_t lun, uint8_t vendor_id[8], uint8_t product_id[16], uint8_t product_rev[4])
// {
//     ESP_LOGD(__func__, "");

//     if (unlikely(lun >= LOGICAL_DISK_NUM)) {
//         ESP_LOGE(__func__, "invalid lun number %u", lun);
//         return;
//     }

//     const char vid[] = "Espressif";
//     const char pid[] = "Mass Storage";
//     const char rev[] = "1.0";

//     memcpy(vendor_id, vid, strlen(vid));
//     memcpy(product_id, pid, strlen(pid));
//     memcpy(product_rev, rev, strlen(rev));

// }

// // Invoked when received Test Unit Ready command.
// // return true allowing host to read/write this LUN e.g SD card inserted
// bool tud_msc_test_unit_ready_cb(uint8_t lun)
// {
//     ESP_LOGD(__func__, "");

//     if (unlikely(lun >= LOGICAL_DISK_NUM)) {
//         ESP_LOGE(__func__, "invalid lun number %u", lun);
//         return false;
//     }

//     if (_logical_disk_ejected()) {
//         // Set 0x3a for media not present.
//         tud_msc_set_sense(lun, SCSI_SENSE_NOT_READY, 0x3A, 0x00);
//         return false;
//     }

//     return true;
// }

// // Invoked when received SCSI_CMD_READ_CAPACITY_10 and SCSI_CMD_READ_FORMAT_CAPACITY to determine the disk size
// // Application update block count and block size
// void tud_msc_capacity_cb(uint8_t lun, uint32_t *block_count, uint16_t *block_size)
// {
//     ESP_LOGD(__func__, "");

//     if (unlikely(lun >= LOGICAL_DISK_NUM)) {
//         ESP_LOGE(__func__, "invalid lun number %u", lun);
//         return;
//     }

//     disk_ioctl(msc_pdrv, GET_SECTOR_COUNT, block_count);
//     disk_ioctl(msc_pdrv, GET_SECTOR_SIZE, block_size);
//     s_disk_block_size[lun] = *block_size;
//     ESP_LOGD(__func__, "lun = %u GET_SECTOR_COUNT = %d，GET_SECTOR_SIZE = %d",lun, *block_count, *block_size);
// }

// bool tud_msc_is_writable_cb(uint8_t lun)
// {
//     ESP_LOGD(__func__, "");

//     if (unlikely(lun >= LOGICAL_DISK_NUM)) {
//         ESP_LOGE(__func__, "invalid lun number %u", lun);
//         return false;
//     }

//     return true;
// }

// // Invoked when received Start Stop Unit command
// // - Start = 0 : stopped power mode, if load_eject = 1 : unload disk storage
// // - Start = 1 : active mode, if load_eject = 1 : load disk storage
// bool tud_msc_start_stop_cb(uint8_t lun, uint8_t power_condition, bool start, bool load_eject)
// {
//     ESP_LOGI(__func__, "");
//     (void) power_condition;

//     if (unlikely(lun >= LOGICAL_DISK_NUM)) {
//         ESP_LOGE(__func__, "invalid lun number %u", lun);
//         return false;
//     }

//     if (load_eject) {
//         if (!start) {
//             // Eject but first flush.
//             if (disk_ioctl(msc_pdrv, CTRL_SYNC, NULL) != RES_OK) {
//                 return false;
//             } else {
//                 s_ejected[lun] = true;
//             }
//         } else {
//             // We can only load if it hasn't been ejected.
//             return !s_ejected[lun];
//         }
//     } else {
//         if (!start) {
//             // Stop the unit but don't eject.
//             if (disk_ioctl(msc_pdrv, CTRL_SYNC, NULL) != RES_OK) {
//                 return false;
//             }
//         }

//         // Always start the unit, even if ejected. Whether media is present is a separate check.
//     }

//     return true;
// }

// // Callback invoked when received READ10 command.
// // Copy disk's data to buffer (up to bufsize) and return number of copied bytes.
// IRAM_ATTR int32_t tud_msc_read10_cb(uint8_t lun, uint32_t lba, uint32_t offset, void *buffer, uint32_t bufsize)
// {
//     ESP_LOGD(__func__, "");

//     if (unlikely(lun >= LOGICAL_DISK_NUM)) {
//         ESP_LOGE(__func__, "invalid lun number %u", lun);
//         return 0;
//     }

//     const uint32_t block_count = bufsize / s_disk_block_size[lun];
//     disk_read(msc_pdrv, buffer, lba, block_count);
//     return block_count * s_disk_block_size[lun];
// }

// // Callback invoked when received WRITE10 command.
// // Process data in buffer to disk's storage and return number of written bytes
// IRAM_ATTR int32_t tud_msc_write10_cb(uint8_t lun, uint32_t lba, uint32_t offset, uint8_t *buffer, uint32_t bufsize)
// {
//     ESP_LOGD(__func__, "");
//     (void) offset;

//     if (unlikely(lun >= LOGICAL_DISK_NUM)) {
//         ESP_LOGE(__func__, "invalid lun number %u", lun);
//         return 0;
//     }

//     const uint32_t block_count = bufsize / s_disk_block_size[lun];
//     disk_write(msc_pdrv, buffer, lba, block_count);
//     return block_count * s_disk_block_size[lun];
// }

// // Callback invoked when received an SCSI command not in built-in list below
// // - READ_CAPACITY10, READ_FORMAT_CAPACITY, INQUIRY, MODE_SENSE6, REQUEST_SENSE
// // - READ10 and WRITE10 has their own callbacks
// int32_t tud_msc_scsi_cb(uint8_t lun, uint8_t const scsi_cmd[16], void *buffer, uint16_t bufsize)
// {
//     // read10 & write10 has their own callback and MUST not be handled here
//     ESP_LOGD(__func__, "");

//     if (unlikely(lun >= LOGICAL_DISK_NUM)) {
//         ESP_LOGE(__func__, "invalid lun number %u", lun);
//         return 0;
//     }

//     void const *response = NULL;
//     uint16_t resplen = 0;

//     // most scsi handled is input
//     bool in_xfer = true;

//     switch (scsi_cmd[0]) {
//         case SCSI_CMD_PREVENT_ALLOW_MEDIUM_REMOVAL:
//             // Host is about to read/write etc ... better not to disconnect disk
//             resplen = 0;
//             break;

//         case SCSI_CMD_START_STOP_UNIT:
//         // Host try to eject/safe remove/poweroff us. We could safely disconnect with disk storage, or go into lower power
//         /* scsi_start_stop_unit_t const * start_stop = (scsi_start_stop_unit_t const *) scsi_cmd;
//             // Start bit = 0 : low power mode, if load_eject = 1 : unmount disk storage as well
//             // Start bit = 1 : Ready mode, if load_eject = 1 : mount disk storage
//             start_stop->start;
//             start_stop->load_eject;
//         */
//         resplen = 0;
//         break;

//         default:
//             // Set Sense = Invalid Command Operation
//             tud_msc_set_sense(lun, SCSI_SENSE_ILLEGAL_REQUEST, 0x20, 0x00);

//             // negative means error -> tinyusb could stall and/or response with failed status
//             resplen = -1;
//             break;
//     }

//     // return resplen must not larger than bufsize
//     if (resplen > bufsize) {
//         resplen = bufsize;
//     }

//     if (response && (resplen > 0)) {
//         if (in_xfer) {
//             memcpy(buffer, response, resplen);
//         } else {
//             // SCSI output
//         }
//     }

//     return resplen;
// }

// #endif

#include "esp_partition.h"

#define BLOCK_SIZE          512

#define LOGICAL_DISK_NUM 1
static bool ejected[LOGICAL_DISK_NUM] = {true};

const esp_partition_t *partition;

void usb_msc_init(void)
{
    partition = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, "vfs");
    assert(partition != NULL);
    ESP_LOGI(__func__, "");

}

//--------------------------------------------------------------------+
// tinyusb callbacks
//--------------------------------------------------------------------+

// Invoked when device is mounted
void tud_mount_cb(void)
{
    // Reset the ejection tracking every time we're plugged into USB. This allows for us to battery
    // power the device, eject, unplug and plug it back in to get the drive.
    for (uint8_t i = 0; i < LOGICAL_DISK_NUM; i++) {
        ejected[i] = false;
    }

    ESP_LOGD(__func__, "");
}

// Invoked when device is unmounted
void tud_umount_cb(void)
{
    ESP_LOGW(__func__, "");
}

// Invoked when usb bus is suspended
// remote_wakeup_en : if host allows us to perform remote wakeup
// USB Specs: Within 7ms, device must draw an average current less than 2.5 mA from bus
void tud_suspend_cb(bool remote_wakeup_en)
{
    ESP_LOGW(__func__, "");
}

// Invoked when usb bus is resumed
void tud_resume_cb(void)
{
    ESP_LOGW(__func__, "");
}

// Callback invoked when WRITE10 command is completed (status received and accepted by host).
// used to flush any pending cache.
void tud_msc_write10_complete_cb(uint8_t lun)
{
    if (lun >= LOGICAL_DISK_NUM) {
        ESP_LOGE(__func__, "invalid lun number %u", lun);
        return;
    }

    // This write is complete, start the autoreload clock.
    ESP_LOGD(__func__, "");
}

static bool _logical_disk_ejected(void)
{
    bool all_ejected = true;

    for (uint8_t i = 0; i < LOGICAL_DISK_NUM; i++) {
        all_ejected &= ejected[i];
    }

    return all_ejected;
}

// Invoked when received SCSI_CMD_INQUIRY
// Application fill vendor id, product id and revision with string up to 8, 16, 4 characters respectively
void tud_msc_inquiry_cb(uint8_t lun, uint8_t vendor_id[8], uint8_t product_id[16], uint8_t product_rev[4])
{
    ESP_LOGD(__func__, "");

    if (lun >= LOGICAL_DISK_NUM) {
        ESP_LOGE(__func__, "invalid lun number %u", lun);
        return;
    }

    const char vid[] = "Espressif";
    const char pid[] = "Mass Storage";
    const char rev[] = "1.0";

    memcpy(vendor_id, vid, strlen(vid));
    memcpy(product_id, pid, strlen(pid));
    memcpy(product_rev, rev, strlen(rev));
}

// Invoked when received Test Unit Ready command.
// return true allowing host to read/write this LUN e.g SD card inserted
bool tud_msc_test_unit_ready_cb(uint8_t lun)
{
    ESP_LOGD(__func__, "");

    if (lun >= LOGICAL_DISK_NUM) {
        ESP_LOGE(__func__, "invalid lun number %u", lun);
        return false;
    }

    if (_logical_disk_ejected()) {
        // Set 0x3a for media not present.
        tud_msc_set_sense(lun, SCSI_SENSE_NOT_READY, 0x3A, 0x00);
        return false;
    }

    return true;
}

// Invoked when received SCSI_CMD_READ_CAPACITY_10 and SCSI_CMD_READ_FORMAT_CAPACITY to determine the disk size
// Application update block count and block size
void tud_msc_capacity_cb(uint8_t lun, uint32_t *block_count, uint16_t *block_size)
{
    ESP_LOGD(__func__, "");

    if (lun >= LOGICAL_DISK_NUM) {
        ESP_LOGE(__func__, "invalid lun number %u", lun);
        return;
    }

    *block_size = BLOCK_SIZE;
    *block_count = partition->size/BLOCK_SIZE;
    ESP_LOGD(__func__, "GET_SECTOR_COUNT = %d, GET_SECTOR_SIZE = %d", *block_count, *block_size);
}

bool tud_msc_is_writable_cb(uint8_t lun)
{
    ESP_LOGD(__func__, "");

    if (lun >= LOGICAL_DISK_NUM) {
        ESP_LOGE(__func__, "invalid lun number %u", lun);
        return false;
    }

    return true;
}

// Invoked when received Start Stop Unit command
// - Start = 0 : stopped power mode, if load_eject = 1 : unload disk storage
// - Start = 1 : active mode, if load_eject = 1 : load disk storage
bool tud_msc_start_stop_cb(uint8_t lun, uint8_t power_condition, bool start, bool load_eject)
{
    ESP_LOGD(__func__, "");
    (void) power_condition;

    if (lun >= LOGICAL_DISK_NUM) {
        ESP_LOGE(__func__, "invalid lun number %u", lun);
        return false;
    }

    if (load_eject) {
        if (!start) {
            ejected[lun] = true;
        } else {
            // We can only load if it hasn't been ejected.
            ejected[lun] = false;
        }
    } else {
        // Always start the unit, even if ejected. Whether media is present is a separate check.
    }

    return true;
}

// Callback invoked when received READ10 command.
// Copy disk's data to buffer (up to bufsize) and return number of copied bytes.
int32_t tud_msc_read10_cb(uint8_t lun, uint32_t lba, uint32_t offset, void *buffer, uint32_t bufsize)
{
    ESP_LOGD(__func__, "");

    if (lun >= LOGICAL_DISK_NUM) {
        ESP_LOGE(__func__, "invalid lun number %u", lun);
        return 0;
    }

    ESP_ERROR_CHECK(esp_partition_read(partition, lba * BLOCK_SIZE, buffer, bufsize));
    return bufsize;
}

// Callback invoked when received WRITE10 command.
// Process data in buffer to disk's storage and return number of written bytes
int32_t tud_msc_write10_cb(uint8_t lun, uint32_t lba, uint32_t offset, uint8_t *buffer, uint32_t bufsize)
{
    ESP_LOGD(__func__, "");
    (void) offset;

    if (lun >= LOGICAL_DISK_NUM) {
        ESP_LOGE(__func__, "invalid lun number %u", lun);
        return 0;
    }

    ESP_ERROR_CHECK(esp_partition_erase_range(partition, lba * BLOCK_SIZE, bufsize));
    ESP_ERROR_CHECK(esp_partition_write(partition, lba * BLOCK_SIZE, buffer, bufsize));
    return bufsize;
}

// Callback invoked when received an SCSI command not in built-in list below
// - READ_CAPACITY10, READ_FORMAT_CAPACITY, INQUIRY, MODE_SENSE6, REQUEST_SENSE
// - READ10 and WRITE10 has their own callbacks
int32_t tud_msc_scsi_cb(uint8_t lun, uint8_t const scsi_cmd[16], void *buffer, uint16_t bufsize)
{
    // read10 & write10 has their own callback and MUST not be handled here
    ESP_LOGD(__func__, "");

    if (lun >= LOGICAL_DISK_NUM) {
        ESP_LOGE(__func__, "invalid lun number %u", lun);
        return 0;
    }

    void const *response = NULL;
    uint16_t resplen = 0;

    // most scsi handled is input
    bool in_xfer = true;

    switch (scsi_cmd[0]) {
        case SCSI_CMD_PREVENT_ALLOW_MEDIUM_REMOVAL:
            // Host is about to read/write etc ... better not to disconnect disk
            resplen = 0;
            break;

        default:
            // Set Sense = Invalid Command Operation
            tud_msc_set_sense(lun, SCSI_SENSE_ILLEGAL_REQUEST, 0x20, 0x00);

            // negative means error -> tinyusb could stall and/or response with failed status
            resplen = -1;
            break;
    }

    // return resplen must not larger than bufsize
    if (resplen > bufsize) {
        resplen = bufsize;
    }

    if (response && (resplen > 0)) {
        if (in_xfer) {
            memcpy(buffer, response, resplen);
        } else {
            // SCSI output
        }
    }

    return resplen;
}
#endif // CONFIG_USB_ENABLED
