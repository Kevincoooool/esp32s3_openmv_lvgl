#include <string.h>
#include "py/mphal.h"
#include "storage.h"
#include "sdram.h"
#include "eth.h"
#include "ulpi.h"

#if OPENAMP_PY
#include "modopenamp.h"
#endif

void PORTENTA_board_early_init(void) {
    HAL_InitTick(0);

    // Enable oscillator pin
    // This is enabled in the bootloader anyway.
    PORTENTA_board_osc_enable(true);

    // PMIC SW1 current limit fix
    __HAL_RCC_GPIOB_CLK_ENABLE();

    // Configure PMIC I2C GPIOs
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.Pin       = GPIO_PIN_6 | GPIO_PIN_7;
    GPIO_InitStructure.Pull      = GPIO_NOPULL;
    GPIO_InitStructure.Speed     = GPIO_SPEED_LOW;
    GPIO_InitStructure.Mode      = GPIO_MODE_AF_OD;
    GPIO_InitStructure.Alternate = GPIO_AF4_I2C1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);

    // Configure PMIC I2C
    I2C_HandleTypeDef i2c;
    i2c.Instance                = I2C1;
    i2c.Init.AddressingMode     = I2C_ADDRESSINGMODE_7BIT;
    i2c.Init.Timing             = 0x20D09DE7;
    i2c.Init.DualAddressMode    = I2C_DUALADDRESS_DISABLED;
    i2c.Init.GeneralCallMode    = I2C_GENERALCALL_DISABLED;
    i2c.Init.NoStretchMode      = I2C_NOSTRETCH_DISABLED;
    i2c.Init.OwnAddress1        = 0xFE;
    i2c.Init.OwnAddress2        = 0xFE;
    i2c.Init.OwnAddress2Masks   = 0;

    __HAL_RCC_I2C1_CLK_ENABLE();
    __HAL_RCC_I2C1_FORCE_RESET();
    __HAL_RCC_I2C1_RELEASE_RESET();
    HAL_I2C_Init(&i2c);

    // Set SW1 current limit to 1.5A
    // Fixes power glitches with Eth and SDRAM.
    uint8_t buf[] = {0x36, 0x02};
    HAL_I2C_Master_Transmit(&i2c, 0x08<<1, buf, 2, 1000);

    HAL_I2C_DeInit(&i2c);
    __HAL_RCC_I2C1_FORCE_RESET();
    __HAL_RCC_I2C1_RELEASE_RESET();
    __HAL_RCC_I2C1_CLK_DISABLE();

    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_6);
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_7);
    __HAL_RCC_GPIOB_CLK_DISABLE();

    // Reset ETH Phy
    __HAL_RCC_GPIOJ_CLK_ENABLE();
    GPIO_InitTypeDef  gpio_eth_rst_init_structure;
    gpio_eth_rst_init_structure.Pin = GPIO_PIN_15;
    gpio_eth_rst_init_structure.Mode = GPIO_MODE_OUTPUT_PP;
    gpio_eth_rst_init_structure.Pull = GPIO_PULLUP;
    gpio_eth_rst_init_structure.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOJ, &gpio_eth_rst_init_structure);
    HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_15, 0);
    HAL_Delay(100);
    HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_15, 1);

    // Put Eth in low-power mode
    eth_init(&eth_instance, MP_HAL_MAC_ETH0);
    eth_low_power_mode(&eth_instance, true);

    // Make sure UPLI is Not in low-power mode.
    ulpi_leave_low_power();

    #if OPENAMP_PY
    OpenAMP_MPU_Config();
    SystemCoreClockUpdate();
    #endif
}

void PORTENTA_reboot_to_bootloader(void) {
    RTC_HandleTypeDef RTCHandle;
    RTCHandle.Instance = RTC;
    HAL_RTCEx_BKUPWrite(&RTCHandle, RTC_BKP_DR0, 0xDF59);
    NVIC_SystemReset();
}

void PORTENTA_board_osc_enable(int enable) {
    __HAL_RCC_GPIOH_CLK_ENABLE();
    GPIO_InitTypeDef  gpio_osc_init_structure;
    gpio_osc_init_structure.Pin = GPIO_PIN_1;
    gpio_osc_init_structure.Mode = GPIO_MODE_OUTPUT_PP;
    gpio_osc_init_structure.Pull = GPIO_PULLUP;
    gpio_osc_init_structure.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOH, &gpio_osc_init_structure);
    HAL_GPIO_WritePin(GPIOH, GPIO_PIN_1, enable);
}

void PORTENTA_board_low_power(int mode)
{
    switch (mode) {
        case 0:     // Leave stop mode.
            ulpi_leave_low_power();
            eth_low_power_mode(NULL, false);
            sdram_leave_low_power();
            break;
        case 1:     // Enter stop mode.
            ulpi_enter_low_power();
            eth_low_power_mode(NULL, true);
            sdram_enter_low_power();
            break;
        case 2:     // Enter standby mode.
            ulpi_enter_low_power();
            eth_low_power_mode(NULL, true);
            sdram_powerdown();
            break;
    }

    // Enable QSPI deepsleep for modes 1 and 2
    mp_spiflash_deepsleep(&spi_bdev2.spiflash, (mode != 0));

    #if defined(M4_APP_ADDR)
    // Signal Cortex-M4 to go to Standby mode.
    if (mode == 2) {
        __HAL_RCC_HSEM_CLK_ENABLE();
        HAL_HSEM_FastTake(0);
        HAL_HSEM_Release(0, 0);
        __HAL_RCC_HSEM_CLK_DISABLE();
        HAL_Delay(100);
    }
    #endif
}
