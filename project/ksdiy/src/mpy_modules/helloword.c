#include <stdint.h>
#include <stdio.h>
#include "py/obj.h"
#include "py/runtime.h"
#include "py/mpstate.h"
#include "py/nlr.h"
#include "usb_cdc.h"





STATIC mp_obj_t mpy_print_hello_word(void)
{
    printf("helloword\n");
    printf("addhelloword\n");
    return mp_const_none;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_0(print_hello_word_obj, mpy_print_hello_word);



STATIC const mp_map_elem_t helloword_module_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR_print_hello_word),           (mp_obj_t)&print_hello_word_obj },

};

STATIC MP_DEFINE_CONST_DICT(helloword_module_globals, helloword_module_globals_table);

const mp_obj_module_t helloword_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&helloword_module_globals,
};


