// Board and hardware specific configuration
#include "RP2040.h"
#define MICROPY_HW_BOARD_NAME                   "Raspberry Pi Pico"
#define MICROPY_HW_FLASH_STORAGE_BYTES          (1280 * 1024)

// Enable USB Mass Storage with FatFS filesystem.
#define MICROPY_HW_USB_MSC  (1)
