#include "who_lcd.h"
#include <string.h>
#include "logo_en_240x240_lcd.h"
#include "logo_ksdiy.h"
#include <stdio.h>

#include "driver/spi_master.h"
#include "driver/i2c.h"

static const char *TAG = "LCD";

static uint16_t lcd_width = 0;
static uint16_t lcd_height = 0;

esp_lcd_panel_handle_t panel_handle = NULL;

static bool gReturnFB = true;

void enable_lcd_backlight(bool backlight)
{
    if (BOARD_LCD_BL != -1)
    {
        gpio_pad_select_gpio(BOARD_LCD_BL);
        gpio_set_direction(BOARD_LCD_BL, GPIO_MODE_OUTPUT);
        gpio_set_level(BOARD_LCD_BL, 1);
    }
    printf("%s backlight .\n", backlight ? "Enabling" : "Disabling");
}

void screen_display(int x1, int y1, int x2, int y2, uint8_t *rgb565_data)
{
    // g_lcd.draw_bitmap(x1, y1, x2, y2, (uint16_t *)rgb565_data);
    esp_lcd_panel_draw_bitmap(panel_handle, x1, y1, x2, y2, (uint16_t *)rgb565_data);
}
void screen_display_row(int x1, int y1, int x2, int y2, uint8_t *rgb565_data)
{
    // g_lcd.draw_row_bitmap(x1, y1, x2, y2, (uint16_t *)rgb565_data);
    esp_lcd_panel_draw_bitmap(panel_handle, x1, y1, x2, y2, (uint16_t *)rgb565_data);
}
esp_err_t register_lcd(uint8_t lcd_type, int width, int height, uint8_t rotate, uint8_t invert, uint8_t bgr)
{
    lcd_width = width;
    lcd_height = height;

    if (BOARD_LCD_BL != -1)
    {
        ESP_LOGI(TAG, "Turn off LCD backlight");
        gpio_config_t bk_gpio_config = {
            .mode = GPIO_MODE_OUTPUT,
            .pin_bit_mask = 1ULL << BOARD_LCD_BL};
        ESP_ERROR_CHECK(gpio_config(&bk_gpio_config));
    }
    ESP_LOGI(TAG, "Initialize SPI bus");
    spi_bus_config_t buscfg = {
        .sclk_io_num = BOARD_LCD_SCK,
        .mosi_io_num = BOARD_LCD_MOSI,
        .miso_io_num = BOARD_LCD_MISO,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = width * 80 * sizeof(uint16_t),
    };
    ESP_ERROR_CHECK(spi_bus_initialize(LCD_HOST, &buscfg, SPI_DMA_CH_AUTO));

    ESP_LOGI(TAG, "Install panel IO");
    esp_lcd_panel_io_handle_t io_handle = NULL;
    esp_lcd_panel_io_spi_config_t io_config = {
        .dc_gpio_num = BOARD_LCD_DC,
        .cs_gpio_num = BOARD_LCD_CS,
        .pclk_hz = 80 * 1000 * 1000,
        .lcd_cmd_bits = 8,
        .lcd_param_bits = 8,
        .spi_mode = 0,
        .trans_queue_depth = 10,
        // .on_color_trans_done = example_notify_lvgl_flush_ready,
        // .user_ctx = &disp_drv,
    };
    // Attach the LCD to the SPI bus
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)LCD_HOST, &io_config, &io_handle));

    esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = BOARD_LCD_RST,
        // .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_RGB,
        .color_space = ESP_LCD_COLOR_SPACE_RGB,
        .bits_per_pixel = 16,
    };

    ESP_LOGI(TAG, "Install ST7789 panel driver");
    ESP_ERROR_CHECK(esp_lcd_new_panel_st7789(io_handle, &panel_config, &panel_handle));

    ESP_ERROR_CHECK(esp_lcd_panel_reset(panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_init(panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_swap_xy(panel_handle, false));
    ESP_ERROR_CHECK(esp_lcd_panel_mirror(panel_handle, false, false));
    ESP_ERROR_CHECK(esp_lcd_panel_invert_color(panel_handle, true));
    ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(panel_handle, true));
    app_lcd_draw_wallpaper();
    return ESP_OK;
}

esp_err_t deinit_lcd(void)
{
    esp_err_t ret = ESP_OK;
 
    return ret;
}

void app_lcd_draw_wallpaper()
{

    uint16_t *pixels = (uint16_t *)heap_caps_malloc((240 * 240) * sizeof(uint16_t), MALLOC_CAP_8BIT | MALLOC_CAP_SPIRAM);
    if (NULL == pixels)
    {
        ESP_LOGE(TAG, "Memory for bitmap is not enough");
        return;
    }
    memcpy(pixels, logo_en_240x240_lcd, (240 * 240) * sizeof(uint16_t));

    esp_lcd_panel_draw_bitmap(panel_handle, 0, 0, 240, 240, (uint16_t *)pixels);

    heap_caps_free(pixels);
}

void app_lcd_set_color(int color)
{

    uint16_t *buffer = (uint16_t *)malloc(lcd_width * sizeof(uint16_t));
    if (NULL == buffer)
    {
        ESP_LOGE(TAG, "Memory for bitmap is not enough");
    }
    else
    {
        for (size_t i = 0; i < lcd_width; i++)
        {
            buffer[i] = color;
        }

        for (int y = 0; y < lcd_height; y++)
        {
            // esp_lcd_panel_draw_bitmap(panel_handle, 0, y, 240, y, buffer);
        }
        free(buffer);
    }
}
void get_lcd_reslution(uint16_t *p_width, uint16_t *p_height)
{
    // scr_info_t lcd_info;
    // g_lcd.get_info(&lcd_info);

    *p_width = lcd_width;
    *p_height = lcd_height;
}
