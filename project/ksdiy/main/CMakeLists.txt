# Set location of base MicroPython directory.
get_filename_component(MICROPY_DIR ${PROJECT_DIR}/../../submodule/micropython ABSOLUTE)

option(CONFIG_OPENMV_SUPPORT "option for openmv" ON)

# Include core source components.
include(${MICROPY_DIR}/py/py.cmake)
include(${PROJECT_DIR}/src/omv_mlab/omv.cmake)

if(MICROPY_PY_LVGL)
set(LV_BINDINGS_DIR ${PROJECT_DIR}/components/lv_bindings)
# file(COPY "${LV_BINDINGS_DIR}/driver/esp32/espidf.c" DESTINATION "${PROJECT_DIR}/src/lvgl_mlab/srcs")
file(COPY "${PROJECT_DIR}/src/pyexec.c" DESTINATION "${PROJECT_DIR}/../../submodule/micropython/shared/runtime")
file(COPY "${PROJECT_DIR}/src/pyexec.h" DESTINATION "${PROJECT_DIR}/../../submodule/micropython/shared/runtime")

include(${PROJECT_DIR}/src/lvgl_mlab/mkrules.cmake)
endif()

if(NOT CMAKE_BUILD_EARLY_EXPANSION)
    include(${MICROPY_DIR}/py/usermod.cmake)
    include(${MICROPY_DIR}/extmod/extmod.cmake)
endif()

set(MICROPY_QSTRDEFS_PORT
    ${PROJECT_DIR}/src/adapter/qstrdefsport.h    
)

set(MICROPY_SOURCE_SHARED
    ${MICROPY_DIR}/shared/readline/readline.c
    ${MICROPY_DIR}/shared/netutils/netutils.c
    ${MICROPY_DIR}/shared/timeutils/timeutils.c
    ${MICROPY_DIR}/shared/runtime/interrupt_char.c
    #${MICROPY_DIR}/shared/runtime/stdout_helpers.c
    ${MICROPY_DIR}/shared/runtime/sys_stdio_mphal.c
    ${MICROPY_DIR}/shared/runtime/pyexec.c
    #${MICROPY_DIR}/shared/runtime/gchelper_generic.c
)

set(MICROPY_SOURCE_LIB
    ${MICROPY_DIR}/lib/littlefs/lfs1.c
    ${MICROPY_DIR}/lib/littlefs/lfs1_util.c
    ${MICROPY_DIR}/lib/littlefs/lfs2.c
    ${MICROPY_DIR}/lib/littlefs/lfs2_util.c
    ${MICROPY_DIR}/lib/mbedtls_errors/mp_mbedtls_errors.c
    ${MICROPY_DIR}/lib/oofatfs/ff.c
    ${MICROPY_DIR}/lib/oofatfs/ffunicode.c
)
if(IDF_TARGET STREQUAL "esp32c3")
    list(APPEND MICROPY_SOURCE_LIB ${MICROPY_DIR}/shared/runtime/gchelper_generic.c)
endif()

set(MICROPY_SOURCE_DRIVERS
    ${MICROPY_DIR}/drivers/bus/softspi.c
    ${MICROPY_DIR}/drivers/dht/dht.c
)

set(MICROPY_SOURCE_PORT
    ${PROJECT_DIR}/src/main.c
    ${PROJECT_DIR}/src/firmware_version.c
    ${PROJECT_DIR}/src/adapter/uart.c
    #${PROJECT_DIR}/src/adapter/usb.c
    ${PROJECT_DIR}/src/adapter/gccollect.c
    ${PROJECT_DIR}/src/adapter/mphalport.c
    ${PROJECT_DIR}/src/adapter/fatfs_port.c
    ${PROJECT_DIR}/src/adapter/help.c
    ${PROJECT_DIR}/src/adapter/modutime.c
    ${PROJECT_DIR}/src/adapter/moduos.c
    ${PROJECT_DIR}/src/adapter/machine_timer.c
    ${PROJECT_DIR}/src/adapter/machine_pin.c
    ${PROJECT_DIR}/src/adapter/machine_touchpad.c
    ${PROJECT_DIR}/src/adapter/machine_adc.c
    #${PROJECT_DIR}/src/adapter/machine_dac.c
    ${PROJECT_DIR}/src/adapter/machine_i2c.c
    ${PROJECT_DIR}/src/adapter/machine_i2s.c
    ${PROJECT_DIR}/src/adapter/machine_pwm.c
    ${PROJECT_DIR}/src/adapter/machine_uart.c
    ${PROJECT_DIR}/src/adapter/modmachine.c
    ${PROJECT_DIR}/src/adapter/modnetwork.c
    ${PROJECT_DIR}/src/adapter/network_lan.c
    ${PROJECT_DIR}/src/adapter/network_ppp.c
    ${PROJECT_DIR}/src/adapter/mpnimbleport.c
    ${PROJECT_DIR}/src/adapter/modsocket.c
    ${PROJECT_DIR}/src/adapter/modesp.c
    ${PROJECT_DIR}/src/adapter/esp32_nvs.c
    ${PROJECT_DIR}/src/adapter/esp32_partition.c
    ${PROJECT_DIR}/src/adapter/esp32_rmt.c
    ${PROJECT_DIR}/src/adapter/esp32_ulp.c
    ${PROJECT_DIR}/src/adapter/modesp32.c
    ${PROJECT_DIR}/src/adapter/espneopixel.c
    ${PROJECT_DIR}/src/adapter/machine_hw_spi.c
    ${PROJECT_DIR}/src/adapter/machine_wdt.c
    ${PROJECT_DIR}/src/adapter/mpthreadport.c
    ${PROJECT_DIR}/src/adapter/machine_rtc.c
    ${PROJECT_DIR}/src/adapter/machine_sdcard.c
    ${PROJECT_DIR}/src/mpy_modules/modmatatalab.c
    ${PROJECT_DIR}/src/mpy_modules/system/system_management.c
    ${PROJECT_DIR}/src/mpy_modules/system/home_key.c
    ${PROJECT_DIR}/src/mpy_modules/helloword.c
    ${PROJECT_DIR}/src/mpy_modules/mod_sensor.c
    ${PROJECT_DIR}/src/mpy_modules/mod_motor.c
    ${PROJECT_DIR}/src/test_esp32lvgl.c
    ${PROJECT_DIR}/src/lcd/who_lcd.c
)

set(AITEST_SOURCE_DRIVERS
    ${PROJECT_DIR}/src/driver/drv_aw9523b.c
    ${PROJECT_DIR}/src/driver/drv_i2c.c
    ${PROJECT_DIR}/src/driver/drv_qmc6310.c
    ${PROJECT_DIR}/src/driver/drv_qmi8658.c
    ${PROJECT_DIR}/src/driver/drv_button.c
    ${PROJECT_DIR}/src/driver/drv_9_sensor.c
    ${PROJECT_DIR}/src/driver/drv_ring_buf.c
    ${PROJECT_DIR}/src/driver/uart1.c
    ${PROJECT_DIR}/src/driver/drv_rgb_led.c
    ${PROJECT_DIR}/src/driver/drv_nvs.c
    ${PROJECT_DIR}/src/driver/drv_motor.c
    ${PROJECT_DIR}/src/driver/drv_hub.c
    
)

if(CONFIG_CHERRY_USB_ENABLED)
	list(APPEND CHERRYUSB_SRCS 
	${PROJECT_DIR}/src/usb_cherry/addition/usb_cherry_platform.c
	${PROJECT_DIR}/src/usb_cherry/CherryUSB/class/cdc/usbd_cdc.c
	${PROJECT_DIR}/src/usb_cherry/CherryUSB/class/msc/usbd_msc.c
	${PROJECT_DIR}/src/usb_cherry/CherryUSB/core/usbd_core.c
	${PROJECT_DIR}/src/usb_cherry/CherryUSB/port/dwc2/usb_dc_dwc2.c)
	list(APPEND MICROPY_SOURCE_PORT ${PROJECT_DIR}/src/usb/cdc_acm_cherryusb.c)
else()
	list(APPEND MICROPY_SOURCE_PORT ${PROJECT_DIR}/src/usb/usb_cdc.c)
endif()

set(MICROPY_SOURCE_QSTR
    ${MICROPY_SOURCE_PY}
    ${MICROPY_SOURCE_EXTMOD}
    ${MICROPY_SOURCE_USERMOD}
    ${MICROPY_SOURCE_SHARED}
    ${MICROPY_SOURCE_LIB}
    ${AITEST_SOURCE_DRIVERS}
    ${MICROPY_SOURCE_PORT}
    ${OMV_MOD_SRCS}
    ${LV_SRC}
    
)

set(IDF_COMPONENTS
    app_update
    bootloader_support
    bt
    driver
    esp_common
    esp_eth
    esp_event
    esp_ringbuf
    esp_rom
    esp_lcd
    esp_wifi
    freertos
    heap
    log
    lwip
    mbedtls
    mdns
    newlib
    nvs_flash
    sdmmc
    soc
    spi_flash
    tcpip_adapter
    ulp
    vfs
    xtensa
    usb
)

if(IDF_VERSION_MINOR GREATER_EQUAL 1)
    list(APPEND IDF_COMPONENTS esp_netif)
endif()

if(IDF_VERSION_MINOR GREATER_EQUAL 2)
    list(APPEND IDF_COMPONENTS esp_system)
    list(APPEND IDF_COMPONENTS esp_timer)
endif()

if(IDF_VERSION_MINOR GREATER_EQUAL 3)
    list(APPEND IDF_COMPONENTS esp_hw_support)
    list(APPEND IDF_COMPONENTS esp_pm)
    list(APPEND IDF_COMPONENTS hal)
endif()

if(IDF_TARGET STREQUAL "esp32")
    list(APPEND IDF_COMPONENTS esp32)
elseif(IDF_TARGET STREQUAL "esp32s2")
    list(APPEND IDF_COMPONENTS esp32s2)
    list(APPEND IDF_COMPONENTS tinyusb)
elseif(IDF_TARGET STREQUAL "esp32s3")
    list(APPEND IDF_COMPONENTS esp32s3)
    list(APPEND IDF_COMPONENTS tinyusb)
    list(APPEND IDF_COMPONENTS esp32-camera)
endif()

# Register the main IDF component.
idf_component_register(
    SRCS
        ${MICROPY_SOURCE_PY}
        ${MICROPY_SOURCE_EXTMOD}
        ${MICROPY_SOURCE_SHARED}
        ${MICROPY_SOURCE_LIB}
        ${AITEST_SOURCE_DRIVERS}
        ${MICROPY_SOURCE_DRIVERS}
        ${MICROPY_SOURCE_PORT}
        ${OMV_SRCS}
	    ${LV_SRC}
        #${CHERRYUSB_SRCS}
    INCLUDE_DIRS
        ${MICROPY_INC_CORE}
        ${MICROPY_INC_USERMOD}
        ${MICROPY_PORT_DIR}
        ${PROJECT_DIR}/src/usb      
        #${PROJECT_DIR}/src/usb_cherry/include
        #${PROJECT_DIR}/src/usb_cherry/CherryUSB/class/cdc
        #${PROJECT_DIR}/src/usb_cherry/CherryUSB/class/msc
        #${PROJECT_DIR}/src/usb_cherry/CherryUSB/common
        #${PROJECT_DIR}/src/usb_cherry/CherryUSB/core
        #${PROJECT_DIR}/src/usb_cherry/CherryUSB/port/dwc2
        ${PROJECT_DIR}/src/lcd/
        ${PROJECT_DIR}/src/driver/
        ${MICROPY_PORT_DIR}/src/adapter/
        ${OMV_INCLUDEDIRS}
        ${MICROPY_BOARD_DIR}
        ${CMAKE_BINARY_DIR}
        ${MICROPY_DIR}/py  
        ${PROJECT_DIR}/src/lvgl_mlab/inc
        ${LV_INCLUDE}
        ${PROJECT_DIR}/components
        ${PROJECT_DIR}/src/mpy_modules/system
        
    REQUIRES
        ${IDF_COMPONENTS}
)

# Set the MicroPython target as the current (main) IDF component target.
set(MICROPY_TARGET ${COMPONENT_TARGET})

# Define mpy-cross flags, for use with frozen code.
set(MICROPY_CROSS_FLAGS -march=xtensawin)

# Set compile options for this port.
target_compile_definitions(${MICROPY_TARGET} PUBLIC
    CONFIG_NN_OPTIMIZED=1
	CONFIG_NN_OPTIMIZATIONS=1
    CONFIG_OPENMV_SUPPORT=1
    ${MICROPY_DEF_CORE}
    MICROPY_ESP_IDF_4=1
    MICROPY_VFS_FAT=1
    MICROPY_VFS_LFS2=1
    FFCONF_H=\"${MICROPY_OOFATFS_DIR}/ffconf.h\"
    LFS1_NO_MALLOC LFS1_NO_DEBUG LFS1_NO_WARN LFS1_NO_ERROR LFS1_NO_ASSERT
    LFS2_NO_MALLOC LFS2_NO_DEBUG LFS2_NO_WARN LFS2_NO_ERROR LFS2_NO_ASSERT
    ${LV_DEFINE}
)

# Disable some warnings to keep the build output clean.
target_compile_options(${MICROPY_TARGET} PUBLIC
    -Wno-clobbered
    -Wno-deprecated-declarations
    -Wno-missing-field-initializers
    -Wno-error=type-limits
    -Wno-error=maybe-uninitialized 
    -Wno-error=unused-but-set-parameter
    -Wno-error=uninitialized
    -Wno-error=implicit-function-declaration
    -Wno-error=return-type
    -Wno-error=format=
    -Wno-error=sizeof-pointer-memaccess
    
)

# Add additional extmod and usermod components.
target_link_libraries(${MICROPY_TARGET} micropy_extmod_btree)
target_link_libraries(${MICROPY_TARGET} usermod)


# Collect all of the include directories and compile definitions for the IDF components.
foreach(comp ${IDF_COMPONENTS})
    micropy_gather_target_properties(__idf_${comp})
endforeach()

if(IDF_VERSION_MINOR GREATER_EQUAL 2)
    # These paths cannot currently be found by the IDF_COMPONENTS search loop above,
    # so add them explicitly.
    list(APPEND MICROPY_CPP_INC_EXTRA ${IDF_PATH}/components/soc/${IDF_TARGET}/include/soc)
    list(APPEND MICROPY_CPP_INC_EXTRA ${IDF_PATH}/components/soc/${IDF_TARGET})
    list(APPEND MICROPY_CPP_INC_EXTRA ${IDF_PATH}/components/soc/include/soc)
    if(IDF_VERSION_MINOR GREATER_EQUAL 3)
         list(APPEND MICROPY_CPP_INC_EXTRA ${IDF_PATH}/components/tinyusb/additions/include)
         list(APPEND MICROPY_CPP_INC_EXTRA ${IDF_PATH}/components/tinyusb/tinyusb/src)
    endif()
endif()
# Include the main MicroPython cmake rules.
include(${MICROPY_DIR}/py/mkrules.cmake)
add_compile_options(-fdiagnostics-color=always)
add_definitions(-w)
# Include LVGL bindings rules
if(MICROPY_PY_LVGL)
# Add lv_bindings rules
all_lv_bindings()
# Include LVGL component, ignore KCONFIG
#idf_build_component(${PROJECT_DIR}/components/lvgl)
endif()
