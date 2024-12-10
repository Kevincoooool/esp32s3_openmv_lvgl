
#include <stdio.h>
#include <math.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_err.h"
#include "esp_log.h"
#include "drv_i2c.h"
#include "drv_aw9523b.h"

static const char *TAG = "DRV AW9523B";
static const char led_addr_table[] = 
{
    0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b,
    0x20, 0x21, 0x22, 0x23, 0x2c, 0x2d, 0x2e, 0x2f,
};
static bool aw9523b_initialized = false;

#define I2C_MASTER_NUM I2C_NUM_1

//#define BOARD_MATATALAB
#define BOARD_MATATALAB_SP4

esp_err_t aw9523b_init(void)
{
    esp_err_t ret = ESP_OK;
    if(aw9523b_initialized == true)
    {
        ESP_LOGD(TAG, "i2c driver has been Initialized");
        return ret;
    }
    if(!is_i2c1_initialized())
    {
        i2c_master_init(I2C_MASTER_NUM);
    }
    ESP_LOGD(TAG, "set_max_current");
    set_max_current(IMAX1_4);
    ESP_LOGD(TAG, "ext_pin_mode");

#if defined BOARD_MATATALAB    
    ext_pin_mode(CAMERA_RESET_PIN, OUTPUT);
    ext_pin_mode(CAMERA_PWDN_PIN, OUTPUT);
    ext_pin_mode(VIBRATION_MOTOR_PIN, OUTPUT);
    ext_pin_mode(LCD_LEDK_PIN, OUTPUT);
    ext_pin_mode(LCD_TP_RESET_PIN, OUTPUT);
    ext_pin_mode(PA_CTRL_PIN, OUTPUT);
    ext_pin_mode(PERI_PWR_ON_PIN, OUTPUT);
    ext_pin_mode(LIGHT_SW_PIN, OUTPUT);
    set_p0_mode(PUSH_PULL);


    ext_pin_mode(HOME_PIN , INPUT);
    ext_pin_mode(BACK_PIN, INPUT);
#elif defined BOARD_MATATALAB_SP4
    ext_pin_mode(CAMERA_RESET_PIN, OUTPUT);
    ext_pin_mode(CAMERA_PWDN_PIN, OUTPUT);
    ext_pin_mode(LCD_TP_RESET_PIN, OUTPUT);
    ext_pin_mode(LCD_LEDK_PIN, OUTPUT);
    ext_pin_mode(LCD_CS_PIN, OUTPUT);
    ext_pin_mode(PA_CTRL_PIN, OUTPUT);
    ext_pin_mode(IR_EN_PIN, OUTPUT);
    ext_pin_mode(LANG_CS_PIN, OUTPUT);

    set_p0_mode(PUSH_PULL);

    ext_pin_mode(USB_IN_PIN, INPUT);
    ext_pin_mode(SD_P_EN_PIN, INPUT);
    
    ext_pin_mode(HOME_PIN , INPUT);

    ext_pin_mode(LED_R_PIN , LED_MODE);
    ext_pin_mode(LED_G_PIN , LED_MODE);
    ext_pin_mode(LED_B_PIN , LED_MODE);
#endif     
    aw9523b_initialized = true;
    return ret;
}

bool is_aw9523b_initialized(void)
{
    return aw9523b_initialized;
}

esp_err_t ext_pin_mode(ext_pin_num pin, ext_io_mode mode)
{
    esp_err_t ret;
    uint8_t port_config_read;
    uint8_t led_config_read;
    uint8_t port_config;
    uint8_t led_config;
    if(!is_i2c1_initialized())
    {
        i2c_master_init(I2C_MASTER_NUM);
    }
    if (pin < 0x10)
    {
        ret = i2c_master_read_reg(I2C_MASTER_NUM, AW9523B_I2C_ADDRESS,  AW9523B_P0_LED_MODE, &led_config_read);
        if(ret != ESP_OK)
        {
            ESP_LOGE(TAG, "read AW9523B_P0_LED_MODE reg error!(%x)", ret);
        }
        led_config = led_config_read;
        if(mode == LED_MODE)
        {
            led_config &= ~(0x01 << pin);
        }
        else
        {
            led_config |= (0x01 << pin);
        }
        ret = i2c_master_write_reg(I2C_MASTER_NUM, AW9523B_I2C_ADDRESS, AW9523B_P0_LED_MODE, led_config);
        if(ret != ESP_OK)
        {
            ESP_LOGE(TAG, "write AW9523B_P0_CONF_STATE reg error!(%x)", ret);
        }
        if(mode == LED_MODE)
        {
            return ESP_OK;
        }
        ret = i2c_master_read_reg(I2C_MASTER_NUM, AW9523B_I2C_ADDRESS,  AW9523B_P0_CONF_STATE, &port_config_read);
        if(ret != ESP_OK)
        {
            ESP_LOGE(TAG, "ext_pin_mode read reg error!(%x)", ret);
        }
        port_config = port_config_read;
        if(mode == OUTPUT)
        {
           port_config &= ~(0x01 << pin);
        }
        else
        {
           port_config |= (0x01 << pin);
        }
        ret = i2c_master_write_reg(I2C_MASTER_NUM, AW9523B_I2C_ADDRESS, AW9523B_P0_CONF_STATE, port_config);
        if(ret != ESP_OK)
        {
            ESP_LOGE(TAG, "ext_pin_mode write reg error!(%x)", ret);
        }
    }
    else
    {
        ret = i2c_master_read_reg(I2C_MASTER_NUM, AW9523B_I2C_ADDRESS,  AW9523B_P1_LED_MODE, &led_config_read);
        if(ret != ESP_OK)
        {
            ESP_LOGE(TAG, "read AW9523B_P1_LED_MODE reg error!(%x)", ret);
        }
        led_config = led_config_read;
        if(mode == LED_MODE)
        {
            led_config &= ~(0x01 << (pin-0x10));
        }
        else
        {
            led_config |= (0x01 << (pin-0x10));
        }
        ret = i2c_master_write_reg(I2C_MASTER_NUM, AW9523B_I2C_ADDRESS, AW9523B_P1_LED_MODE, led_config);
        if(ret != ESP_OK)
        {
            ESP_LOGE(TAG, "write AW9523B_P1_LED_MODE reg error!(%x)", ret);
        }
        if(mode == LED_MODE)
        {
            return ESP_OK;
        }
        ret = i2c_master_read_reg(I2C_MASTER_NUM, AW9523B_I2C_ADDRESS,  AW9523B_P1_CONF_STATE, &port_config_read);
        if(ret != ESP_OK)
        {
            ESP_LOGE(TAG, "ext_pin_mode read reg error!(%x)", ret);
        }
        port_config = port_config_read;
        if(mode == OUTPUT)
        {
           port_config &= ~(0x01 << (pin-0x10));
        }
        else
        {
           port_config |= (0x01 << (pin-0x10));
        }
        ret = i2c_master_write_reg(I2C_MASTER_NUM, AW9523B_I2C_ADDRESS, AW9523B_P1_CONF_STATE, port_config);
        if(ret != ESP_OK)
        {
            ESP_LOGE(TAG, "ext_pin_mode write reg error!(%x)", ret);
        }
    }
    return ret;
}

esp_err_t set_p0_mode(p0_port_mode mode)
{
    esp_err_t ret;
    uint8_t global_control_read;
    uint8_t global_control;
    if(!is_i2c1_initialized())
    {
        i2c_master_init(I2C_MASTER_NUM);
    }
    ret = i2c_master_read_reg(I2C_MASTER_NUM, AW9523B_I2C_ADDRESS,  AW9523B_REG_GLOB_CTR, &global_control_read);
    if(ret != ESP_OK)
    {
        ESP_LOGE(TAG, "set_p0_mode read reg error!");
    }
    global_control = global_control_read;
    if(mode == OPEN_DRAIN)
    {
        global_control &= ~(0x01 << 4);
    }
    else
    {
        global_control |= (0x01 << 4);
    }
    ret = i2c_master_write_reg(I2C_MASTER_NUM, AW9523B_I2C_ADDRESS, AW9523B_REG_GLOB_CTR, global_control);
    if(ret != ESP_OK)
    {
        ESP_LOGE(TAG, "set_p0_mode write reg error!");
    }
    return ret;
}

esp_err_t set_max_current(max_current_type current)
{
    esp_err_t ret;
    uint8_t global_control_read;
    uint8_t global_control;
    if(!is_i2c1_initialized())
    {
        i2c_master_init(I2C_MASTER_NUM);
    }
    ret = i2c_master_read_reg(I2C_MASTER_NUM, AW9523B_I2C_ADDRESS,  AW9523B_REG_GLOB_CTR, &global_control_read);
    if(ret != ESP_OK)
    {
        ESP_LOGE(TAG, "set_max_current read reg error!(%x)", ret);
    }
    global_control = global_control_read;
    global_control = (global_control & 0xFC) | (current & 0x03);
    ret = i2c_master_write_reg(I2C_MASTER_NUM, AW9523B_I2C_ADDRESS, AW9523B_REG_GLOB_CTR, global_control);
    if(ret != ESP_OK)
    {
        ESP_LOGE(TAG, "set_max_current write reg error!(%x)", ret);
    }
    return ret;
}

esp_err_t set_led_dimming(ext_pin_num pin, uint8_t step)
{
    esp_err_t ret;
    uint8_t led_addrs;
    if(!is_aw9523b_initialized())
    {
        aw9523b_init();
    }
    if (pin < 0x10)
    {
        led_addrs = led_addr_table[pin];
    }
    else
    {
        led_addrs = led_addr_table[8 + pin - 0x10];
    }
    ret = i2c_master_write_reg(I2C_MASTER_NUM, AW9523B_I2C_ADDRESS, led_addrs, step);
    if(ret != ESP_OK)
    {
        ESP_LOGE(TAG, "set_led_dimming write reg error!");
    }
    return ret;
}

esp_err_t ext_write_digital(ext_pin_num pin, uint8_t val)
{
    esp_err_t ret;
    uint8_t port_out_state_read;
    uint8_t port_out_state;
    if(!is_aw9523b_initialized())
    {
        aw9523b_init();
    }
    if (pin < 0x10)
    {
        ret = i2c_master_read_reg(I2C_MASTER_NUM, AW9523B_I2C_ADDRESS,  AW9523B_P0_OUT_STATE, &port_out_state_read);
        if(ret != ESP_OK)
        {
            ESP_LOGE(TAG, "ext_write_digital read reg error!");
        }
        port_out_state = port_out_state_read;
        if (val == LOW)
        {
           port_out_state &= ~(0x01 << pin);
        }
        else
        {
           port_out_state |= (0x01 << pin);
        }
        ret = i2c_master_write_reg(I2C_MASTER_NUM, AW9523B_I2C_ADDRESS, AW9523B_P0_OUT_STATE, port_out_state);
        if(ret != ESP_OK)
        {
            ESP_LOGE(TAG, "ext_write_digital write reg error!");
        }
    }
    else
    {
        ret = i2c_master_read_reg(I2C_MASTER_NUM, AW9523B_I2C_ADDRESS,  AW9523B_P1_OUT_STATE, &port_out_state_read);
        if(ret != ESP_OK)
        {
            ESP_LOGE(TAG, "ext_write_digital read reg error!");
        }
        port_out_state = port_out_state_read;
        if (val == LOW)
        {
           port_out_state &= ~(0x01 << (pin-0x10));
        }
        else
        {
           port_out_state |= (0x01 << (pin-0x10));
        }
        ret = i2c_master_write_reg(I2C_MASTER_NUM, AW9523B_I2C_ADDRESS, AW9523B_P1_OUT_STATE, port_out_state);
        if(ret != ESP_OK)
        {
            ESP_LOGE(TAG, "ext_write_digital write reg error!");
        }
    }
    return ret; 
}

uint8_t ext_read_digital(ext_pin_num pin)
{
    esp_err_t ret;
    uint8_t port_in_state_read;
    uint8_t value;
    if(!is_aw9523b_initialized())
    {
        aw9523b_init();
    }
    if (pin < 0x10)
    {
        ret = i2c_master_read_reg(I2C_MASTER_NUM, AW9523B_I2C_ADDRESS,  AW9523B_P0_IN_STATE, &port_in_state_read);
        if(ret != ESP_OK)
        {
            ESP_LOGE(TAG, "ext_read_digital read reg error!");
        }
        value = (port_in_state_read) & (1 << pin);
    }
    else
    {
        ret = i2c_master_read_reg(I2C_MASTER_NUM, AW9523B_I2C_ADDRESS,  AW9523B_P1_IN_STATE, &port_in_state_read);
        if(ret != ESP_OK)
        {
            ESP_LOGE(TAG, "ext_read_digital read reg error!");
        }
        value = (port_in_state_read) & (1 << (pin-0x10));
    }
    return value;
}  
