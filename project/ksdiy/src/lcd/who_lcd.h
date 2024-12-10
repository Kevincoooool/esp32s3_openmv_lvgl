/*
 * @Author: Kevincoooool
 * @Date: 2022-03-02 19:29:05
 * @Description: 
 * @version:  
 * @Filename: Do not Edit
 * @LastEditTime: 2022-03-03 09:05:21
 * @FilePath: \esp-who\components\modules\lcd\who_lcd.h
 */
#pragma once

#include <stdint.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"
#include "esp_err.h"

#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_lcd_panel_ops.h"
#include "driver/gpio.h"

#define BOARD_LCD_MOSI 47
#define BOARD_LCD_MISO -1
#define BOARD_LCD_SCK 21
#define BOARD_LCD_CS 14
#define BOARD_LCD_DC 45
#define BOARD_LCD_RST 2
#define BOARD_LCD_BL 48
#define BOARD_LCD_PIXEL_CLOCK_HZ (40 * 1000 * 1000)
#define BOARD_LCD_BK_LIGHT_ON_LEVEL 0
#define BOARD_LCD_BK_LIGHT_OFF_LEVEL !BOARD_LCD_BK_LIGHT_ON_LEVEL
#define BOARD_LCD_H_RES 240
#define BOARD_LCD_V_RES 240
#define BOARD_LCD_CMD_BITS 8
#define BOARD_LCD_PARAM_BITS 8
#define LCD_HOST SPI2_HOST

#ifdef __cplusplus
extern "C"
{
#endif

typedef enum { 
	LCD_NONE=0, 
	LCD_SHIELD,
	LCD_DISPLAY
}LCD_TYPE;
esp_err_t register_lcd(uint8_t lcd_type,int width,int height,uint8_t rotate,uint8_t invert,uint8_t bgr);
esp_err_t deinit_lcd(void);
void screen_display(int x1, int y1, int x2, int y2, uint8_t *rgb565_data);
void app_lcd_draw_wallpaper();
void app_lcd_set_color(int color);
void screen_display_row(int x1, int y1, int x2, int y2, uint8_t *rgb565_data);
void get_lcd_reslution(uint16_t *p_width, uint16_t *p_height);


#ifdef __cplusplus
}
#endif

