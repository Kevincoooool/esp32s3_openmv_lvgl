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
#include "py/obj.h"
#include "py/nlr.h"
#include "py/runtime.h"

#include "py_helper.h"
#include "who_lcd.h"
extern void get_lcd_reslution(uint16_t *p_width, uint16_t *p_height);
extern void lcd_driver_flush(int x1, int y1, int x2, int y2, uint8_t *rgb565_data);
extern void enable_lcd_backlight(bool backlight);
uint16_t *colors = NULL;
static bool bl_intensity;
static uint16_t lcd_width = 240, lcd_height = 240, lcd_row_cnt = 0;
static uint16_t cb_lcd_width = 0, cb_lcd_height = 0;
static uint8_t type = LCD_NONE;
static bool lcd_need_reinit=false;

#define debug(fmt, ...) \
  if (0)                \
  printf(fmt, ##__VA_ARGS__)
const int pixel_size = sizeof(uint16_t);
static uint8_t first_in = 1;

static mp_obj_t IRAM_ATTR py_lcd_display(uint n_args, const mp_obj_t *args, mp_map_t *kw_args)
{

	image_t *src_img = py_helper_arg_to_image_mutable_bayer(args[0]);
	
    rectangle_t rect;
    py_helper_keyword_rectangle_roi(src_img, n_args, args, 1, kw_args, &rect);
	int x0, x1, y0, y1,len;
	image_t dst_img;
  	dst_img.w = lcd_width;
  	dst_img.h = lcd_height;
  	dst_img.bpp = IMAGE_BPP_RGB565;

	switch(src_img->bpp){
		case IMAGE_BPP_RGB565:
		{
			 if((src_img->w<=lcd_width) && (src_img->h<=lcd_height))
			 {
			 	len=src_img->w*src_img->h*2;
			   	dst_img.w = src_img->w;
	  			dst_img.h = src_img->h;
	  			dst_img.bpp = IMAGE_BPP_RGB565;
			 	fb_alloc_mark();
		   		dst_img.data = fb_alloc0(len, FB_ALLOC_NO_HINT);
				for(int i=0;i<len;i+=2)
				{
					dst_img.data[i]=src_img->data[i+1];
					dst_img.data[i+1]=src_img->data[i];
				}	
				x0=(lcd_width-dst_img.w)/2;
				y0=(lcd_height-dst_img.h)/2;  
				screen_display(x0, y0, dst_img.w, dst_img.h,dst_img.data);
				fb_free();
		   		fb_alloc_free_till_mark();
			 }
			 else 
			 { 	
			   	dst_img.w = lcd_width;
	  			dst_img.h = lcd_height;
	  			dst_img.bpp = IMAGE_BPP_RGB565;
				len=lcd_width*lcd_height*2;
			 	fb_alloc_mark();
		   		dst_img.data = fb_alloc0(len, FB_ALLOC_NO_HINT);
				x0=(src_img->w-lcd_width);
				y0=(src_img->h-lcd_height)/2; 
				x1=x0+lcd_width*2;
				y1=y0+lcd_height;
				uint8_t* pdst=dst_img.data;
				for(int i=y0;i<y1;i++)
				{
					uint8_t* psrc=src_img->data+i*src_img->w*2;
					for(int j=x0;j<x1;j+=2)
					{
						*pdst=*(psrc+j+1);
						*(pdst+1)=*(psrc+j);
						pdst+=2;
					}
				}	
	 
				screen_display(0, 0, dst_img.w, dst_img.h,dst_img.data);
				fb_free();
		   		fb_alloc_free_till_mark();
			 	
			 }
			break;
		}	
		case IMAGE_BPP_GRAYSCALE:
		{
			 if((src_img->w<=lcd_width) && (src_img->h<=lcd_height))
			 {
			 	len=src_img->w*src_img->h*2;
			   	dst_img.w = src_img->w;
	  			dst_img.h = src_img->h;
	  			dst_img.bpp = IMAGE_BPP_RGB565;
			 	fb_alloc_mark();
		   		dst_img.data = fb_alloc0(len, FB_ALLOC_NO_HINT);
				
				uint8_t* pdst=dst_img.data;
				uint8_t* psrc=src_img->data;
				for(int i=0;i<len;i+=2)
				{
					*(pdst+i)=((*psrc)&0xf8)|((*psrc)>>5);
					*(pdst+i+1)=(((*psrc)<<3)&0xe0)|((*psrc)>>3);
					psrc++;
				}	
				x0=(lcd_width-dst_img.w)/2;
				y0=(lcd_height-dst_img.h)/2;  
				screen_display(x0, y0, dst_img.w, dst_img.h,dst_img.data);
				fb_free();
		   		fb_alloc_free_till_mark();
			 }
			 else 
			 { 	
			   	dst_img.w = lcd_width;
	  			dst_img.h = lcd_height;
	  			dst_img.bpp = IMAGE_BPP_RGB565;
				len=lcd_width*lcd_height*2;
			 	fb_alloc_mark();
		   		dst_img.data = fb_alloc0(len, FB_ALLOC_NO_HINT);
				x0=(src_img->w-lcd_width)/2;
				y0=(src_img->h-lcd_height)/2; 
				x1=x0+lcd_width;
				y1=y0+lcd_height;
				uint8_t* pdst=dst_img.data;
				for(int i=y0;i<y1;i++)
				{
					uint8_t* psrc=src_img->data+i*src_img->w;
					for(int j=x0;j<x1;j++)
					{
						*pdst=((*(psrc+j))&0xf8)|((*(psrc+j))>>5);
						*(pdst+1)=(((*(psrc+j))<<3)&0xe0)|((*(psrc+j))>>3);
						pdst+=2;
					}
				}	
				screen_display(0, 0, dst_img.w, dst_img.h,dst_img.data);
				fb_free();
		   		fb_alloc_free_till_mark();
			 	
			 }
			break;
		}
		default:
			break;
		 
	}
    return mp_const_none;
}


STATIC MP_DEFINE_CONST_FUN_OBJ_KW(py_lcd_display_obj, 1, py_lcd_display);

/*Todo: added clear lvgl draw_buf memory here*/
static mp_obj_t py_lcd_init(uint n_args, const mp_obj_t *args, mp_map_t *kw_args)
{
	//deinit_lcd();
	int type=py_helper_keyword_int(n_args, args, 0, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_type), LCD_SHIELD);
	int w = py_helper_keyword_int(n_args, args, 1, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_width), 240);
	if ((w <= 0) || (32768< w) || ((w%4)!=0)) nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "Invalid Width!"));		
	int h = py_helper_keyword_int(n_args, args, 2, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_height), 240);
	if ((h <= 0) || (32768 < h)|| ((h%4)!=0)) nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "Invalid height!"));	
	int rotate=py_helper_keyword_int(n_args, args, 0, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_rotate), 0);
	if ((rotate>=4 )||(rotate<0)) nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "Invalid rotate!"));	
	int bgr=py_helper_keyword_int(n_args, args, 0, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_bgr), 0);
	if (rotate<0) nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "Invalid rotate!"));
	uint8_t invert=(type==LCD_DISPLAY)?1:0;
	cb_lcd_width = w;
	cb_lcd_height = h;
	if (!lcd_need_reinit)
	{
		lcd_need_reinit=true;
		bl_intensity = true;
		lcd_width=w;
		lcd_height=h;	
		enable_lcd_backlight(bl_intensity);
		register_lcd(type,lcd_width,lcd_height,rotate,invert,bgr);
	}
	enable_lcd_backlight(1);
	app_lcd_set_color(0);
	return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(py_lcd_init_obj, 0, py_lcd_init);


static mp_obj_t py_lcd_deinit()
{
    switch (type) {
        case LCD_NONE:
            return mp_const_none;
        case LCD_SHIELD:
		case LCD_DISPLAY:
			deinit_lcd();
            lcd_width = 0;
            lcd_height = 0;
            return mp_const_none;
    }
    return mp_const_none;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_0(py_lcd_deinit_obj, py_lcd_deinit);

static mp_obj_t py_lcd_clear()
{
    app_lcd_set_color(0);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(py_lcd_clear_obj, py_lcd_clear);
/*
STATIC mp_obj_t py_lcd_clear(uint n_args, const mp_obj_t *args)
{
  bl_intensity = false;
  enable_lcd_backlight(bl_intensity);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(py_lcd_clear_obj, 0, 1, py_lcd_clear);
*/
STATIC mp_obj_t py_lcd_width()
{
  if (!lcd_width)
    get_lcd_reslution(&lcd_width, &lcd_height);
  return mp_obj_new_int(lcd_width);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(py_lcd_width_obj, py_lcd_width);

STATIC mp_obj_t py_lcd_height()
{
  if (!lcd_height)
    get_lcd_reslution(&lcd_width, &lcd_height);
  return mp_obj_new_int(lcd_height);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(py_lcd_height_obj, py_lcd_height);

STATIC mp_obj_t py_lcd_set_backlight(mp_obj_t intensity_obj)
{
  int intensity = mp_obj_get_int(intensity_obj);
  if ((intensity < 0) || (255 < intensity))
    mp_raise_msg(&mp_type_ValueError, MP_ERROR_TEXT("0 <= intensity <= 255!"));
  bl_intensity = !!intensity;
  enable_lcd_backlight(bl_intensity);
  return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_lcd_set_backlight_obj, py_lcd_set_backlight);

STATIC mp_obj_t py_lcd_get_backlight()
{
  return mp_obj_new_int(bl_intensity);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(py_lcd_get_backlight_obj, py_lcd_get_backlight);

STATIC const mp_rom_map_elem_t globals_dict_table[] = {
    {MP_ROM_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_lcd)},
    {MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&py_lcd_init_obj)},
    {MP_ROM_QSTR(MP_QSTR_width), MP_ROM_PTR(&py_lcd_width_obj)},
    {MP_ROM_QSTR(MP_QSTR_height), MP_ROM_PTR(&py_lcd_height_obj)},
    {MP_ROM_QSTR(MP_QSTR_get_backlight), MP_ROM_PTR(&py_lcd_get_backlight_obj)},
    {MP_ROM_QSTR(MP_QSTR_set_backlight), MP_ROM_PTR(&py_lcd_set_backlight_obj)},
    {MP_ROM_QSTR(MP_QSTR_display), MP_ROM_PTR(&py_lcd_display_obj)},
    {MP_ROM_QSTR(MP_QSTR_clear), MP_ROM_PTR(&py_lcd_clear_obj)}};
STATIC MP_DEFINE_CONST_DICT(globals_dict, globals_dict_table);

const mp_obj_module_t lcd_module = {
    .base = {&mp_type_module},
    .globals = (mp_obj_t)&globals_dict,
};
