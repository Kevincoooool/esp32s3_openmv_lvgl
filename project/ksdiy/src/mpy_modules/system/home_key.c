#include <stdio.h>
#include "home_key.h"
#include "esp_log.h"
#include "drv_aw9523b.h"

static const char *TAG = "HOME_KEY";

static uint8_t button_key_value = 0;

void home_key_press_status_update(void)
{
    button_key_value = !ext_read_digital(HOME_PIN);
    return;
}

uint8_t home_key_pressed()
{
    return button_key_value;
}
