/**
 * @file lv_demo_conf.h
 * Configuration file for v8.1.0-dev
 *
 */
/*
 * COPY THIS FILE AS lv_demo_conf.h
 */

#if 1 /*Set it to "1" to enable the content*/

#ifndef LV_DEMO_CONF_H
#define LV_DEMO_CONF_H


/*******************
 * GENERAL SETTING
 *******************/
#if CONFIG_LV_EX_PRINTF
#define LV_EX_PRINTF       CONFIG_LV_EX_PRINTF 
#else
#define LV_EX_PRINTF       0       /*Enable printf-ing data in demoes and examples*/
#endif
#define LV_EX_KEYBOARD     0       /*Add PC keyboard support to some examples (`lv_drivers` repository is required)*/
#define LV_EX_MOUSEWHEEL   0       /*Add 'encoder' (mouse wheel) support to some examples (`lv_drivers` repository is required)*/

/*********************
 * DEMO USAGE
 *********************/

/*Show some widget*/
#if defined CONFIG_LV_USE_DEMO_WIDGETS
#define LV_USE_DEMO_WIDGETS               CONFIG_LV_USE_DEMO_WIDGETS
#else 
#define LV_USE_DEMO_WIDGETS               0
#endif
#if LV_USE_DEMO_WIDGETS
    #if CONFIG_LV_DEMO_WIDGETS_SLIDESHOW
    #define LV_DEMO_WIDGETS_SLIDESHOW     CONFIG_LV_DEMO_WIDGETS_SLIDESHOW
    #else
    #define LV_DEMO_WIDGETS_SLIDESHOW     0
    #endif
#endif

/*Demonstrate the usage of encoder and keyboard*/
#if CONFIG_LV_USE_DEMO_KEYPAD_AND_ENCODER
#define LV_USE_DEMO_KEYPAD_AND_ENCODER    CONFIG_LV_USE_DEMO_KEYPAD_AND_ENCODER
#else
#define LV_USE_DEMO_KEYPAD_AND_ENCODER    0
#endif

/*Benchmark your system*/
#if CONFIG_LV_USE_DEMO_BENCHMARK
#define LV_USE_DEMO_BENCHMARK             CONFIG_LV_USE_DEMO_BENCHMARK
#else
#define LV_USE_DEMO_BENCHMARK             0
#endif

/*Stress test for LVGL*/
#if LV_USE_DEMO_STRESS
#define LV_USE_DEMO_STRESS                LV_USE_DEMO_STRESS
#else
#define LV_USE_DEMO_STRESS                0
#endif

/*Music player demo*/
#if CONFIG_LV_USE_DEMO_MUSIC
#define LV_USE_DEMO_MUSIC                 CONFIG_LV_USE_DEMO_MUSIC
#else
#define LV_USE_DEMO_MUSIC                 0
#endif
#if LV_USE_DEMO_MUSIC
    #if CONFIG_LV_DEMO_MUSIC_LANDSCAPE
    #define LV_DEMO_MUSIC_LANDSCAPE       CONFIG_LV_DEMO_WIDGETS_SLIDESHOW
    #else
    #define LV_DEMO_MUSIC_LANDSCAPE       0
    #endif
    #if CONFIG_LV_DEMO_MUSIC_LARGE
    #define LV_DEMO_MUSIC_LARGE           CONFIG_LV_DEMO_MUSIC_LARGE
    #else
    #define LV_DEMO_MUSIC_LARGE           0
    #endif
    #if CONFIG_LV_DEMO_MUSIC_AUTO_PLAY
    #define LV_DEMO_MUSIC_AUTO_PLAY       CONFIG_LV_DEMO_MUSIC_AUTO_PLAY
    #else
    #define LV_DEMO_MUSIC_AUTO_PLAY       0
    #endif
#endif

#endif /*LV_DEMO_CONF_H*/

#endif /*End of "Content enable"*/

