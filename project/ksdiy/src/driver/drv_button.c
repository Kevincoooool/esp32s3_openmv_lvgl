#include "drv_aw9523b.h"
#include "drv_button.h"
#define BOTTON_WINDOW_DATAS_NUM   10
#define BOTTON_PRESSED_COUNT      4

bool is_home_button_pressed(void)
{
    static uint8_t botton_home_press_count = 0;
    static bool botton_home_press_flag = 0;
    if(ext_read_digital(HOME_PIN) == 0)
    {
        botton_home_press_count ++;
        if(botton_home_press_count > BOTTON_PRESSED_COUNT && botton_home_press_flag == 0)
        {
            botton_home_press_flag = 1;
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        botton_home_press_count = 0;
        if(botton_home_press_flag)
        {
            botton_home_press_flag = 0;
        }
        return false;
    }
}

bool is_program_1_button_pressed(void)
{
    static uint8_t botton_program_num1_press_count = 0;
    static bool botton_program_num1_press_flag = 0;
    if(ext_read_digital(PROGRAM_1_PIN) == 0)
    {
        botton_program_num1_press_count ++;
        if(botton_program_num1_press_count > BOTTON_PRESSED_COUNT && botton_program_num1_press_flag == 0)
        {
            botton_program_num1_press_flag = 1;
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        botton_program_num1_press_count = 0;
        if(botton_program_num1_press_flag)
        {
            botton_program_num1_press_flag = 0;
        }
        return false;
    }
}

bool is_program_2_button_pressed(void)
{
    static uint8_t botton_program_num2_press_count = 0;
    static bool botton_program_num2_press_flag = 0;
    if(ext_read_digital(PROGRAM_2_PIN) == 0)
    {
        botton_program_num2_press_count ++;
        if(botton_program_num2_press_count > BOTTON_PRESSED_COUNT && botton_program_num2_press_flag == 0)
        {
            botton_program_num2_press_flag = 1;
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        botton_program_num2_press_count = 0;
        if(botton_program_num2_press_flag)
        {
            botton_program_num2_press_flag = 0;
        }
        return false;
    }
}
