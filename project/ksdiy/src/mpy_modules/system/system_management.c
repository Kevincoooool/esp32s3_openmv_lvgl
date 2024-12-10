#include <stdio.h>
#include <math.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_system.h"

#include "home_key.h"

#include "system_management.h"

#include "drv_aw9523b.h"
#include "drv_rgb_led.h"

#include "drv_nvs.h"

extern void mp_hal_delay_ms(uint32_t ms);


static const char *TAG = "USER SYSTEM";

static int32_t start_py_value = 0;

void system_status_update(void)
{
    home_key_press_status_update();
}

void system_management_task(void *pvParameter)
{
    //aw9523b_init();
	nvs_read_i32(START_PY_NAMESPACE, START_PY_KEY, &start_py_value);
    ESP_LOGI("app_main", "start_py_value:%d", start_py_value);

    while(true)
    {
        system_status_update();
        if(home_key_pressed()){
            ESP_LOGI(TAG, "home key pressed");
            set_rgb_led_pwm(80, 0, 0);
            start_py_value +=1;
            if(start_py_value > 3){
                start_py_value = 0;
            }
            nvs_write_i32(START_PY_NAMESPACE, START_PY_KEY, start_py_value);
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            esp_restart();
        }else{
            set_rgb_led_pwm(0, 0, 0);
        }
        vTaskDelay(SYSTEM_POLLING_TIME / portTICK_PERIOD_MS);

    }
}
