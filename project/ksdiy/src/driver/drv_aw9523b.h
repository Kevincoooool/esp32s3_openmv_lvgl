#ifndef __DRV_AW9523B_H__
#define __DRV_AW9523B_H__

#include "esp_err.h"
#include "driver/gpio.h"

/** Registers */
#define AW9523B_I2C_ADDRESS   0x5B  ///< I2C base address for AW9523B
#define AW9523B_REG_ID        0x10  ///< id register
#define AW9523B_ID            0x23  ///< id value
#define AW9523B_P0_IN_STATE   0x00  ///< P0 port input state
#define AW9523B_P1_IN_STATE   0x01  ///< P1 port input state
#define AW9523B_P0_OUT_STATE  0x02  ///< P0 port output state
#define AW9523B_P1_OUT_STATE  0x03  ///< P1 port output state
#define AW9523B_P0_CONF_STATE 0x04  ///< P0 port config state
#define AW9523B_P1_CONF_STATE 0x05  ///< P1 port config state
#define AW9523B_REG_GLOB_CTR  0x11  ///< Global control register
#define AW9523B_P0_LED_MODE   0x12  ///< P0 port led mode switch register
#define AW9523B_P1_LED_MODE   0x13  ///< P1 port led mode switch register
#define AW9523B_REG_SOFT_RST  0x7F  ///< Soft reset register

#define HIGH              1
#define LOW               0

typedef enum
{
    OUTPUT = 0x00,
    INPUT = 0x01,
    LED_MODE = 0x02
} ext_io_mode;

typedef enum
{
    P00  = 0x00,
    P01  = 0x01,
    P02  = 0x02,
    P03  = 0x03,
    P04  = 0x04,
    P05  = 0x05,
    P06  = 0x06,
    P07  = 0x07,

    P10  = 0x10,
    P11  = 0x11,
    P12  = 0x12,
    P13  = 0x13,
    P14  = 0x14,
    P15  = 0x15,
    P16  = 0x16,
    P17  = 0x17,
} ext_pin_num;


//#define BOARD_MATATALAB
#define BOARD_MATATALAB_SP5

#if defined BOARD_MATATALAB
#define CAMERA_RESET_PIN          P10
#define CAMERA_PWDN_PIN           P11
#define VIBRATION_MOTOR_PIN       P12
#define LCD_LEDK_PIN              P13
#define LCD_TP_RESET_PIN          P14
#define PA_CTRL_PIN               P11
#define PERI_PWR_ON_PIN           P16
#define LIGHT_SW_PIN              P17


#define HOME_PIN                  P00
#define BACK_PIN                  P01

#elif defined BOARD_MATATALAB_SP4
#define CAMERA_RESET_PIN          P13
#define CAMERA_PWDN_PIN           P14
#define LCD_LEDK_PIN              P12
#define LCD_TP_RESET_PIN          P11
#define LCD_CS_PIN                P10
#define PA_CTRL_PIN               P11
#define IR_EN_PIN                 P07
#define SD_P_EN_PIN               P04
#define USB_IN_PIN                P03

#define HOME_PIN                  P00
#define PROGRAM_1_PIN             P01
#define PROGRAM_2_PIN             P02
//#define VIBRATION_MOTOR_PIN       P12
//#define LCD_TP_RESET_PIN          P14
//#define PERI_PWR_ON_PIN           P16
//#define LIGHT_SW_PIN              P17


//#define BACK_PIN                  P01

#elif defined BOARD_MATATALAB_SP5
#define CAMERA_RESET_PIN          P13
#define CAMERA_PWDN_PIN           P12
#define LCD_LEDK_PIN              P06
#define LCD_TP_RESET_PIN          P11
#define LCD_CS_PIN                P10
#define PA_CTRL_PIN               P05
#define IR_EN_PIN                 P07
#define USB_IN_PIN                P03
#define SD_P_EN_PIN               P04

#define HOME_PIN                  P00
#define PROGRAM_1_PIN             P01
#define PROGRAM_2_PIN             P02
#define LED_R_PIN                 P14
#define LED_G_PIN                 P15
#define LED_B_PIN                 P16
#define LANG_CS_PIN               P17

#endif

typedef enum
{
    EXT_PORT0 = 0x00, 
    EXT_PORT1 = 0x01
}ext_port;

typedef enum
{
    OPEN_DRAIN = 0x00, // Port 0 open drain mode
    PUSH_PULL = 0x01   // Port 0 push pull mode
}p0_port_mode;

typedef enum
{
    IMAX = 0x00,    // IMAX
    IMAX3_4 = 0x01, // 3/4 IMAX
    IMAX2_4 = 0x02, // 2/4 IMAX
    IMAX1_4 = 0x03, // 1/4 IMAX
}max_current_type;

extern esp_err_t aw9523b_init(void);
extern bool is_aw9523b_initialized(void);
extern esp_err_t ext_pin_mode(ext_pin_num pin, ext_io_mode mode);
extern esp_err_t set_p0_mode(p0_port_mode mode);
extern esp_err_t set_max_current(max_current_type current);
extern esp_err_t set_led_dimming(ext_pin_num pin, uint8_t step);
extern esp_err_t ext_write_digital(ext_pin_num pin, uint8_t val);
extern uint8_t ext_read_digital(ext_pin_num pin);

#endif /* __DRV_AW9523B_H__ */ 
