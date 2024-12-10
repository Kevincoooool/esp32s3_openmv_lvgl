#ifndef __HOME_KEY_H__
#define __HOME_KEY_H__

/*
extern void key_value_update(uint32_t key_value);
extern void key_press_status_update(void);
extern uint32_t is_key_pressed(void);
extern uint8_t single_key_pressed(key_type_list key_num);
extern button_key_status_type get_key_status(void);
extern button_key_status_type get_single_key_status(key_type_list key_num);
extern void button_key_test(void);
*/

void home_key_press_status_update(void);
uint8_t home_key_pressed();

#endif /* __BUTTON_KEY_H__ */ 
