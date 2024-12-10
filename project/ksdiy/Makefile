# Makefile for MicroPython on ESP32.
#
# This is a simple, convenience wrapper around idf.py (which uses cmake).

# Select the board to build for, defaulting to GENERIC.
BOARD ?= GENERIC_S3

# If the build directory is not given, make it reflect the board name.
BUILD ?= build-$(BOARD)

# Device serial settings.
PORT ?= /dev/ttyACM0
BAUD ?= 2000000

PYTHON ?= python3

.PHONY: all clean deploy erase submodules FORCE monitor flash

CMAKE_ARGS =

ifdef USER_C_MODULES
	CMAKE_ARGS += -DUSER_C_MODULES=${USER_C_MODULES}
endif

IDFPY_FLAGS += -D MICROPY_BOARD=$(BOARD) -D MICROPY_PY_LVGL=1 -B $(BUILD) $(CMAKE_ARGS)

ifdef FROZEN_MANIFEST
       IDFPY_FLAGS += -D MICROPY_FROZEN_MANIFEST=$(FROZEN_MANIFEST)
endif

all:
	main/copy_leftmodules.sh
	rm -rf $(BUILD)/frozen_content.c && idf.py $(IDFPY_FLAGS) build
	@$(PYTHON) makeimg.py \
		$(BUILD)/bootloader/bootloader.bin \
		$(BUILD)/partition_table/partition-table.bin \
    		$(BUILD)/micropython.bin \
    		$(BUILD)/firmware.bin

$(BUILD)/bootloader/bootloader.bin $(BUILD)/partition_table/partition-table.bin $(BUILD)/micropython.bin: FORCE

clean:
	idf.py $(IDFPY_FLAGS) fullclean

deploy:
	idf.py $(IDFPY_FLAGS) menuconfig
	idf.py $(IDFPY_FLAGS) -p $(PORT) -b $(BAUD) flash monitor
flash:
	# idf.py $(IDFPY_FLAGS) menuconfig
	idf.py $(IDFPY_FLAGS) -p $(PORT) -b $(BAUD) flash monitor
erase:
	idf.py $(IDFPY_FLAGS) -p $(PORT) -b $(BAUD) erase_flash
monitor:
	idf.py $(IDFPY_FLAGS) -p $(PORT) -b $(BAUD) monitor
