#include <stdint.h>
#include <stdio.h>
#include "py/obj.h"
#include "py/runtime.h"
#include "py/mpstate.h"
#include "py/nlr.h"
#include "uart1.h"
#include "string.h"

STATIC mp_obj_t mpy_detection_electric()
{

    uart1_send_data("G128 M3\n",8);
    return mp_const_none;
}

STATIC mp_obj_t mpy_read_ir_message()
{

    uart1_send_data("G128 M4\n",8);
    read_hub_data();
    return mp_const_none;
}

STATIC mp_obj_t mpy_read_usb_state()
{

    uart1_send_data("G128 M5\n",8);
    return mp_const_none;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_0(mpy_detection_electric_obj, mpy_detection_electric);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(mpy_read_ir_message_obj, mpy_read_ir_message);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(mpy_read_usb_state_obj, mpy_read_usb_state);


STATIC const mp_map_elem_t hub_module_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR_detection_electric),              (mp_obj_t)&mpy_detection_electric_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_read_ir_message),                 (mp_obj_t)&mpy_read_ir_message_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_read_usb_state),                  (mp_obj_t)&mpy_read_usb_state_obj },

};

STATIC MP_DEFINE_CONST_DICT(hub_module_globals, hub_module_globals_table);

const mp_obj_module_t hub_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&hub_module_globals,
};