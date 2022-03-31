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
#include "ns2016.h"
#include "tp_i2c.h"

#define TAG "ns2016"


ns2016_status_t ns2016_status;
uint8_t current_dev_addr;       // set during init

esp_err_t i2c_master_read_reg(i2c_port_t i2c_num, uint8_t slaver_addr, uint8_t *data)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();

    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (slaver_addr << 1) | I2C_MASTER_READ, true);

    i2c_master_read_byte(cmd, data, I2C_MASTER_NACK);
    i2c_master_stop(cmd);

    esp_err_t ret = i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    return ret;
}

esp_err_t i2c_master_read_xy(i2c_port_t i2c_num, uint8_t slaver_addr, uint8_t *buffer)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();

    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (slaver_addr << 1) | I2C_MASTER_READ, true);
    i2c_master_read_byte(cmd, buffer, I2C_MASTER_ACK);
    i2c_master_read_byte(cmd, buffer+1, I2C_MASTER_NACK); //the lastest byte will not give a ASK
    i2c_master_stop(cmd);

    esp_err_t ret = i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    return ret;
}

esp_err_t i2c_master_write_reg(i2c_port_t i2c_num, uint8_t slaver_addr, uint8_t data)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();

    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (slaver_addr << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write(cmd, &data, 1, true);
    // i2c_master_write_byte(cmd, data, true);
    i2c_master_stop(cmd);

    esp_err_t ret = i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    return ret;
}

/**
  * @brief  Initialize for NS2016 communication via I2C
  * @param  dev_addr: Device address on communication Bus (I2C slave address of NS2016).
  * @retval None
  */
void ns2016_init(uint16_t dev_addr) {
    if (!ns2016_status.inited) {

/* I2C master is initialized before calling this function */
#if 0
        esp_err_t code = i2c_master_init();
#else
        esp_err_t code = ESP_OK;
#endif

        if (code != ESP_OK) {
            ns2016_status.inited = false;
            ESP_LOGE(TAG, "Error during I2C init %s", esp_err_to_name(code));
        } 
        else 
        {
            ns2016_status.inited = true;
            current_dev_addr = dev_addr;
            ESP_LOGI(TAG, "Found touch panel controller");
        }
    }
}

/**
  * @brief  Get the touch screen X and Y positions values. Ignores multi touch
  * @param  drv:
  * @param  data: Store data here
  * @retval Always false
  */
bool ns2016_read(lv_indev_drv_t *drv, lv_indev_data_t *data) {
    esp_err_t ret;
    static int16_t last_x = 0;    // 12bit pixel value
    static int16_t last_y = 0;    // 12bit pixel value

    //Read X value
    ret = i2c_master_write_reg(I2C_NUM_0, current_dev_addr, NS2016_READ_X);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Error i2c_master_write_reg: %s", esp_err_to_name(ret));
        data->point.x = last_x;
        data->point.y = last_y;
        data->state = LV_INDEV_STATE_REL;   // no touch detected
        return false;
    }
    uint8_t x_value[2] = {0, 0};
    ret = i2c_master_read_xy(I2C_NUM_0, current_dev_addr, x_value);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Error getting X coordinates: %s", esp_err_to_name(ret));
        data->point.x = last_x;
        data->point.y = last_y;
        data->state = LV_INDEV_STATE_REL;   // no touch detected
        return false;
    }

    //Read Y value
    ret = i2c_master_write_reg(I2C_NUM_0, current_dev_addr, NS2016_READ_Y);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Error i2c_master_write_reg: %s", esp_err_to_name(ret));
        data->point.x = last_x;
        data->point.y = last_y;
        data->state = LV_INDEV_STATE_REL;   // no touch detected
        return false;
    }
    uint8_t y_value[2] = {0, 0};
    ret = i2c_master_read_xy(I2C_NUM_0, current_dev_addr, y_value);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Error getting Y coordinates: %s", esp_err_to_name(ret));
        data->point.x = last_x;
        data->point.y = last_y;
        data->state = LV_INDEV_STATE_REL;   // no touch detected
        return false;
    }

    // last_x = ((x_value[0] & NS2016_MSB_MASK) << 4) | ((x_value[1] & NS2016_LSB_MASK) >> 4);
    // last_y = ((y_value[0] & NS2016_MSB_MASK) << 4) | ((y_value[1] & NS2016_LSB_MASK) >> 4);
    last_x = x_value[0];
    last_y = y_value[0];

    // last_x = x_value;
    // last_y = y_value;

// #if CONFIG_LV_NS2016_SWAPXY
//     int16_t swap_buf = last_x;
//     last_x = last_y;
//     last_y = swap_buf;
// #endif
// #if CONFIG_LV_NS2016_INVERT_X
//     last_x =  LV_HOR_RES - last_x;
// #endif
// #if CONFIG_LV_NS2016_INVERT_Y
//     last_y = LV_VER_RES - last_y;
// #endif
    data->point.x = last_x;
    data->point.y = last_y;
    data->state = LV_INDEV_STATE_PR;
    // ESP_LOGV(TAG, "X=%u Y=%u", data->point.x, data->point.y);
    ESP_LOGE(TAG, "X=%u Y=%u", data->point.x, data->point.y);
    return false;
}
