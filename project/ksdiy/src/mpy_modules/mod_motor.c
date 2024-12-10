#include <stdint.h>
#include <stdio.h>
#include "py/obj.h"
#include "py/runtime.h"
#include "py/mpstate.h"
#include "py/nlr.h"
#include "uart1.h"
#include "string.h"
#include "drv_motor.h"
#include "drv_hub.h"
STATIC mp_obj_t mpy_uart1_init()
{

    uart1_init();
    return mp_const_none;
}


STATIC mp_obj_t mpy_motor_cmd(mp_obj_t arr)
{
    int len;
    const char * arr_in = mp_obj_str_get_str(arr);
    char *cmd_data = (char *)malloc(strlen(arr_in)+2);

    strcpy(cmd_data,arr_in);
    strcat(cmd_data,"\n");

    len = strlen(cmd_data);

    uart1_send_data(cmd_data,len);

    free(cmd_data);

    return mp_const_none;
}


STATIC mp_obj_t mpy_car(mp_obj_t arr)
{

    int len;
    char *car_cmd = "G129 M1 ";
    const char * arr_in = mp_obj_str_get_str(arr);
    char *cmd_data = (char *)malloc(strlen(car_cmd)+strlen(arr_in)+2);

    strcpy(cmd_data,car_cmd);
    strcat(cmd_data,arr_in);
    strcat(cmd_data,"\n");

    len = strlen(cmd_data);

    uart1_send_data(cmd_data,len);

    free(cmd_data);
    
    return mp_const_none;
}


STATIC mp_obj_t mpy_stop()
{
    uart1_send_data("G129 M1 R0 L0\n",14);
    printf("motor is stop\n");
    return mp_const_none;
}


STATIC mp_obj_t mpy_right_motor(mp_obj_t arr)
{
    int len;
    char *right_cmd = "G17 M1 P";
    const char * arr_in = mp_obj_str_get_str(arr);
    char *cmd_data = (char *)malloc((strlen(right_cmd)+strlen(arr_in)+2));

    strcpy(cmd_data,right_cmd);
    strcat(cmd_data,arr_in);
    strcat(cmd_data,"\n");
    
    len = strlen(cmd_data);
    printf("len %d= ",len);
    uart1_send_data(cmd_data,len);

    free(cmd_data);

    return mp_const_none;
}


STATIC mp_obj_t mpy_left_motor(mp_obj_t arr)
{
    int len;
    char *left_cmd = "G18 M1 P";
    const char * arr_in = mp_obj_str_get_str(arr);
    char *cmd_data = (char *)malloc(strlen(left_cmd)+strlen(arr_in)+2);

    strcpy(cmd_data,left_cmd);
    strcat(cmd_data,arr_in);
    strcat(cmd_data,"\n");

    len = strlen(cmd_data);

    uart1_send_data(cmd_data,len);

    free(cmd_data);

    return mp_const_none;
}



STATIC mp_obj_t mpy_motion_motor(mp_obj_t arr)
{
    int len;
    char *left_cmd = "G129 M1 ";
    const char * arr_in = mp_obj_str_get_str(arr);
    char *cmd_data = (char *)malloc(strlen(left_cmd)+strlen(arr_in)+2);

    strcpy(cmd_data,left_cmd);
    strcat(cmd_data,arr_in);
    strcat(cmd_data,"\n");

    len = strlen(cmd_data);

    uart1_send_data(cmd_data,len);

    free(cmd_data);
    return mp_const_none;
}




STATIC mp_obj_t mpy_change_device_id(mp_obj_t arr)
{
    int len;
    char *left_cmd = "G255 M1 D";
    const char * arr_in = mp_obj_str_get_str(arr);
    char *cmd_data = (char *)malloc(strlen(left_cmd)+strlen(arr_in)+2);

    strcpy(cmd_data,left_cmd);
    strcat(cmd_data,arr_in);
    strcat(cmd_data,"\n");

    len = strlen(cmd_data);

    uart1_send_data(cmd_data,len);

    free(cmd_data);

    
    return mp_const_none;
}


STATIC mp_obj_t mpy_recevied_cmd()
{
    uint8_t *data;
    uart1_read_data(&data);
    
    return mp_const_none;
}


STATIC mp_obj_t mpy_forward(mp_obj_t arr)
{

    const char * arr_in = mp_obj_str_get_str(arr);
    forward(arr_in);
    return mp_const_none;
}


STATIC mp_obj_t mpy_backward(mp_obj_t arr)
{
    const char * arr_in = mp_obj_str_get_str(arr);
    backward(arr_in);

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

STATIC mp_obj_t mpy_detection_electric()
{

    uart1_send_data("G128 M3\n",8);
    return mp_const_none;
}

STATIC mp_obj_t mpy_read_ir_message()
{
    uart1_send_data("G128 M4\n",8);
    return mp_const_none;
}

STATIC mp_obj_t mpy_read_usb_state()
{

    uart1_send_data("G128 M5\n",8);
    return mp_const_none;
}

STATIC mp_obj_t mpy_read_ir_cmd()
{
    int ir_cmd_data = read_ir_cmd();
    return mp_obj_new_int(ir_cmd_data);
}





STATIC mp_obj_t mpy_turn_left(mp_obj_t arr)
{
    const char * arr_in = mp_obj_str_get_str(arr);
    turn_right(arr_in);
    return mp_const_none;
}


STATIC mp_obj_t mpy_turn_right(mp_obj_t arr)
{
    const char * arr_in = mp_obj_str_get_str(arr);
    turn_left(arr_in);
    return mp_const_none;
}



STATIC MP_DEFINE_CONST_FUN_OBJ_1(mpy_motor_cmd_obj, mpy_motor_cmd);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mpy_car_obj, mpy_car);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(mpy_stop_obj, mpy_stop);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mpy_right_motor_obj, mpy_right_motor);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mpy_left_motor_obj, mpy_left_motor);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mpy_motion_motor_obj, mpy_motion_motor);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mpy_change_device_id_obj, mpy_change_device_id);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mpy_forward_obj, mpy_forward);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mpy_backward_obj, mpy_backward);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(mpy_recevied_cmd_obj, mpy_recevied_cmd);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(mpy_detection_electric_obj, mpy_detection_electric);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(mpy_read_ir_message_obj, mpy_read_ir_message);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(mpy_read_usb_state_obj, mpy_read_usb_state);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(mpy_uart1_init_obj, mpy_uart1_init);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mpy_turn_left_obj, mpy_turn_left);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mpy_turn_right_obj, mpy_turn_right);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(mpy_read_ir_cmd_obj, mpy_read_ir_cmd);



STATIC const mp_map_elem_t my_motor_module_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR_car),                         (mp_obj_t)&mpy_car_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_right_motor),                 (mp_obj_t)&mpy_right_motor_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_left_motor),                  (mp_obj_t)&mpy_left_motor_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_motion),                      (mp_obj_t)&mpy_motion_motor_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_change_device_id),            (mp_obj_t)&mpy_change_device_id_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_recevied_cmd),                (mp_obj_t)&mpy_recevied_cmd_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_motor_cmd),                   (mp_obj_t)&mpy_motor_cmd_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_stop),                        (mp_obj_t)&mpy_stop_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_forward),                     (mp_obj_t)&mpy_forward_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_backward),                    (mp_obj_t)&mpy_backward_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_detection_electric),          (mp_obj_t)&mpy_detection_electric_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_read_ir_message),             (mp_obj_t)&mpy_read_ir_message_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_read_usb_state),              (mp_obj_t)&mpy_read_usb_state_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_uart1_init),                  (mp_obj_t)&mpy_uart1_init_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_turn_left),                  (mp_obj_t)&mpy_turn_left_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_turn_right),                  (mp_obj_t)&mpy_turn_right_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_read_ir_cmd),                  (mp_obj_t)&mpy_read_ir_cmd_obj },

};

STATIC MP_DEFINE_CONST_DICT(my_motor_module_globals, my_motor_module_globals_table);

const mp_obj_module_t my_motor_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&my_motor_module_globals,
};
