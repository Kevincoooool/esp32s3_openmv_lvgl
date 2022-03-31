USE_MBOOT ?= 0

# MCU settings
MCU_SERIES = h7
CMSIS_MCU = STM32H747xx
AF_FILE = boards/stm32h743_af.csv
CFLAGS_MOD = -DCORE_CM7

# MicroPython settings
OPENAMP_PY = 0
MICROPY_JERRYSCRIPT = 0
MICROPY_PY_LWIP = 0
MICROPY_PY_RNDIS = 0
MICROPY_PY_NETWORK_CYW43 = 0
#MICROPY_PY_USSL = 1
#MICROPY_SSL_MBEDTLS = 1
#MICROPY_SSL_AXTLS = 1

ifeq ($(USE_MBOOT),1)
# When using Mboot all the text goes together after the filesystem
LD_FILES = boards/stm32h747.ld boards/common_blifs.ld
TEXT0_ADDR = 0x08040000
# TEXT1_ADDR = 0x08100000 - We'll reserve second flash bank for the Cortex-M4
else
# When not using Mboot the ISR text goes first, then the rest after the filesystem
LD_FILES = boards/stm32h747.ld boards/common_ifs.ld
TEXT0_ADDR = 0x08000000
TEXT1_ADDR = 0x08040000
# TEXT2_ADDR = 0x08100000 - We'll reserve second flash bank for the Cortex-M4
endif
