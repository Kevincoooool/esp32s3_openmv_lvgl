if(ESP_PLATFORM)

file(GLOB_RECURSE SOURCES lv_examples/*.c)
file(GLOB_RECURSE ASSETS ../lvgl/examples/assets/*.c)

idf_component_register(SRCS ${SOURCES}
                       ${ASSETS}
                       INCLUDE_DIRS .
                       REQUIRES lvgl)

else()
    message(FATAL_ERROR "LVGL LV examples: ESP_PLATFORM is not defined. Try reinstalling ESP-IDF.")
endif()
