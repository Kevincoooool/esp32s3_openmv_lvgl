/*
* Copyright © 2020 Wolfgang Christl

* Permission is hereby granted, free of charge, to any person obtaining a copy of this 
* software and associated documentation files (the “Software”), to deal in the Software 
* without restriction, including without limitation the rights to use, copy, modify, merge, 
* publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons 
* to whom the Software is furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all copies or 
* substantial portions of the Software.
* 
* THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
* INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR 
* PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE 
* FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, 
* ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE 
* SOFTWARE.
*/

#include <esp_log.h>
#include <driver/i2c.h>
#include <lvgl/lvgl.h>
#include "gt615.h"
#include "tp_i2c.h"

#define TAG "gt615"


gt615_status_t gt615_status;
uint8_t current_dev_addr;       // set during init

#define GPIO_TOUCH_INT_IO    CONFIG_LV_TOUCH_PEN_IRQ
#define GPIO_TOUCH_RST_IO    CONFIG_LV_TOUCH_RST

esp_err_t i2c_master_read_data(i2c_port_t i2c_num, uint8_t slaver_addr, uint16_t reg_addr, uint8_t *buffer, uint16_t size)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();

    i2c_master_start(cmd);

    i2c_master_write_byte(cmd, (slaver_addr << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, (reg_addr >> 8) & 0xff, true);
    i2c_master_write_byte(cmd, reg_addr & 0xff, true);
    i2c_master_stop(cmd);

    esp_err_t ret = i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);

    cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (slaver_addr << 1) | I2C_MASTER_READ, true);

    if(size > 1) 
    {
        i2c_master_read(cmd, buffer, size - 1, I2C_MASTER_ACK);
    }
    i2c_master_read_byte(cmd, buffer + size - 1, I2C_MASTER_NACK); //the lastest byte will not give a ASK
    i2c_master_stop(cmd);

    ret = i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    return ret;
}

esp_err_t i2c_master_write_data(i2c_port_t i2c_num, uint8_t slaver_addr, uint16_t reg_addr, uint8_t *buffer, uint16_t size)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();

    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (slaver_addr << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, (reg_addr >> 8) & 0xff, true);
    i2c_master_write_byte(cmd, reg_addr & 0xff, true);

    for(uint16_t i = 0; i < size; i++)
    {
        i2c_master_write(cmd, (buffer + i), 1, true);
    }
    i2c_master_stop(cmd);

    esp_err_t ret = i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    return ret;
}

/**
  * @brief  Initialize for GT615 communication via I2C
  * @param  dev_addr: Device address on communication Bus (I2C slave address of GT615).
  * @retval None
  */
void gt615_init(uint16_t dev_addr) {
    if (!gt615_status.inited) 
    {
        esp_err_t code = ESP_OK;
        gpio_config_t io_conf;
        //disable interrupt
        io_conf.intr_type = GPIO_PIN_INTR_DISABLE;
        //set as output mode
        io_conf.mode = GPIO_MODE_OUTPUT;
        io_conf.pin_bit_mask = (1ULL<<GPIO_TOUCH_RST_IO);
        //disable pull-down mode
        io_conf.pull_down_en = 0;
        //disable pull-up mode
        io_conf.pull_up_en = 1;
        //configure GPIO with the given settings
        gpio_config(&io_conf);
        io_conf.pin_bit_mask = (1ULL<<GPIO_TOUCH_INT_IO);
        io_conf.pull_up_en = 0;
        gpio_config(&io_conf);

        gpio_set_level(GPIO_TOUCH_INT_IO, 0);
        gpio_set_level(GPIO_TOUCH_RST_IO, 0);
        gpio_set_level(GPIO_TOUCH_INT_IO, 1);
        ets_delay_us(200);
        gpio_set_level(GPIO_TOUCH_RST_IO, 1);
        ets_delay_us(6000);
        gpio_set_level(GPIO_TOUCH_INT_IO, 0);
        vTaskDelay(60 / portTICK_RATE_MS);
        uint8_t data[11];
        code = i2c_master_read_data(I2C_NUM_0, GT615_I2C_SLAVE_ADDR, 0x8000, data, 10);
        data[11] = '\0';
        if (code != ESP_OK)
        {
            gt615_status.inited = false;
            ESP_LOGE(TAG, "Error during I2C init %s", esp_err_to_name(code));
        } 
        else 
        {
            gt615_status.inited = true;
            current_dev_addr = dev_addr;
            ESP_LOGI(TAG, "Found touch panel controller");
            ESP_LOGE(TAG, "%s", data);
        }
    }
}

/**
  * @brief  Get the touch screen X and Y positions values. Ignores multi touch
  * @param  drv:
  * @param  data: Store data here
  * @retval Always false
  */
bool gt615_read(lv_indev_drv_t *drv, lv_indev_data_t *data) {
    esp_err_t ret;
//     static int16_t last_x = 0;    // 12bit pixel value
//     static int16_t last_y = 0;    // 12bit pixel value

//     //Read X value
//     ret = i2c_master_write_reg(I2C_NUM_0, current_dev_addr, NS2016_READ_X);
//     if (ret != ESP_OK) {
//         ESP_LOGE(TAG, "Error i2c_master_write_reg: %s", esp_err_to_name(ret));
//         data->point.x = last_x;
//         data->point.y = last_y;
//         data->state = LV_INDEV_STATE_REL;   // no touch detected
//         return false;
//     }
//     uint8_t x_value[2] = {0, 0};
//     // uint8_t x_value;
//     ret = i2c_master_read_xy(I2C_NUM_0, current_dev_addr, x_value);
//     if (ret != ESP_OK) {
//         ESP_LOGE(TAG, "Error getting X coordinates: %s", esp_err_to_name(ret));
//         data->point.x = last_x;
//         data->point.y = last_y;
//         data->state = LV_INDEV_STATE_REL;   // no touch detected
//         return false;
//     }

//     //Read Y value
//     ret = i2c_master_write_reg(I2C_NUM_0, current_dev_addr, NS2016_READ_Y);
//     if (ret != ESP_OK) {
//         ESP_LOGE(TAG, "Error i2c_master_write_reg: %s", esp_err_to_name(ret));
//         data->point.x = last_x;
//         data->point.y = last_y;
//         data->state = LV_INDEV_STATE_REL;   // no touch detected
//         return false;
//     }
//     uint8_t y_value[2] = {0, 0};
//     // uint8_t x_value;
//     ret = i2c_master_read_xy(I2C_NUM_0, current_dev_addr, y_value);
//     if (ret != ESP_OK) {
//         ESP_LOGE(TAG, "Error getting Y coordinates: %s", esp_err_to_name(ret));
//         data->point.x = last_x;
//         data->point.y = last_y;
//         data->state = LV_INDEV_STATE_REL;   // no touch detected
//         return false;
//     }

//     // last_x = ((x_value[0] & NS2016_MSB_MASK) << 4) | ((x_value[1] & NS2016_LSB_MASK) >> 4);
//     // last_y = ((y_value[0] & NS2016_MSB_MASK) << 4) | ((y_value[1] & NS2016_LSB_MASK) >> 4);
//     last_x = x_value[0];
//     last_y = y_value[0];
//     // last_x = x_value;
//     // last_y = y_value;

//     // last_x = x_value;
//     // last_y = y_value;

// // #if CONFIG_LV_NS2016_SWAPXY
// //     int16_t swap_buf = last_x;
// //     last_x = last_y;
// //     last_y = swap_buf;
// // #endif
// // #if CONFIG_LV_NS2016_INVERT_X
// //     last_x =  LV_HOR_RES - last_x;
// // #endif
// // #if CONFIG_LV_NS2016_INVERT_Y
// //     last_y = LV_VER_RES - last_y;
// // #endif
//     data->point.x = last_x;
//     data->point.y = last_y;
//     data->state = LV_INDEV_STATE_PR;
//     // ESP_LOGV(TAG, "X=%u Y=%u", data->point.x, data->point.y);
//     ESP_LOGE(TAG, "X=%u Y=%u", data->point.x, data->point.y);
    return false;
}
