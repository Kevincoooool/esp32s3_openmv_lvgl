#define MICROPY_HW_BOARD_NAME       "OPENMV4P"
#define MICROPY_HW_MCU_NAME         "STM32H743"
#define MICROPY_PY_SYS_PLATFORM     "OpenMV4P-H7"
#define MICROPY_PY_SYS_STDIO_BUFFER (0)
#define MICROPY_STREAMS_POSIX_API   (1)

#define MICROPY_OBJ_REPR            (MICROPY_OBJ_REPR_C)
#define UINT_FMT                    "%u"
#define INT_FMT                     "%d"
typedef int mp_int_t;               // must be pointer size
typedef unsigned int mp_uint_t;     // must be pointer size

#define MICROPY_FATFS_EXFAT         (1)
#define MICROPY_HW_ENABLE_RNG       (1)
#define MICROPY_HW_ENABLE_RTC       (1)
#define MICROPY_HW_ENABLE_TIMER     (1)
#define MICROPY_HW_ENABLE_SERVO     (1)
#define MICROPY_HW_ENABLE_DAC       (1)
#define MICROPY_HW_ENABLE_ADC       (1)
#define MICROPY_HW_ENABLE_SPI2      (1)
#define MICROPY_HW_ENABLE_USB       (1)
#define MICROPY_HW_HAS_FLASH        (1)
#define MICROPY_HW_ENABLE_SDCARD    (1)
// Reserved DMA streams
#define MICROPY_HW_DMA1S0_IS_RESERVED
#define MICROPY_HW_DMA2S1_IS_RESERVED
#define MICROPY_HW_TIM_IS_RESERVED(id) (id == 1 || id == 6)

// Note these are not used in top system.c.
#define MICROPY_HW_CLK_PLLM         (3)
#define MICROPY_HW_CLK_PLLN         (200)
#define MICROPY_HW_CLK_PLLP         (2)
#define MICROPY_HW_CLK_PLLQ         (8)
#define MICROPY_HW_CLK_PLLR         (2)

void board_low_power(int mode);
#define MICROPY_BOARD_LEAVE_STOP    board_low_power(0);
#define MICROPY_BOARD_ENTER_STOP    board_low_power(1);
#define MICROPY_BOARD_ENTER_STANDBY board_low_power(2);

#define MICROPY_HW_USB_VID                      0x1209
#define MICROPY_HW_USB_PID                      0xabd1
#define MICROPY_HW_USB_PID_CDC_MSC              (MICROPY_HW_USB_PID)
#define MICROPY_HW_USB_PID_CDC_HID              (MICROPY_HW_USB_PID)
#define MICROPY_HW_USB_PID_CDC                  (MICROPY_HW_USB_PID)
#define MICROPY_HW_USB_PID_MSC                  (MICROPY_HW_USB_PID)
#define MICROPY_HW_USB_PID_CDC2_MSC             (MICROPY_HW_USB_PID)
#define MICROPY_HW_USB_PID_CDC2                 (MICROPY_HW_USB_PID)
#define MICROPY_HW_USB_PID_CDC3                 (MICROPY_HW_USB_PID)
#define MICROPY_HW_USB_PID_CDC3_MSC             (MICROPY_HW_USB_PID)
#define MICROPY_HW_USB_PID_CDC_MSC_HID          (MICROPY_HW_USB_PID)
#define MICROPY_HW_USB_PID_CDC2_MSC_HID         (MICROPY_HW_USB_PID)
#define MICROPY_HW_USB_PID_CDC3_MSC_HID         (MICROPY_HW_USB_PID)
#define MICROPY_HW_USB_LANGID_STRING            0x409
#define MICROPY_HW_USB_MANUFACTURER_STRING      "OpenMV"
#define MICROPY_HW_USB_PRODUCT_FS_STRING        "OpenMV Virtual Comm Port in FS Mode"
#define MICROPY_HW_USB_PRODUCT_HS_STRING        "OpenMV Virtual Comm Port in HS Mode"
#define MICROPY_HW_USB_SERIALNUMBER_FS_STRING   "000000000011"
#define MICROPY_HW_USB_SERIALNUMBER_HS_STRING   "000000000010"
#define MICROPY_HW_USB_INTERFACE_FS_STRING      "VCP Interface"
#define MICROPY_HW_USB_INTERFACE_HS_STRING      "VCP Interface"
#define MICROPY_HW_USB_CONFIGURATION_FS_STRING  "VCP Config"
#define MICROPY_HW_USB_CONFIGURATION_HS_STRING  "VCP Config"

// UART1 config
#define MICROPY_HW_UART1_TX  (pin_B14)
#define MICROPY_HW_UART1_RX  (pin_B15)

// UART3 config
#define MICROPY_HW_UART3_TX  (pin_B10)
#define MICROPY_HW_UART3_RX  (pin_B11)
#define MICROPY_HW_UART3_RTS (pin_B14)
#define MICROPY_HW_UART3_CTS (pin_B13)

// I2C buses
#define MICROPY_HW_I2C2_SCL (pin_B10)
#define MICROPY_HW_I2C2_SDA (pin_B11)

#define MICROPY_HW_I2C4_SCL (pin_D12)
#define MICROPY_HW_I2C4_SDA (pin_D13)

// SPI buses
#define MICROPY_HW_SPI2_NSS  (pin_B12)
#define MICROPY_HW_SPI2_SCK  (pin_B13)
#define MICROPY_HW_SPI2_MISO (pin_B14)
#define MICROPY_HW_SPI2_MOSI (pin_B15)

// FDCAN bus
#define MICROPY_HW_CAN2_NAME  "FDCAN2"
#define MICROPY_HW_CAN2_TX    (pin_B13)
#define MICROPY_HW_CAN2_RX    (pin_B12)

// SD card detect switch
#define MICROPY_HW_SDCARD_DETECT_PIN        (pin_G7)
#define MICROPY_HW_SDCARD_DETECT_PULL       (GPIO_PULLUP)
#define MICROPY_HW_SDCARD_DETECT_PRESENT    (GPIO_PIN_RESET)

// USB config
#define MICROPY_HW_USB_FS                   (1)
#define MICROPY_HW_USB_CDC_RX_DATA_SIZE     (512)
#define MICROPY_HW_USB_CDC_TX_DATA_SIZE     (512)
#define MICROPY_HW_USB_VBUS_DETECT_PIN      (pin_A9)

// LEDs
#define MICROPY_HW_LED1             (pin_C0) // red
#define MICROPY_HW_LED2             (pin_C1) // green
#define MICROPY_HW_LED3             (pin_C2) // blue
#define MICROPY_HW_LED4             (pin_E2) // IR
#define MICROPY_HW_LED_OTYPE        (GPIO_MODE_OUTPUT_PP)
// NOTE: LEDs are active low.
#define MICROPY_HW_LED_ON(pin)      (pin->gpio->BSRR = (pin->pin_mask << 16))
#define MICROPY_HW_LED_OFF(pin)     (pin->gpio->BSRR = pin->pin_mask)

// Servos
#define PYB_SERVO_NUM (2)

// Board network interfaces config.
#if MICROPY_PY_WINC1500
extern const struct _mod_network_nic_type_t mod_network_nic_type_winc;
#define MICROPY_PY_USOCKET_EXTENDED_STATE   (1)
#define MICROPY_BOARD_NETWORK_INTERFACES \
    { MP_ROM_QSTR(MP_QSTR_WINC), MP_ROM_PTR(&mod_network_nic_type_winc) },\
    { MP_ROM_QSTR(MP_QSTR_WLAN), MP_ROM_PTR(&mod_network_nic_type_winc) },
#else
#define MICROPY_BOARD_NETWORK_INTERFACES
#endif

// Use external SPI flash for storage
#define MICROPY_HW_ENABLE_INTERNAL_FLASH_STORAGE (0)
#define MICROPY_HW_SPIFLASH_ENABLE_CACHE (1)

// QSPI Flash 256MBits
#define MICROPY_HW_QSPI_PRESCALER           (2) // F_CLK = F_AHB/2 (100MHz)
#define MICROPY_HW_SPIFLASH_SIZE_BITS       (256 * 1024 * 1024)
#define MICROPY_HW_QSPIFLASH_SIZE_BITS_LOG2 (28)
#define MICROPY_HW_QSPIFLASH_CS             (pyb_pin_QSPIFLASH_CS)
#define MICROPY_HW_QSPIFLASH_SCK            (pyb_pin_QSPIFLASH_SCK)
#define MICROPY_HW_QSPIFLASH_IO0            (pyb_pin_QSPIFLASH_IO0)
#define MICROPY_HW_QSPIFLASH_IO1            (pyb_pin_QSPIFLASH_IO1)
#define MICROPY_HW_QSPIFLASH_IO2            (pyb_pin_QSPIFLASH_IO2)
#define MICROPY_HW_QSPIFLASH_IO3            (pyb_pin_QSPIFLASH_IO3)

// block device config for SPI flash
extern const struct _mp_spiflash_config_t spiflash_config;
extern struct _spi_bdev_t spi_bdev;
#define MICROPY_HW_BDEV_IOCTL(op, arg) ( \
    (op) == BDEV_IOCTL_NUM_BLOCKS ? (MICROPY_HW_SPIFLASH_SIZE_BITS / 8 / FLASH_BLOCK_SIZE) : \
    (op) == BDEV_IOCTL_INIT ? spi_bdev_ioctl(&spi_bdev, (op), (uint32_t)&spiflash_config) : \
    spi_bdev_ioctl(&spi_bdev, (op), (arg)) \
)
#define MICROPY_HW_BDEV_READBLOCKS(dest, bl, n) spi_bdev_readblocks(&spi_bdev, (dest), (bl), (n))
#define MICROPY_HW_BDEV_WRITEBLOCKS(src, bl, n) spi_bdev_writeblocks(&spi_bdev, (src), (bl), (n))
#define MICROPY_HW_BDEV_SPIFLASH_EXTENDED (&spi_bdev)

// SDRAM
#define MICROPY_HW_SDRAM_SIZE               (32 * 1024 * 1024)
#define MICROPY_HW_SDRAM_STARTUP_TEST       (1)
#define MICROPY_HW_SDRAM_TEST_FAIL_ON_ERROR (true)

// Timing configuration for 240MHz/3=80MHz (12.5ns)
//#define MICROPY_HW_SDRAM_CLOCK_PERIOD       3
//#define MICROPY_HW_SDRAM_CAS_LATENCY        2
//#define MICROPY_HW_SDRAM_FREQUENCY          (80000) // 80 MHz
//#define MICROPY_HW_SDRAM_TIMING_TMRD        (2)
//#define MICROPY_HW_SDRAM_TIMING_TXSR        (6)
//#define MICROPY_HW_SDRAM_TIMING_TRAS        (4)
//#define MICROPY_HW_SDRAM_TIMING_TRC         (5)
//#define MICROPY_HW_SDRAM_TIMING_TWR         (2)
//#define MICROPY_HW_SDRAM_TIMING_TRP         (2)
//#define MICROPY_HW_SDRAM_TIMING_TRCD        (2)

// Timing configuration for 200MHz/2=100MHz (10ns)
#define MICROPY_HW_SDRAM_CLOCK_PERIOD       2
#define MICROPY_HW_SDRAM_CAS_LATENCY        2
#define MICROPY_HW_SDRAM_FREQUENCY          (100000) // 100 MHz
#define MICROPY_HW_SDRAM_TIMING_TMRD        (2)
#define MICROPY_HW_SDRAM_TIMING_TXSR        (7)
#define MICROPY_HW_SDRAM_TIMING_TRAS        (5)
#define MICROPY_HW_SDRAM_TIMING_TRC         (6)
#define MICROPY_HW_SDRAM_TIMING_TWR         (3)
#define MICROPY_HW_SDRAM_TIMING_TRP         (2)
#define MICROPY_HW_SDRAM_TIMING_TRCD        (2)

// 16-bit SDRAM
//#define MICROPY_HW_SDRAM_ROW_BITS_NUM       13
//#define MICROPY_HW_SDRAM_MEM_BUS_WIDTH      16
//#define MICROPY_HW_SDRAM_REFRESH_CYCLES     8192

// 32-bit SDRAM
#define MICROPY_HW_SDRAM_ROW_BITS_NUM       12
#define MICROPY_HW_SDRAM_MEM_BUS_WIDTH      32
#define MICROPY_HW_SDRAM_REFRESH_CYCLES     4096

#define MICROPY_HW_SDRAM_COLUMN_BITS_NUM    9
#define MICROPY_HW_SDRAM_INTERN_BANKS_NUM   4
#define MICROPY_HW_SDRAM_RPIPE_DELAY        0
#define MICROPY_HW_SDRAM_RBURST             (1)
#define MICROPY_HW_SDRAM_WRITE_PROTECTION   (0)

#define MICROPY_HW_SDRAM_AUTOREFRESH_NUM    (8)
#define MICROPY_HW_SDRAM_BURST_LENGTH       1

#define MICROPY_HW_SDRAM_REFRESH_RATE       (64) // ms

#define MICROPY_HW_FMC_SDCKE0   (pin_C5)
#define MICROPY_HW_FMC_SDNE0    (pin_C4)
#define MICROPY_HW_FMC_SDCLK    (pin_G8)
#define MICROPY_HW_FMC_SDNCAS   (pin_G15)
#define MICROPY_HW_FMC_SDNRAS   (pin_F11)
#define MICROPY_HW_FMC_SDNWE    (pin_A7)
#define MICROPY_HW_FMC_BA0      (pin_G4)
#define MICROPY_HW_FMC_BA1      (pin_G5)
#define MICROPY_HW_FMC_NBL0     (pin_E0)
#define MICROPY_HW_FMC_NBL1     (pin_E1)
#define MICROPY_HW_FMC_NBL2     (pin_I4)
#define MICROPY_HW_FMC_NBL3     (pin_I5)
#define MICROPY_HW_FMC_A0       (pin_F0)
#define MICROPY_HW_FMC_A1       (pin_F1)
#define MICROPY_HW_FMC_A2       (pin_F2)
#define MICROPY_HW_FMC_A3       (pin_F3)
#define MICROPY_HW_FMC_A4       (pin_F4)
#define MICROPY_HW_FMC_A5       (pin_F5)
#define MICROPY_HW_FMC_A6       (pin_F12)
#define MICROPY_HW_FMC_A7       (pin_F13)
#define MICROPY_HW_FMC_A8       (pin_F14)
#define MICROPY_HW_FMC_A9       (pin_F15)
#define MICROPY_HW_FMC_A10      (pin_G0)
#define MICROPY_HW_FMC_A11      (pin_G1)
#define MICROPY_HW_FMC_A12      (pin_G2)
#define MICROPY_HW_FMC_D0       (pin_D14)
#define MICROPY_HW_FMC_D1       (pin_D15)
#define MICROPY_HW_FMC_D2       (pin_D0)
#define MICROPY_HW_FMC_D3       (pin_D1)
#define MICROPY_HW_FMC_D4       (pin_E7)
#define MICROPY_HW_FMC_D5       (pin_E8)
#define MICROPY_HW_FMC_D6       (pin_E9)
#define MICROPY_HW_FMC_D7       (pin_E10)
#define MICROPY_HW_FMC_D8       (pin_E11)
#define MICROPY_HW_FMC_D9       (pin_E12)
#define MICROPY_HW_FMC_D10      (pin_E13)
#define MICROPY_HW_FMC_D11      (pin_E14)
#define MICROPY_HW_FMC_D12      (pin_E15)
#define MICROPY_HW_FMC_D13      (pin_D8)
#define MICROPY_HW_FMC_D14      (pin_D9)
#define MICROPY_HW_FMC_D15      (pin_D10)
#define MICROPY_HW_FMC_D16      (pin_H8)
#define MICROPY_HW_FMC_D17      (pin_H9)
#define MICROPY_HW_FMC_D18      (pin_H10)
#define MICROPY_HW_FMC_D19      (pin_H11)
#define MICROPY_HW_FMC_D20      (pin_H12)
#define MICROPY_HW_FMC_D21      (pin_H13)
#define MICROPY_HW_FMC_D22      (pin_H14)
#define MICROPY_HW_FMC_D23      (pin_H15)
#define MICROPY_HW_FMC_D24      (pin_I0)
#define MICROPY_HW_FMC_D25      (pin_I1)
#define MICROPY_HW_FMC_D26      (pin_I2)
#define MICROPY_HW_FMC_D27      (pin_I3)
#define MICROPY_HW_FMC_D28      (pin_I6)
#define MICROPY_HW_FMC_D29      (pin_I7)
#define MICROPY_HW_FMC_D30      (pin_I9)
#define MICROPY_HW_FMC_D31      (pin_I10)
