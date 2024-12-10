
#include <stdio.h>
#include "drv_rgb_led.h"
#include <stdio.h>
#include <math.h>
#include "esp_err.h"
#include "esp_log.h"
#include "drv_aw9523b.h"

void set_rgb_led_pwm(uint8_t r, uint8_t g, uint8_t b)
{
    set_led_dimming(LED_R_PIN, r);
    set_led_dimming(LED_G_PIN, g);
    set_led_dimming(LED_B_PIN, b);
    return;

}



