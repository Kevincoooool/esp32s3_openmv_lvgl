
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_err.h"
#include "esp_log.h"
#include "driver/i2c.h"
#include "drv_i2c.h"

static const char *TAG = "DRV I2C";
static bool i2c_initialized[2] = {false, false};
static SemaphoreHandle_t i2c_hal_mutex = NULL;

#define I2C_HAL_ENTER  xSemaphoreTake(i2c_hal_mutex, portMAX_DELAY);       
#define I2C_HAL_EXIT  xSemaphoreGive(i2c_hal_mutex); 

/**
 * @brief i2c master initialization
 */
esp_err_t i2c_master_init(uint8_t i2c_num)
{
    int i2c_master_port = I2C0_MASTER_NUM;
    if(i2c_initialized[i2c_num] == true)
    {
        ESP_LOGD(TAG, "i2c driver has been Initialized");
        return ESP_OK;
    }
    i2c_config_t conf;
    conf.mode = I2C_MODE_MASTER;
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf.clk_flags = 0;
    if(i2c_num == I2C_NUM_1)
    {
        i2c_master_port = I2C1_MASTER_NUM;
        conf.sda_io_num = I2C1_MASTER_SDA_IO;        
        conf.scl_io_num = I2C1_MASTER_SCL_IO;
        conf.master.clk_speed = I2C1_MASTER_FREQ_HZ;
    }    
    else if(i2c_num == I2C_NUM_0)
    {
        i2c_master_port = I2C0_MASTER_NUM;
        conf.sda_io_num = I2C0_MASTER_SDA_IO;        
        conf.scl_io_num = I2C0_MASTER_SCL_IO;
        conf.master.clk_speed = I2C0_MASTER_FREQ_HZ;
    }
    i2c_param_config(i2c_master_port, &conf);
    esp_err_t ret = i2c_driver_install(i2c_master_port, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, ESP_INTR_FLAG_SHARED);
    if(ret != ESP_OK)
    { 
        ESP_LOGE(TAG, "install i2c driver failed!");
    }
    else
    {
        i2c_hal_mutex = xSemaphoreCreateMutex();
        xSemaphoreGive(i2c_hal_mutex);
        if(i2c_hal_mutex == NULL)
        {
            ESP_LOGE(TAG, "step motor task init mutex failed!");
            return ESP_FAIL; 
        }
        else
        {
            i2c_initialized[i2c_num] = true;
        }
    }
    return ret;
}

bool is_i2c0_initialized(void)
{
    return i2c_initialized[I2C_NUM_0];
}

bool is_i2c1_initialized(void)
{
    return i2c_initialized[I2C_NUM_1];
}

esp_err_t i2c_master_read_reg(i2c_port_t i2c_num, uint8_t slaver_addr, uint8_t reg_addr, uint8_t *data)
{
    I2C_HAL_ENTER
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (slaver_addr << 1) | WRITE_BIT, ACK_CHECK_EN);
    i2c_master_write(cmd, &reg_addr, 1, ACK_CHECK_EN);

    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (slaver_addr << 1) | READ_BIT, ACK_CHECK_EN);
    i2c_master_read_byte(cmd, data, NACK_VAL);
    i2c_master_stop(cmd);

    esp_err_t ret = i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    I2C_HAL_EXIT
    return ret;
}

esp_err_t i2c_master_read_mem(i2c_port_t i2c_num, uint8_t slaver_addr, uint8_t reg_addr, uint8_t *buffer, uint16_t size)
{
    I2C_HAL_ENTER
    if (size == 0)
    {
        return ESP_OK;
    }
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (slaver_addr << 1) | WRITE_BIT, ACK_CHECK_EN);
    i2c_master_write(cmd, &reg_addr, 1, ACK_CHECK_EN);

    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (slaver_addr << 1) | READ_BIT, ACK_CHECK_EN);
    if(size > 1) 
    {
        i2c_master_read(cmd, buffer, size - 1, ACK_VAL);
    }
    i2c_master_read_byte(cmd, buffer + size - 1, NACK_VAL); //the lastest byte will not give a ASK
    i2c_master_stop(cmd);

    esp_err_t ret = i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    I2C_HAL_EXIT
    return ret;
}

esp_err_t i2c_master_write_reg(i2c_port_t i2c_num, uint8_t slaver_addr, uint8_t reg_addr, uint8_t data)
{
    I2C_HAL_ENTER
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (slaver_addr << 1) | WRITE_BIT, ACK_CHECK_EN);
    i2c_master_write(cmd, &reg_addr, 1, ACK_CHECK_EN);
    i2c_master_write(cmd, &data, 1, ACK_CHECK_EN);
    i2c_master_stop(cmd);

    esp_err_t ret = i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    I2C_HAL_EXIT
    return ret;
}

esp_err_t i2c_master_write_mem(i2c_port_t i2c_num, uint8_t slaver_addr, uint8_t reg_addr, uint8_t *buffer, uint16_t size)
{
    I2C_HAL_ENTER
    if (size == 0)
    {
        return ESP_OK;
    }
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (slaver_addr << 1) | WRITE_BIT, ACK_CHECK_EN);
    i2c_master_write(cmd, &reg_addr, 1, ACK_CHECK_EN);
    for(uint16_t i = 0; i < size; i++)
    {
        i2c_master_write(cmd, (buffer + i), 1, ACK_CHECK_EN);
    }
    i2c_master_stop(cmd);

    esp_err_t ret = i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    I2C_HAL_EXIT
    return ret;
}

