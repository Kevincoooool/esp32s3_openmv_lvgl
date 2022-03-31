/* LVGL Example project
 *
 * Basic project to test LVGL on ESP32 based projects.
 *
 * This example code is in the Public Domain (or CC0 licensed, at your option.)
 *
 * Unless required by applicable law or agreed to in writing, this
 * software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
 * CONDITIONS OF ANY KIND, either express or implied.
 */
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_freertos_hooks.h"
#include "freertos/semphr.h"
#include "esp_system.h"
#include "esp_log.h"
#include "driver/gpio.h"

/* Littlevgl specific */
#ifdef LV_LVGL_H_INCLUDE_SIMPLE
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif

#include "lvgl_helpers.h"
#include "drv_aw9523b.h"
#include "drv_nvs.h"

#ifndef CONFIG_LV_TFT_DISPLAY_MONOCHROME
    #if defined CONFIG_LV_USE_DEMO_WIDGETS
        #include "../components/lv_examples/lv_examples/src/lv_demo_widgets/lv_demo_widgets.h"
    #elif defined CONFIG_LV_USE_DEMO_KEYPAD_AND_ENCODER
        #include "../components/lv_examples/lv_examples/src/lv_demo_keypad_encoder/lv_demo_keypad_encoder.h"
    #elif defined CONFIG_LV_USE_DEMO_BENCHMARK
        #include "../components/lv_examples/lv_examples/src/lv_demo_benchmark/lv_demo_benchmark.h"
    #elif defined CONFIG_LV_USE_DEMO_STRESS
        #include "../components/lv_examples/lv_examples/src/lv_demo_stress/lv_demo_stress.h"
    #elif defined CONFIG_LV_USE_DEMO_MUSIC
        #include "../components/lv_examples/lv_examples/src/lv_demo_music/lv_demo_music.h"
    #else
        #error "No demo application selected."
    #endif
#endif

//#define BOARD_MATATALAB
//#define BOARD_S3_DevKitC
#define BOARD_MATATALAB_SP4

/*********************
 *      DEFINES
 *********************/
#define TAG "demo"
#define LV_TICK_PERIOD_MS 1

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lv_tick_task(void *arg);
static void guiTask(void *pvParameter);
static void create_demo_application(void);
static void guiStart(void);

#include "py/runtime.h"
#include "py/stackctrl.h"
#include "py/mpthread.h"
typedef struct _thread_entry_args_t {
    mp_obj_dict_t *dict_locals;
    mp_obj_dict_t *dict_globals;
    size_t stack_size;
    //mp_obj_t fun;
    void *(*fun)(void *);
    size_t n_args;
    size_t n_kw;
    mp_obj_t args[];
} thread_entry_args_t;

/**********************
 *   APPLICATION MAIN
 **********************/
static void *thread_entry(void *args_in)
{
    thread_entry_args_t *args = (thread_entry_args_t *)args_in;

    mp_state_thread_t ts;
    mp_thread_set_state(&ts);

    mp_stack_set_top(&ts + 1); // need to include ts in root-pointer scan
    mp_stack_set_limit(args->stack_size);

  // The GC starts off unlocked on this thread.
    ts.gc_lock_depth = 0;

    ts.mp_pending_exception = MP_OBJ_NULL;

    // set locals and globals from the calling context
    mp_locals_set(args->dict_locals);
    mp_globals_set(args->dict_globals);

    //MP_THREAD_GIL_ENTER();

    // signal that we are set up and running
    mp_thread_start();

    // TODO set more thread-specific state here:
    //  cur_exception (root pointer)

    printf("[thread] start ts=%p args=%p stack=%p\n", &ts, &args, MP_STATE_THREAD(stack_top));
    args->fun(args_in);
    printf("[thread] finish ts=%p\n", &ts);

    // signal that we are finished
    mp_thread_finish();

    //MP_THREAD_GIL_EXIT();

    return NULL;
}

void test_esp32lvgl(void) {
    /* If you want to use a task to create the graphic, you NEED to create a Pinned task
     * Otherwise there can be problem such as memory corruption and so on.
     * NOTE: When not using Wi-Fi nor Bluetooth you can pin the guiTask to core 0 */
    thread_entry_args_t *th_args = (thread_entry_args_t *) malloc(sizeof(thread_entry_args_t));
    th_args->dict_locals = mp_locals_get();
    th_args->dict_globals = mp_globals_get();

    // set the stack size to use
    th_args->stack_size = 4096*2;

    // set the function for thread entry
    th_args->fun = guiTask;
    mp_thread_create(thread_entry, th_args, &th_args->stack_size);     
}

/* Creates a semaphore to handle concurrent call to lvgl stuff
 * If you wish to call *any* lvgl function from other threads/tasks
 * you should lock on the very same semaphore! */
SemaphoreHandle_t xGuiSemaphore = NULL;
lv_disp_drv_t disp_drv;

static void wait_call(void)
{
    disp_drv.draw_buf->flushing = false;
}

void enable_lcd_backlight(bool backlight)
{
    #if defined BOARD_MATATALAB
    ext_write_digital(LCD_LEDK_PIN, !backlight);
    #elif defined BOARD_S3_DevKitC
    //st7789_enable_backlight(true);
    #elif defined BOARD_MATATALAB_SP4
    ext_write_digital(LCD_LEDK_PIN, backlight);
    #endif
    printf("%s backlight by extgpio.\n", backlight ? "Enabling" : "Disabling");
}

void get_lcd_reslution(uint16_t *p_width, uint16_t *p_height)
{
  *p_width = disp_drv.hor_res;
  *p_height = disp_drv.ver_res;
}

#define LINES_ONCE  (DISP_BUF_SIZE/LV_HOR_RES_MAX)
void lcd_driver_flush(int x1, int y1, int x2, int y2, uint8_t *rgb565_data)
{
  lv_area_t d_area = {x1, 0, x2, 0};
  lv_color_t *d_data = (lv_color_t *) rgb565_data;
  int cnt = 0;
  
  while(true) {
    int _y2 = y1+LINES_ONCE*(cnt+1)-1;
    bool flush_done = false;
    
    d_area.y1 = y1+LINES_ONCE*cnt;
    if(_y2 >= y2) {
      d_area.y2 = y2;   
      flush_done = true;
    } else {
      d_area.y2 = _y2;
    }
    disp_driver_flush(&disp_drv, &d_area, d_data);
    if(flush_done) break;
    d_data += DISP_BUF_SIZE;
    cnt++;
  }
}

static bool lvgl_inited = false;
static void guiTask(void *pvParameter) {
    (void) pvParameter;
    
    if(!lvgl_inited) {
      guiStart();
      lvgl_inited = true;
    }
    printf("LCD resolution %dx%d\n", LV_HOR_RES, LV_VER_RES);
    
    // do {
    //   lv_obj_t * scr = lv_scr_act();
    //   lv_disp_t * disp = lv_disp_get_default();
    //   if(disp->screen_cnt == 1 || !scr->parent) printf("No GUI applications\n");
    //   printf("disp->screen_cnt %d, disp->refr_timer %p, scr->parent %p\n", disp->screen_cnt, scr->parent, disp->refr_timer);
    // } while(0);
    
    // uint8_t key_num = 0;
    // set_rgb_led_pwm(0, 0, 80);

    while (true) {
        // lv_obj_t * scr = lv_scr_act();
        // lv_disp_t * disp = lv_disp_get_default();
        // //printf("disp->screen_cnt %d, disp->refr_timer %p, scr->parent %p\n", disp->screen_cnt, disp->refr_timer, scr->parent);
        // if(!disp->refr_timer) {
        //   vTaskDelay(pdMS_TO_TICKS(5000));
        // } else {
        //   /* Delay 1 tick (assumes FreeRTOS tick is 10ms */
        //   vTaskDelay(pdMS_TO_TICKS(10));
        //   uint8_t key_value = ext_read_digital(HOME_PIN);
        //   if(!key_value){
        //       key_num++;
        //       if(key_num >200){
        //       key_num = 0;
        //       printf("key pressed");
        //       int32_t start_py_value = 0;
        //       nvs_read_i32(START_PY_NAMESPACE, START_PY_KEY, &start_py_value);
        //       ESP_LOGI("app_main", "start_py_value:%d", start_py_value);
        //       set_rgb_led_pwm(80, 0, 0);
        //       start_py_value +=1;
        //       if(start_py_value > 3){
        //           start_py_value = 0;
        //       }
        //       nvs_write_i32(START_PY_NAMESPACE, START_PY_KEY, start_py_value);
        //       vTaskDelay(2000 / portTICK_PERIOD_MS);
        //       esp_restart();
        //       }

        //       //esp_restart();
        //     }

          /* Try to take the semaphore, call lvgl related function on success */
          if (pdTRUE == xSemaphoreTake(xGuiSemaphore, portMAX_DELAY)) {
              lv_task_handler();
              xSemaphoreGive(xGuiSemaphore);
         }
       }
    // }
//     printf("%s exit\n", __func__);
//     vTaskDelete(NULL);     
//     lv_disp_draw_buf_t *disp_buf = disp_drv.draw_buf;
//      /* A task should NEVER return */
//     free(disp_buf->buf1);
// #ifndef CONFIG_LV_TFT_DISPLAY_MONOCHROME
//     free(disp_buf->buf2);
// #endif
}

static void guiStart(void)
{
    xGuiSemaphore = xSemaphoreCreateMutex();
  
    lv_init();

    /* Initialize SPI or I2C bus used by the drivers */
    lvgl_driver_init();

    lv_color_t* buf1 = heap_caps_malloc(DISP_BUF_SIZE * sizeof(lv_color_t), MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);  //MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT
    assert(buf1 != NULL);

    /* Use double buffered when not working with monochrome displays */
#ifndef CONFIG_LV_TFT_DISPLAY_MONOCHROME
    lv_color_t* buf2 = heap_caps_malloc(DISP_BUF_SIZE * sizeof(lv_color_t), MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT); //MALLOC_CAP_DMA
    assert(buf2 != NULL);
#else
    static lv_color_t *buf2 = NULL;
#endif

    lv_disp_draw_buf_t disp_buf;

    uint32_t size_in_px = DISP_BUF_SIZE;

#if defined CONFIG_LV_TFT_DISPLAY_CONTROLLER_IL3820         \
    || defined CONFIG_LV_TFT_DISPLAY_CONTROLLER_JD79653A    \
    || defined CONFIG_LV_TFT_DISPLAY_CONTROLLER_UC8151D     \
    || defined CONFIG_LV_TFT_DISPLAY_CONTROLLER_SSD1306

    /* Actual size in pixels, not bytes. */
    size_in_px *= 8;
#endif

    /* Initialize the working buffer depending on the selected display.
     * NOTE: buf2 == NULL when using monochrome displays. */
    lv_disp_draw_buf_init(&disp_buf, buf1, buf2, size_in_px);

    lv_disp_drv_init(&disp_drv);

    /*Set the resolution of the display*/
    disp_drv.hor_res = LV_HOR_RES_MAX;
    disp_drv.ver_res = LV_VER_RES_MAX;
    disp_drv.dpi = 100;

    disp_drv.flush_cb = disp_driver_flush;
    disp_drv.wait_cb = wait_call;

    /* When using a monochrome display we need to register the callbacks:
     * - rounder_cb
     * - set_px_cb */
#ifdef CONFIG_LV_TFT_DISPLAY_MONOCHROME
    disp_drv.rounder_cb = disp_driver_rounder;
    disp_drv.set_px_cb = disp_driver_set_px;
#endif

    disp_drv.draw_buf = &disp_buf;
    lv_disp_drv_register(&disp_drv);

    /* Register an input device when enabled on the menuconfig */
#if CONFIG_LV_TOUCH_CONTROLLER != TOUCH_CONTROLLER_NONE
    lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.read_cb = touch_driver_read;
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    lv_indev_drv_register(&indev_drv);
#endif
    
    /* Create and start a periodic timer interrupt to call lv_tick_inc */
    const esp_timer_create_args_t periodic_timer_args = {
        .callback = &lv_tick_task,
        .name = "periodic_gui"
    };
    esp_timer_handle_t periodic_timer;
    ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &periodic_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer, LV_TICK_PERIOD_MS * 1000));

    /* Create the demo application */
    //create_demo_application();
}

static void create_demo_application(void)
{
    st7789_enable_backlight(1);
    /* When using a monochrome display we only show "Hello World" centered on the
     * screen */
#if defined CONFIG_LV_TFT_DISPLAY_MONOCHROME || \
    defined CONFIG_LV_TFT_DISPLAY_CONTROLLER_ST7735S

    /* use a pretty small demo for monochrome displays */
    /* Get the current screen  */
    lv_obj_t * scr = lv_disp_get_scr_act(NULL);

    /*Create a Label on the currently active screen*/
    lv_obj_t * label1 =  lv_label_create(scr, NULL);

    /*Modify the Label's text*/
    lv_label_set_text(label1, "Hello\nworld");

    /* Align the Label to the center
     * NULL means align on parent (which is the screen now)
     * 0, 0 at the end means an x, y offset after alignment*/
    lv_obj_align(label1, NULL, LV_ALIGN_CENTER, 0, 0);
#else
    /* Otherwise we show the selected demo */

    #if defined CONFIG_LV_USE_DEMO_WIDGETS
        lv_demo_widgets();
    #elif defined CONFIG_LV_USE_DEMO_KEYPAD_AND_ENCODER
        lv_demo_keypad_encoder();
    #elif defined CONFIG_LV_USE_DEMO_BENCHMARK
        lv_demo_benchmark();
    #elif defined CONFIG_LV_USE_DEMO_STRESS
        lv_demo_stress();
    #elif defined CONFIG_LV_USE_DEMO_MUSIC
        lv_demo_music();
    #else
        #error "No demo application selected."
    #endif
#endif
}

static void lv_tick_task(void *arg) {
    (void) arg;

    lv_tick_inc(LV_TICK_PERIOD_MS);
}

int xt_clock_freq(void)
{
}

