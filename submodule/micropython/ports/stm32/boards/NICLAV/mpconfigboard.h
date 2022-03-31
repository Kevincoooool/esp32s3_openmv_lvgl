#define MICROPY_HW_BOARD_NAME       "NICLAV"
#define MICROPY_HW_MCU_NAME         "STM32H747"
#define MICROPY_PY_SYS_PLATFORM     "Nicla Vision"
#define MICROPY_HW_FLASH_FS_LABEL   "nicla"
#define MICROPY_PY_SYS_STDIO_BUFFER (0)
#define MICROPY_STREAMS_POSIX_API   (1)

#define MICROPY_OBJ_REPR            (MICROPY_OBJ_REPR_C)
#define UINT_FMT                    "%u"
#define INT_FMT                     "%d"
typedef int mp_int_t;               // must be pointer size
typedef unsigned int mp_uint_t;     // must be pointer size

#define MICROPY_FATFS_EXFAT         (1)
#define MICROPY_HW_ENABLE_RTC       (1)
#define MICROPY_HW_ENABLE_RNG       (1)
#define MICROPY_HW_ENABLE_ADC       (1)
#define MICROPY_HW_ENABLE_DAC       (1)
#define MICROPY_HW_ENABLE_USB       (1)
#define MICROPY_HW_ENABLE_SPI4      (1)
#define MICROPY_HW_HAS_SWITCH       (0)
#define MICROPY_HW_HAS_FLASH        (1)
#define MICROPY_HW_ENABLE_SERVO     (1)
#define MICROPY_HW_ENABLE_TIMER     (1)
#define MICROPY_HW_ENABLE_SDCARD    (0)
#define MICROPY_HW_ENABLE_MMCARD    (0)
// Reserved DMA streams
#define MICROPY_HW_DMA1S1_IS_RESERVED
#define MICROPY_HW_DMA2S1_IS_RESERVED
#define MICROPY_HW_TIM_IS_RESERVED(id) (id == 3 || id == 6)

#define MICROPY_HW_CLK_USE_BYPASS   (1)

#define MICROPY_BOARD_EARLY_INIT NICLAV_board_early_init
void NICLAV_board_early_init(void);

#define MICROPY_RESET_TO_BOOTLOADER NICLAV_reboot_to_bootloader
void NICLAV_reboot_to_bootloader(void);

void NICLAV_board_low_power(int mode);
#define MICROPY_BOARD_LEAVE_STOP    NICLAV_board_low_power(0);
#define MICROPY_BOARD_ENTER_STOP    NICLAV_board_low_power(1);
#define MICROPY_BOARD_ENTER_STANDBY NICLAV_board_low_power(2);

void NICLAV_board_osc_enable(int enable);
#define MICROPY_BOARD_OSC_ENABLE    NICLAV_board_osc_enable(1);
#define MICROPY_BOARD_OSC_DISABLE   NICLAV_board_osc_enable(0);

// There is an external 32kHz oscillator
#define RTC_ASYNCH_PREDIV           (0)
#define RTC_SYNCH_PREDIV            (0x7fff)
#define MICROPY_HW_RTC_USE_BYPASS   (1)
#define MICROPY_HW_RTC_USE_US       (1)
#define MICROPY_HW_RTC_USE_CALOUT   (1)

#define MICROPY_HW_ENABLE_INTERNAL_FLASH_STORAGE (0)
#define MICROPY_HW_SPIFLASH_ENABLE_CACHE (1)

// QSPI Flash 128Mbit
#define MICROPY_HW_QSPI_PRESCALER           (4) //200MHz/4=50MHz
#define MICROPY_HW_QSPI_SAMPLE_SHIFT	    (0)
#define MICROPY_HW_SPIFLASH_SIZE_BITS       (120 * 1024 * 1024)
#define MICROPY_HW_QSPIFLASH_SIZE_BITS_LOG2 (27)
#define MICROPY_HW_QSPIFLASH_CS             (pyb_pin_QSPI2_CS)
#define MICROPY_HW_QSPIFLASH_SCK            (pyb_pin_QSPI2_CLK)
#define MICROPY_HW_QSPIFLASH_IO0            (pyb_pin_QSPI2_D0)
#define MICROPY_HW_QSPIFLASH_IO1            (pyb_pin_QSPI2_D1)
#define MICROPY_HW_QSPIFLASH_IO2            (pyb_pin_QSPI2_D2)
#define MICROPY_HW_QSPIFLASH_IO3            (pyb_pin_QSPI2_D3)

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

// 4 wait states
#define MICROPY_HW_FLASH_LATENCY    FLASH_LATENCY_2

// UART config
#define MICROPY_HW_UART3_TX         (pin_C10)
#define MICROPY_HW_UART3_RX         (pin_C11)

// I2C busses
#define MICROPY_HW_I2C1_SCL         (pin_B8)
#define MICROPY_HW_I2C1_SDA         (pin_B9)

#define MICROPY_HW_I2C2_SCL         (pin_F1)
#define MICROPY_HW_I2C2_SDA         (pin_F0)

#define MICROPY_HW_I2C3_SCL         (pin_A8)
#define MICROPY_HW_I2C3_SDA         (pin_C9)

// SPI buses
#define MICROPY_HW_SPI4_NSS         (pin_E11)
#define MICROPY_HW_SPI4_SCK         (pin_E12)
#define MICROPY_HW_SPI4_MISO        (pin_E13)
#define MICROPY_HW_SPI4_MOSI        (pin_E14)

// LEDs
#define MICROPY_HW_LED1             (pyb_pin_LEDR) // red
#define MICROPY_HW_LED2             (pyb_pin_LEDG) // green
#define MICROPY_HW_LED3             (pyb_pin_LEDB) // yellow
#define MICROPY_HW_LED_ON(pin)      (mp_hal_pin_low(pin))
#define MICROPY_HW_LED_OFF(pin)     (mp_hal_pin_high(pin))

// WiFi SDMMC
#define MICROPY_HW_SDIO_SDMMC       (2)
#define MICROPY_HW_SDIO_CK          (pin_D6)
#define MICROPY_HW_SDIO_CMD         (pin_D7)
#define MICROPY_HW_SDIO_D0          (pin_B14)
#define MICROPY_HW_SDIO_D1          (pin_B15)
#define MICROPY_HW_SDIO_D2          (pin_G11)
#define MICROPY_HW_SDIO_D3          (pin_B4)

// USB config
#define MICROPY_HW_USB_HS           (1)
#define MICROPY_HW_USB_HS_ULPI_NXT  (pin_C3)
#define MICROPY_HW_USB_HS_ULPI_DIR  (pin_C2)

//#define MICROPY_HW_USB_FS           (1)
#define MICROPY_HW_USB_CDC_RX_DATA_SIZE     (512)
#define MICROPY_HW_USB_CDC_TX_DATA_SIZE     (512)
#define MICROPY_HW_USB_CDC_1200BPS_TOUCH    (1)
#define GPIO_AF10_OTG_HS                    (GPIO_AF10_OTG2_HS)

#define MICROPY_HW_USB_VID                      0x2341
#define MICROPY_HW_USB_PID                      0x045F
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
#define MICROPY_HW_USB_MANUFACTURER_STRING      "Arduino"
#define MICROPY_HW_USB_PRODUCT_FS_STRING        "Nicla Vision Virtual Comm Port in FS Mode"
#define MICROPY_HW_USB_PRODUCT_HS_STRING        "Nicla Vision Virtual Comm Port in HS Mode"
//#define MICROPY_HW_USB_SERIALNUMBER_FS_STRING   "000000000011"
//#define MICROPY_HW_USB_SERIALNUMBER_HS_STRING   "000000000010"
#define MICROPY_HW_USB_CONFIGURATION_FS_STRING  "Nicla Vision Config"
#define MICROPY_HW_USB_CONFIGURATION_HS_STRING  "Nicla Vision Config"
#define MICROPY_HW_USB_INTERFACE_FS_STRING      "Nicla Vision Interface"
#define MICROPY_HW_USB_INTERFACE_HS_STRING      "Nicla Vision Interface"
