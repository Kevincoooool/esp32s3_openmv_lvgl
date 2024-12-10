/*
 * This file is part of the OpenMV project.
 *
 * Copyright (c) 2013-2019 Ibrahim Abdelkader <iabdalkader@openmv.io>
 * Copyright (c) 2013-2019 Kwabena W. Agyeman <kwagyeman@openmv.io>
 *
 * This work is licensed under the MIT license, see the file LICENSE for details.
 *
 * LCD Python module.
 */

#include <mp.h>
#include <objstr.h>
#include <spi.h>
#include <systick.h>
#include "imlib.h"
#include "fb_alloc.h"
#include "ff_wrapper.h"
#include "py_assert.h"
#include "py_helper.h"
#include "py_image.h"
#include "driver/gpio.h"


#define RST_PIN             GPIO_NUM_1
#define RST_PIN_WRITE(bit)  gpio_set_level(RST_PIN, bit);

#define RS_PIN              GPIO_NUM_45
#define RS_PIN_WRITE(bit)   gpio_set_level(RS_PIN, bit);

#define CS_PIN              GPIO_NUM_14
#define CS_PIN_WRITE(bit)   gpio_set_level(CS_PIN, bit);

#define LED_PIN             GPIO_NUM_48
#define LED_PIN_WRITE(bit)  gpio_set_level(LED_PIN, bit);

//extern mp_obj_t pyb_spi_send(mp_uint_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
//extern mp_obj_t pyb_spi_make_new(mp_obj_t type_in, mp_uint_t n_args, mp_uint_t n_kw, const mp_obj_t *args);
//extern mp_obj_t pyb_spi_deinit(mp_obj_t self_in);


extern void machine_hw_spi_transfer(mp_obj_base_t *self_in, size_t len, const uint8_t *src, uint8_t *dest);
extern mp_obj_t machine_hw_spi_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args);
extern void machine_hw_spi_deinit(mp_obj_base_t *self_in);




static mp_obj_t spi_port = NULL;
static int width = 0;
static int height = 0;
static enum { LCD_NONE=0, LCD_SHIELD,LCD_DISPLAY} type = LCD_NONE;
static bool backlight_init = false;

// Send out 8-bit data using the SPI object.
static void lcd_write_command_byte(uint8_t data_byte)
{
/*
    mp_map_t arg_map;
    arg_map.all_keys_are_qstrs = true;
    arg_map.is_fixed = true;
    arg_map.is_ordered = true;
    arg_map.used = 0;
    arg_map.alloc = 0;
    arg_map.table = NULL;
*/
    CS_PIN_WRITE(false);
    RS_PIN_WRITE(false); // command
    machine_hw_spi_transfer((mp_obj_base_t *)spi_port, 1, &data_byte, NULL);
	/*
    pyb_spi_send(
        2, (mp_obj_t []) {
            spi_port,
            mp_obj_new_int(data_byte)
        },
        &arg_map
    );
    */
    CS_PIN_WRITE(true);
}

// Send out 8-bit data using the SPI object.
static void lcd_write_data_byte(uint8_t data_byte)
{
/*
    mp_map_t arg_map;
    arg_map.all_keys_are_qstrs = true;
    arg_map.is_fixed = true;
    arg_map.is_ordered = true;
    arg_map.used = 0;
    arg_map.alloc = 0;
    arg_map.table = NULL;
*/
    CS_PIN_WRITE(false);
    RS_PIN_WRITE(true); // data
    machine_hw_spi_transfer((mp_obj_base_t *)spi_port, 1, &data_byte, NULL);
    /*
    pyb_spi_send(
        2, (mp_obj_t []) {
            spi_port,
            mp_obj_new_int(data_byte)
        },
        &arg_map
    );
    */
    CS_PIN_WRITE(true);
}

// Send out 8-bit data using the SPI object.
static void lcd_write_command(uint8_t data_byte, uint32_t len, uint8_t *dat)
{
    lcd_write_command_byte(data_byte);
    for (uint32_t i=0; i<len; i++) lcd_write_data_byte(dat[i]);
}

// write reg
static void lcd_write_regist(uint8_t data_byte, uint8_t data)
{
    lcd_write_command_byte(data_byte);
    lcd_write_data_byte(data);
}

// Send out 8-bit data using the SPI object.
static void lcd_write_data(uint32_t len, uint8_t *dat)
{
/*
    mp_obj_str_t arg_str;
    arg_str.base.type = &mp_type_bytes;
    arg_str.hash = 0;
    arg_str.len = len;
    arg_str.data = dat;

    mp_map_t arg_map;
    arg_map.all_keys_are_qstrs = true;
    arg_map.is_fixed = true;
    arg_map.is_ordered = true;
    arg_map.used = 0;
    arg_map.alloc = 0;
    arg_map.table = NULL;
*/
    CS_PIN_WRITE(false);
    RS_PIN_WRITE(true); // data
    machine_hw_spi_transfer((mp_obj_base_t *)spi_port, len, dat, NULL);
    /*
    pyb_spi_send(
        2, (mp_obj_t []) {
            spi_port,
            &arg_str
        },
        &arg_map
    );
    */
    CS_PIN_WRITE(true);
}

static mp_obj_t py_lcd_deinit()
{
    switch (type) {
        case LCD_NONE:
            return mp_const_none;
        case LCD_SHIELD:
		case LCD_DISPLAY:
            gpio_isr_handler_remove(RST_PIN)
            gpio_isr_handler_remove( RS_PIN);
            gpio_isr_handler_remove(CS_PIN);
            machine_hw_spi_deinit(spi_port);
            spi_port = NULL;
            width = 0;
            height = 0;
            type = LCD_NONE;
            if (backlight_init) {
                gpio_isr_handler_remove(LED_PIN);
                backlight_init = false;
            }
            return mp_const_none;
    }
    return mp_const_none;
}

static mp_obj_t py_lcd_write_regist(mp_obj_t addr, mp_obj_t val) {
    lcd_write_regist(mp_obj_get_int(addr), mp_obj_get_int(val));
    return mp_const_none;
}

static mp_obj_t py_lcd_init(uint n_args, const mp_obj_t *args, mp_map_t *kw_args)
{
    py_lcd_deinit();
	int lcd_type=py_helper_keyword_int(n_args, args, 0, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_type), LCD_SHIELD);
	int w = py_helper_keyword_int(n_args, args, 1, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_width), 128);
	if ((w <= 0) || (32768< w) || ((w%4)!=0)) nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "Invalid Width!"));		
	int h = py_helper_keyword_int(n_args, args, 2, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_height), 160);
	if ((h <= 0) || (32768 < h)|| ((h%4)!=0)) nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "Invalid height!"));	
	switch (lcd_type) {
        case LCD_NONE:
            return mp_const_none;
        case LCD_SHIELD:
		case LCD_DISPLAY:	
        {    
            spi_port = machine_hw_spi_make_new(NULL,
                2, // n_args
                3, // n_kw
                (mp_obj_t []) {
                    MP_OBJ_NEW_SMALL_INT(2), // SPI Port
                    MP_OBJ_NEW_SMALL_INT(SPI_MODE_MASTER),
                    MP_OBJ_NEW_QSTR(MP_QSTR_baudrate),
                    MP_OBJ_NEW_SMALL_INT(50000000), 
                    MP_OBJ_NEW_QSTR(MP_QSTR_polarity),
                    MP_OBJ_NEW_SMALL_INT(0),
                    MP_OBJ_NEW_QSTR(MP_QSTR_phase),
                    MP_OBJ_NEW_SMALL_INT(0)
                }
            );
				
			gpio_pad_select_gpio(CS_PIN);
			gpio_set_direction(CS_PIN,GPIO_MODE_OUTPUT);
			
			gpio_pad_select_gpio(RS_PIN);
			gpio_set_direction(CS_PIN,GPIO_MODE_OUTPUT);
			
			gpio_pad_select_gpio(RST_PIN);
			gpio_set_direction(CS_PIN,GPIO_MODE_OUTPUT);
			
			gpio_pad_select_gpio(LED_PIN);
			gpio_set_direction(CS_PIN,GPIO_MODE_OUTPUT);
			
			LED_PIN_WRITE(true);					
			width = w;
		    height = h;		
            type = lcd_type;
            backlight_init = false;

            RST_PIN_WRITE(false);
            systick_sleep(100);
            RST_PIN_WRITE(true);
            systick_sleep(100);
            lcd_write_command_byte(0x11); // Sleep Exit
            systick_sleep(120);

            // Memory Data Access Control
            uint8_t madctl = 0xC0;
            uint8_t bgr = py_helper_keyword_int(n_args, args, 3, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_bgr), 0);
            lcd_write_command(0x36, 1, (uint8_t []) {madctl | (bgr<<3)});
			lcd_write_command(0x3A, 1, (uint8_t []) {0x05});
#if 0
            // Interface Pixel Format
            
			/**********add st7735驱动***************/
			lcd_write_command(0xB1, 3, (uint8_t []) {0x05,0X3C,0X3C});
			lcd_write_command(0xB2, 3, (uint8_t []) {0x05,0X3C,0X3C});
			lcd_write_command(0xB3, 6, (uint8_t []) {0x05,0X3C,0X3C,0x05,0X3C,0X3C});
			lcd_write_command(0xB4, 1, (uint8_t []) {0x03});
			lcd_write_command(0xC0, 3, (uint8_t []) {0x28,0x08,0x04});
			lcd_write_command(0xC1, 1, (uint8_t []) {0xC0});
			lcd_write_command(0xC2, 2, (uint8_t []) {0x0D,0X00});
			lcd_write_command(0xC3, 2, (uint8_t []) {0x8D,0X2A});
			lcd_write_command(0xC4, 2, (uint8_t []) {0x8D,0XEE});
			lcd_write_command(0xC5, 1, (uint8_t []) {0x1A});
			lcd_write_command(0xE0, 16, (uint8_t []) {0x04,0x22,0x07,0x0A,0x2E,0x30,0x25,0x2a,0x28,0x26,0x2e,0x3a,0x00,0x01,0x03,0x13});
			lcd_write_command(0xE1, 16, (uint8_t []) {0x04,0x16,0x06,0x0d,0x2d,0x26,0x23,0x27,0x27,0x25,0x2d,0x3b,0x00,0x01,0x04,0x13});
#endif			// Display on
			if (type == LCD_DISPLAY){
				lcd_write_command_byte(0x21);  //invert color
			}
			lcd_write_command(0x2A, 4, (uint8_t []) {0x00,0x00,(((width-1)>>8)&0xFF),((width-1)&0xFF)});
			lcd_write_command(0x2B, 4, (uint8_t []) {0x00,0x00,(((height-1)>>8)&0xFF),((height-1)&0xFF)});
			lcd_write_command(0x36, 1, (uint8_t []) {0xc0});
			lcd_write_command_byte(0x29);

            return mp_const_none;
        }
		
		
    }
    return mp_const_none;
}

static mp_obj_t py_lcd_width()
{
    if (type == LCD_NONE) return mp_const_none;
    return mp_obj_new_int(width);
}

static mp_obj_t py_lcd_height()
{
    if (type == LCD_NONE) return mp_const_none;
    return mp_obj_new_int(height);
}

static mp_obj_t py_lcd_type()
{
    if (type == LCD_NONE) return mp_const_none;
    return mp_obj_new_int(type);
}

//add by zheng 20201223   支持设置LCD的扫描方向，横屏和竖屏
static mp_obj_t py_lcd_set_direction(mp_obj_t state_obj)
{
	int w=width;
	int h=height;
	
    switch (type) {
        case LCD_NONE:
            return mp_const_none;
        case LCD_SHIELD:
		case LCD_DISPLAY:
        {
            //bool bit = !!mp_obj_get_int(state_obj);
            int bit=mp_obj_get_int(state_obj);
            if(bit==0){
				width = w;
	            height = h;		
				lcd_write_command(0x2A, 4, (uint8_t []) {0x00,0x00,(((width-1)>>8)&0xFF),((width-1)&0xFF)});
				lcd_write_command(0x2B, 4, (uint8_t []) {0x00,0x00,(((height-1)>>8)&0xFF),((height-1)&0xFF)});
				lcd_write_command(0x36, 1, (uint8_t []) {0xc0});
			}
			if(bit==1){
				width = h;
	            height = w;
				lcd_write_command(0x2A, 4, (uint8_t []) {0x00,0x00,(((width-1)>>8)&0xFF),((width-1)&0xFF)});
				lcd_write_command(0x2B, 4, (uint8_t []) {0x00,0x00,(((height-1)>>8)&0xFF),((height-1)&0xFF)});
				lcd_write_command(0x36, 1, (uint8_t []) {0xa0});	
			}
			if(bit==2){
				width = w;
	            height = h;		
				lcd_write_command(0x2A, 4, (uint8_t []) {0x00,0x00,(((width-1)>>8)&0xFF),((width-1)&0xFF)});
				lcd_write_command(0x2B, 4, (uint8_t []) {0x00,0x00,(((height-1)>>8)&0xFF),((height-1)&0xFF)});
				lcd_write_command(0x36, 1, (uint8_t []) {0x00});	
			}
			if(bit==3){
				width = h;
				height = w;	
				lcd_write_command(0x2A, 4, (uint8_t []) {0x00,0x00,(((width-1)>>8)&0xFF),((width-1)&0xFF)});
				lcd_write_command(0x2B, 4, (uint8_t []) {0x00,0x00,(((height-1)>>8)&0xFF),((height-1)&0xFF)});
				lcd_write_command(0x36, 1, (uint8_t []) {0x60});	
			}	
			if(bit>3 ||(bit<0))
			{
				 PY_ASSERT_FALSE_MSG(bit,"Operation not supported!!");
			}		
            return mp_const_none;
        }
		
    }
    return mp_const_none;
}

static mp_obj_t py_lcd_set_backlight(mp_obj_t state_obj)
{
    switch (type) {
        case LCD_NONE:
            return mp_const_none;
        case LCD_SHIELD:
		case LCD_DISPLAY:	
        {
            bool bit = !!mp_obj_get_int(state_obj);
            if (!backlight_init) {
				gpio_pad_select_gpio(LED_PIN);
				gpio_set_direction(CS_PIN,GPIO_MODE_OUTPUT);
                LED_PIN_WRITE(bit); // Set first to prevent glitches.
                backlight_init = true;
            }
            LED_PIN_WRITE(bit);
            return mp_const_none;
        }
    }
    return mp_const_none;
}

static mp_obj_t py_lcd_get_backlight()
{
    switch (type) {
        case LCD_NONE:
            return mp_const_none;
        case LCD_SHIELD:
		case LCD_DISPLAY:
            if (!backlight_init) {
                return mp_const_none;
            }
            return mp_obj_new_int(HAL_GPIO_ReadPin(LED_PORT, LED_PIN));
    }
    return mp_const_none;
}

static mp_obj_t py_lcd_display(uint n_args, const mp_obj_t *args, mp_map_t *kw_args)
{
    image_t *arg_img = py_image_cobj(args[0]);
    PY_ASSERT_TRUE_MSG(IMAGE_IS_MUTABLE(arg_img), "Image format is not supported.");

    rectangle_t rect;
    py_helper_keyword_rectangle_roi(arg_img, n_args, args, 1, kw_args, &rect);

    // Fit X.
    int l_pad = 0, r_pad = 0;
    if (rect.w > width) {
        int adjust = rect.w - width;
        rect.w -= adjust;
        rect.x += adjust / 2;
    } else if (rect.w < width) {
        int adjust = width - rect.w;
        l_pad = adjust / 2;
        r_pad = (adjust + 1) / 2;
    }

    // Fit Y.
    int t_pad = 0, b_pad = 0;
    if (rect.h > height) {
        int adjust = rect.h - height;
        rect.h -= adjust;
        rect.y += adjust / 2;
    } else if (rect.h < height) {
        int adjust = height - rect.h;
        t_pad = adjust / 2;
        b_pad = (adjust + 1) / 2;
    }

    switch (type) {
        case LCD_NONE:
            return mp_const_none;
        case LCD_SHIELD:
		case LCD_DISPLAY:
            lcd_write_command_byte(0x2C);
            fb_alloc_mark();
            uint8_t *zero = fb_alloc0(width*2, FB_ALLOC_NO_HINT);
            uint16_t *line = fb_alloc(width*2, FB_ALLOC_NO_HINT);
            for (int i=0; i<t_pad; i++) {
                lcd_write_data(width*2, zero);
            }
            for (int i=0; i<rect.h; i++) {
                if (l_pad) {
                    lcd_write_data(l_pad*2, zero); // l_pad < width
                }
                if (IM_IS_GS(arg_img)) {
                    for (int j=0; j<rect.w; j++) {
                        uint8_t pixel = IM_GET_GS_PIXEL(arg_img, (rect.x + j), (rect.y + i));
                        line[j] = IM_RGB565(IM_R825(pixel),IM_G826(pixel),IM_B825(pixel));
                    }
                    lcd_write_data(rect.w*2, (uint8_t *) line);
                } else {
                    lcd_write_data(rect.w*2, (uint8_t *)
                        (((uint16_t *) arg_img->pixels) +
                        ((rect.y + i) * arg_img->w) + rect.x));
                }
                if (r_pad) {
                    lcd_write_data(r_pad*2, zero); // r_pad < width
                }
            }
            for (int i=0; i<b_pad; i++) {
                lcd_write_data(width*2, zero);
            }
            fb_alloc_free_till_mark();
            return mp_const_none;
    }
    return mp_const_none;
}

static mp_obj_t py_lcd_clear()
{
    switch (type) {
        case LCD_NONE:
            return mp_const_none;
        case LCD_SHIELD:
		case LCD_DISPLAY:
            lcd_write_command_byte(0x2C);
            fb_alloc_mark();
            uint8_t *zero = fb_alloc0(width*2, FB_ALLOC_NO_HINT);
            for (int i=0; i<height; i++) {
                lcd_write_data(width*2, zero);
            }
            fb_alloc_free_till_mark();
            return mp_const_none;
    }
    return mp_const_none;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_KW(py_lcd_init_obj, 0, py_lcd_init);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(py_lcd_deinit_obj, py_lcd_deinit);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(py_lcd_width_obj, py_lcd_width);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(py_lcd_height_obj, py_lcd_height);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(py_lcd_type_obj, py_lcd_type);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_lcd_set_backlight_obj, py_lcd_set_backlight);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(py_lcd_get_backlight_obj, py_lcd_get_backlight);
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(py_lcd_display_obj, 1, py_lcd_display);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(py_lcd_clear_obj, py_lcd_clear);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_lcd_set_direction_obj, py_lcd_set_direction); //add by zheng 20201223
STATIC MP_DEFINE_CONST_FUN_OBJ_2(py_lcd_write_regist_obj,         py_lcd_write_regist);

static const mp_map_elem_t globals_dict_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__),        MP_OBJ_NEW_QSTR(MP_QSTR_lcd) },
    { MP_ROM_QSTR(MP_QSTR_LCD_NONE),            MP_ROM_INT(LCD_NONE)  				},       
    { MP_ROM_QSTR(MP_QSTR_LCD_SHIELD),          MP_ROM_INT(LCD_SHIELD) 				},
    { MP_ROM_QSTR(MP_QSTR_LCD_DISPLAY),         MP_ROM_INT(LCD_DISPLAY) 			},
    { MP_OBJ_NEW_QSTR(MP_QSTR_init),            (mp_obj_t)&py_lcd_init_obj          },
    { MP_OBJ_NEW_QSTR(MP_QSTR_deinit),          (mp_obj_t)&py_lcd_deinit_obj        },
    { MP_OBJ_NEW_QSTR(MP_QSTR_width),           (mp_obj_t)&py_lcd_width_obj         },
    { MP_OBJ_NEW_QSTR(MP_QSTR_height),          (mp_obj_t)&py_lcd_height_obj        },
    { MP_OBJ_NEW_QSTR(MP_QSTR_type),            (mp_obj_t)&py_lcd_type_obj          },
    { MP_OBJ_NEW_QSTR(MP_QSTR_set_backlight),   (mp_obj_t)&py_lcd_set_backlight_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_get_backlight),   (mp_obj_t)&py_lcd_get_backlight_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_display),         (mp_obj_t)&py_lcd_display_obj       },
    { MP_OBJ_NEW_QSTR(MP_QSTR_clear),           (mp_obj_t)&py_lcd_clear_obj         },
    { MP_OBJ_NEW_QSTR(MP_QSTR_set_direction),   (mp_obj_t)&py_lcd_set_direction_obj },  //add by zheng 20201223
	{ MP_OBJ_NEW_QSTR(MP_QSTR_write_regist),    (mp_obj_t)&py_lcd_write_regist_obj  },  
	{ NULL, NULL },
};
STATIC MP_DEFINE_CONST_DICT(globals_dict, globals_dict_table);

const mp_obj_module_t lcd_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_t)&globals_dict,
};

void py_lcd_init0()
{
    py_lcd_deinit();
}


