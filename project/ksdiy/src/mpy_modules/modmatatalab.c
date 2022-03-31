#include <stdint.h>
#include <stdio.h>
#include "py/obj.h"
#include "py/runtime.h"
#include "py/mpstate.h"
#include "py/nlr.h"
#include "usb_cdc.h"


#define MODULE_ENABLE_VALUE   (1)
#define MODULE_DISABLE_VALUE  (0)

STATIC mp_obj_t mpy_is_dbg_mode_enable(void)
{
    return mp_obj_new_bool(is_dbg_mode_enabled());
}

STATIC MP_DEFINE_CONST_FUN_OBJ_0(is_dbg_mode_enable_obj, mpy_is_dbg_mode_enable);

STATIC mp_obj_t mpy_cdc_task_debug_mode(void)
{
    cdc_task_debug_mode();
    return mp_const_none;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_0(cdc_task_debug_mode_obj, mpy_cdc_task_debug_mode);

STATIC mp_obj_t mpy_cdc_task(void)
{
    cdc_task_serial_mode();
    return mp_const_none;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_0(cdc_task_obj, mpy_cdc_task);

STATIC const mp_map_elem_t matatalab_module_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR_is_dbg_mode),           (mp_obj_t)&is_dbg_mode_enable_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_cdc_debug_task),        (mp_obj_t)&cdc_task_debug_mode_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_cdc_task),              (mp_obj_t)&cdc_task_obj },
};

STATIC MP_DEFINE_CONST_DICT(matatalab_module_globals, matatalab_module_globals_table);

const mp_obj_module_t matatalab_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&matatalab_module_globals,
};


