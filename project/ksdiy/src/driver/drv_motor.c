#include <stdint.h>
#include <stdio.h>
#include "py/obj.h"
#include "py/runtime.h"
#include "py/mpstate.h"
#include "py/nlr.h"
#include "uart1.h"
#include "string.h"
#include <stdlib.h>
#include "drv_motor.h"

void any_cmd(char *arr)
{
    int len;
    char *cmd_data = (char *)malloc(sizeof(strlen(arr)+2));
    strcpy(cmd_data,arr);
    strcat(cmd_data,"\n");

    len = strlen(cmd_data);

    uart1_send_data(cmd_data,len);

    free(cmd_data);
}

void stop()
{
    uart1_send_data("G129 M1 R0 L0\n",14);
}

void left_motor(char *arr)
{
    int len;
    char *left_cmd = "G18 M1 P";
    char *cmd_data = (char *)malloc(sizeof(strlen(left_cmd)+strlen(arr)+2));

    strcpy(cmd_data,left_cmd);
    strcat(cmd_data,arr);
    strcat(cmd_data,"\n");

    len = strlen(cmd_data);

    uart1_send_data(cmd_data,len);

    free(cmd_data);
}

void right_motor(char *arr)
{
    int len;
    char *right_cmd = "G17 M1 P";
    char *cmd_data = (char *)malloc(sizeof(strlen(right_cmd)+strlen(arr)+2));

    strcpy(cmd_data,right_cmd);
    strcat(cmd_data,arr);
    strcat(cmd_data,"\n");
    
    len = strlen(cmd_data);

    uart1_send_data(cmd_data,len);

    free(cmd_data);

}

void car(char *arr)
{
    int len;
    char *car_cmd = "G129 M1 ";
    char *cmd_data = (char *)malloc(sizeof(strlen(arr)+2));

    strcpy(cmd_data,car_cmd);
    strcat(cmd_data,arr);
    strcat(cmd_data,"\n");

    len = strlen(cmd_data);

    uart1_send_data(cmd_data,len);

    free(cmd_data);
}

void change_device_id(char *arr)
{
    int len;
    char *left_cmd = "G255 M1 D";

    char *cmd_data = (char *)malloc(sizeof(strlen(left_cmd)+strlen(arr)+2));

    strcpy(cmd_data,left_cmd);
    strcat(cmd_data,arr);
    strcat(cmd_data,"\n");

    len = strlen(cmd_data);

    uart1_send_data(cmd_data,len);

    free(cmd_data);
}
void turn_right(char *arr)
{
    int len;
    char *car_cmd = "G129 M1 R";
    //const char * arr_in = mp_obj_str_get_str(arr);
    char *cmd_data = (char *)malloc(strlen(car_cmd)+(2*strlen(arr)+4));


    strcpy(cmd_data,car_cmd);
    strcat(cmd_data,arr);
    strcat(cmd_data," L");
    strcat(cmd_data,arr);
    strcat(cmd_data,"\n");

    len = strlen(cmd_data);
    //printf("len = %d\n ",len);
    //printf("cmd_data = %s\n ",cmd_data);
    uart1_send_data(cmd_data,len);
}
void turn_left(char *arr)
{
    int len;
    char *car_cmd = "G129 M1 R-";
    //const char * arr_in = mp_obj_str_get_str(arr);
    char *cmd_data = (char *)malloc(strlen(car_cmd)+(2*strlen(arr)+5));


    strcpy(cmd_data,car_cmd);
    strcat(cmd_data,arr);
    strcat(cmd_data," L-");
    strcat(cmd_data,arr);
    strcat(cmd_data,"\n");

    len = strlen(cmd_data);
    //printf("len = %d\n ",len);
    //printf("cmd_data = %s\n ",cmd_data);
    uart1_send_data(cmd_data,len);


    free(cmd_data);
}
void backward(char *arr)
{
    int len;
    char *car_cmd = "G129 M1 R";
    //const char * arr_in = mp_obj_str_get_str(arr);
    char *cmd_data = (char *)malloc(strlen(car_cmd)+(2*strlen(arr)+5));


    strcpy(cmd_data,car_cmd);
    strcat(cmd_data,arr);
    strcat(cmd_data," L-");
    strcat(cmd_data,arr);
    strcat(cmd_data,"\n");

    len = strlen(cmd_data);
    //printf("cmd_data = %s\n ",cmd_data);
    //printf("len = %d\n ",len);
    uart1_send_data(cmd_data,len);


    free(cmd_data);

}
void forward(char *arr)
{
    int len;
    char *car_cmd = "G129 M1 R-";
    //const char * arr_in = mp_obj_str_get_str(arr);
    char *cmd_data = (char *)malloc(strlen(car_cmd)+(2*strlen(arr)+5));


    strcpy(cmd_data,car_cmd);
    strcat(cmd_data,arr);
    strcat(cmd_data," L");
    strcat(cmd_data,arr);
    strcat(cmd_data,"\n");

    len = strlen(cmd_data);
    //printf("cmd_data = %s\n ",cmd_data);
    //printf("len = %d\n ",len);
    uart1_send_data(cmd_data,len);


    free(cmd_data);
}
void recevied_cmd()
{
    uint8_t *recevied_data;
    uart1_read_data(&recevied_data);
}


