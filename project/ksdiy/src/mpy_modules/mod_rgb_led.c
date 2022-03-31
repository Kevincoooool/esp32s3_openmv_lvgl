#include <stdint.h>
#include <stdio.h>
#include "py/obj.h"
#include "py/runtime.h"
#include "py/mpstate.h"
#include "py/nlr.h"
#include "uart1.h"
#include "string.h"


STATIC mp_obj_t mpy_single_rgb(mp_obj_t arr)
{
    int len;
    char *rgb_cmd = "G128 M2 I";
    const char * arr_in = mp_obj_str_get_str(arr);
    char *cmd_data = (char *)malloc(strlen(rgb_cmd)+strlen(arr_in)+2);

    strcpy(cmd_data,rgb_cmd);
    strcat(cmd_data,arr_in);
    strcat(cmd_data,"\n");

    len = strlen(cmd_data);

    uart1_send_data(cmd_data,len);

    free(cmd_data);

    return mp_const_none;
}

STATIC mp_obj_t mpy_all_rgb(mp_obj_t arr)
{
    int len;
    char *all_rgb_cmd = "G128 M1 ";
    const char * arr_in = mp_obj_str_get_str(arr);
    char *cmd_data = (char *)malloc(strlen(all_rgb_cmd)+strlen(arr_in)+2);

    strcpy(cmd_data,all_rgb_cmd);
    strcat(cmd_data,arr_in);
    strcat(cmd_data,"\n");

    len = strlen(cmd_data);

    uart1_send_data(cmd_data,len);

    free(cmd_data);

    return mp_const_none;
}


STATIC MP_DEFINE_CONST_FUN_OBJ_1(mpy_single_rgb_obj, mpy_single_rgb);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mpy_all_rgb_obj, mpy_all_rgb);



STATIC const mp_map_elem_t rgb_led_module_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR_car),                         (mp_obj_t)&mpy_single_rgb_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_right_motor),                 (mp_obj_t)&mpy_all_rgb_obj },


};

STATIC MP_DEFINE_CONST_DICT(rgb_led_module_globals, rgb_led_module_globals_table);

const mp_obj_module_t rgb_led_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&rgb_led_module_globals,
};