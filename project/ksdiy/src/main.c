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
#include <string.h>
#include <stdarg.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_task.h"
#include "soc/cpu.h"
#include "esp_log.h"

#if CONFIG_IDF_TARGET_ESP32
#include "esp32/spiram.h"
#elif CONFIG_IDF_TARGET_ESP32S2
#include "esp32s2/spiram.h"
#elif CONFIG_IDF_TARGET_ESP32S3
#include "esp32s3/spiram.h"
#endif

#include "py/stackctrl.h"
#include "py/nlr.h"
#include "py/compile.h"
#include "py/runtime.h"
#include "py/persistentcode.h"
#include "py/repl.h"
#include "py/gc.h"
#include "py/mphal.h"
#include "shared/runtime/interrupt_char.h"
#include "shared/runtime/pyexec.h"
#include "shared/readline/readline.h"
#include "uart.h"
#include "uart1.h"
#include "usb_cdc.h"
#include "modmachine.h"
#include "modnetwork.h"
#include "mpthreadport.h"
#include "omv_sensor.h"
#include "fb_alloc.h"
#include "framebuffer.h"
#include "usbdbg.h"
// #include "drv_qmi8658.h"
// #include "drv_qmc6310.h"
#include "who_lcd.h"
// #include "esp_camera.h"
#if MICROPY_BLUETOOTH_NIMBLE
#include "extmod/modbluetooth.h"
#endif

// MicroPython runs as a task under FreeRTOS
#define MP_TASK_PRIORITY        (ESP_TASK_PRIO_MIN + 1)
#define MP_TASK_STACK_SIZE      (16 * 1024)

int vprintf_null(const char *format, va_list ap) {
    // do nothing: this is used as a log target during raw repl mode
    return 0;
}

void mp_task(void *pvParameter) {
    volatile uint32_t sp = (uint32_t)get_sp();
    register_lcd(NULL, NULL, false);
    #if MICROPY_PY_THREAD
    mp_thread_init(pxTaskGetStackStart(NULL), MP_TASK_STACK_SIZE / sizeof(uintptr_t));
    #endif
    // #if CONFIG_USB_ENABLED
    usb_cdc_init();
    usb_msc_init();
    // #endif
    uart_init();
    
    machine_init();
    framebuffer_init0();
    fb_alloc_init0();
    
    // sensor_init();

    // TODO: CONFIG_SPIRAM_SUPPORT is for 3.3 compatibility, remove after move to 4.0.
    #if CONFIG_ESP32_SPIRAM_SUPPORT || CONFIG_SPIRAM_SUPPORT
    // Try to use the entire external SPIRAM directly for the heap
    size_t mp_task_heap_size;
    void *mp_task_heap = (void *)SOC_EXTRAM_DATA_LOW;
    switch (esp_spiram_get_chip_size()) {
        case ESP_SPIRAM_SIZE_16MBITS:
            mp_task_heap_size = 2 * 1024 * 1024;
            break;
        case ESP_SPIRAM_SIZE_32MBITS:
        case ESP_SPIRAM_SIZE_64MBITS:
            mp_task_heap_size = 4 * 1024 * 1024;
            break;
        default:
            // No SPIRAM, fallback to normal allocation
            mp_task_heap_size = heap_caps_get_largest_free_block(MALLOC_CAP_8BIT);
            mp_task_heap = malloc(mp_task_heap_size);
            break;
    }
    #elif CONFIG_ESP32S2_SPIRAM_SUPPORT
    // Try to use the entire external SPIRAM directly for the heap
    size_t mp_task_heap_size;
    size_t esp_spiram_size = esp_spiram_get_size();
    void *mp_task_heap = (void *)SOC_EXTRAM_DATA_HIGH - esp_spiram_size;
    if (esp_spiram_size > 0) {
        mp_task_heap_size = esp_spiram_size;
    } else {
        // No SPIRAM, fallback to normal allocation
        mp_task_heap_size = heap_caps_get_largest_free_block(MALLOC_CAP_8BIT);
        mp_task_heap = malloc(mp_task_heap_size);
    }
    #elif CONFIG_ESP32S3_SPIRAM_SUPPORT
    size_t mp_task_heap_size = 2 * 1024 * 1024;
    size_t esp_spiram_size = esp_spiram_get_size();
    void *mp_task_heap = NULL;
    ESP_LOGI("mp_task", "esp_spiram_size[%d]", esp_spiram_size);
    // void *mp_task_heap = (void *)SOC_EXTRAM_DATA_HIGH - esp_spiram_size;
    if (esp_spiram_size > 0) {
        mp_task_heap = heap_caps_malloc(mp_task_heap_size, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
        ESP_LOGI("mp_task=", "mp_task_heap(%p), mp_task_heap_size[%d]\n",mp_task_heap, mp_task_heap_size);
        if(mp_task_heap == NULL)
        {
            ESP_LOGI("mp_task", "malloc heap for mp task failed, heap size if %d\n", mp_task_heap_size);
            vTaskDelay(2000 / portTICK_PERIOD_MS);
        }
    } else {
        // No SPIRAM, fallback to normal allocation
        mp_task_heap_size = heap_caps_get_largest_free_block(MALLOC_CAP_8BIT);
        mp_task_heap = malloc(mp_task_heap_size);
    }
    #else
    // Allocate the uPy heap using malloc and get the largest available region
    size_t mp_task_heap_size = heap_caps_get_largest_free_block(MALLOC_CAP_8BIT);
    void *mp_task_heap = malloc(mp_task_heap_size);
    #endif

soft_reset:
    // initialise the stack pointer for the main thread
    mp_stack_set_top((void *)sp);
    mp_stack_set_limit(MP_TASK_STACK_SIZE - 1024);
    gc_init(mp_task_heap, mp_task_heap + mp_task_heap_size);
    mp_init();
    // mp_obj_list_init(mp_sys_path, 0);
    // mp_obj_list_append(mp_sys_path, MP_OBJ_NEW_QSTR(MP_QSTR_));
    mp_obj_list_append(mp_sys_path, MP_OBJ_NEW_QSTR(MP_QSTR__slash_lib));
    // mp_obj_list_init(mp_sys_argv, 0);
    readline_init0();

    // initialise peripherals
    machine_pins_init();
    // run boot-up scripts
    pyexec_frozen_module("_boot.py");
    sensor_init();
    pyexec_file_if_exists("boot.py");    
    //pyexec_file_if_exists("system.py");
    int ret = pyexec_file_if_exists("main.py");
    if (ret & PYEXEC_FORCED_EXIT) {
        printf("soft_reset_exit\r\n");
        goto soft_reset_exit;
    }
 repl_again:
    ESP_LOGD("mp_task", "mp_interrupt_char 0x%X", mp_interrupt_char);
    usbdbg_init();
    // If there's no script ready, just re-exec REPL
    while (!usbdbg_script_ready()) {
        nlr_buf_t nlr;
        vTaskDelay(10 / portTICK_PERIOD_MS);
        if (nlr_push(&nlr) == 0) {
            // enable IDE interrupt
            usbdbg_set_irq_enabled(true);
            // run REPL
            if (pyexec_mode_kind == PYEXEC_MODE_RAW_REPL) {
                if (pyexec_raw_repl() != 0) {
                    break;
                }
            } else {
                if (pyexec_friendly_repl() != 0) {
                    break;
                }
            }
            nlr_pop();
        }
    }

    if (usbdbg_script_ready()) {
        nlr_buf_t nlr;
        if (nlr_push(&nlr) == 0) {
            // Enable IDE interrupt
            usbdbg_set_irq_enabled(true);

            // Execute the script.
            ESP_LOGI("mp_task", "Execute the script");
            pyexec_str(usbdbg_get_script());
            nlr_pop();
        } else {
            ESP_LOGI("mp_task", "mp_obj_print_exception");
            mp_obj_print_exception(&mp_plat_print, (mp_obj_t)nlr.ret_val);
        }
    }
    ESP_LOGI("mp_task", "usbdbg_wait_for_command");
    usbdbg_wait_for_command(1000);

    bool script_sta = usbdbg_handle_script_status();
    ESP_LOGI("mp_task", "script_status %d", script_sta);
    if(script_sta) {
      goto repl_again;
    }
    goto soft_reset_exit;
soft_reset_exit:

    #if MICROPY_BLUETOOTH_NIMBLE
    mp_bluetooth_deinit();
    #endif
    machine_timer_deinit_all();
    #if MICROPY_PY_THREAD
    mp_thread_deinit();
    #endif
    gc_sweep_all();
    mp_hal_stdout_tx_str("MPY: soft reboot\r\n");
    esp32_sensor_deinit();
    // esp_camera_deinit();
    // vTaskDelay(200);
    // deinitialise peripherals
    machine_pins_deinit();
    machine_deinit();
    usocket_events_deinit();

    mp_deinit();
    fflush(stdout);
    goto soft_reset;
}

void get_cmd_data()
{
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    uart1_init();
    while (1)
    {
        
        uart1_read_data();
        
        vTaskDelay(50 / portTICK_PERIOD_MS);
    }
}

void app_main(void) {
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        nvs_flash_erase();
        nvs_flash_init();
    }
    xTaskCreatePinnedToCore(mp_task, "mp_task", MP_TASK_STACK_SIZE / sizeof(StackType_t), NULL, MP_TASK_PRIORITY, &mp_main_task_handle, MP_TASK_COREID);
    xTaskCreatePinnedToCore(get_cmd_data, "get_cmd_data", 4 * 1024, NULL, MP_TASK_PRIORITY, &mp_main_task_handle, MP_TASK_COREID);
}

void nlr_jump_fail(void *val) {
    printf("NLR jump failed, val=%p\n", val);
    esp_restart();
}

// modussl_mbedtls uses this function but it's not enabled in ESP IDF
void mbedtls_debug_set_threshold(int threshold) {
    (void)threshold;
}

void *esp_native_code_commit(void *buf, size_t len, void *reloc) {
    len = (len + 3) & ~3;
    uint32_t *p = heap_caps_malloc(len, MALLOC_CAP_EXEC);
    if (p == NULL) {
        m_malloc_fail(len);
    }
    if (reloc) {
        mp_native_relocate(reloc, buf, (uintptr_t)p);
    }
    memcpy(p, buf, len);
    return p;
}
