#ifndef __NS2016_H
/*
* Copyright © 2020 Matatalab Yanminge

* Permission is hereby granted, free of charge, to any person obtaining a copy of this 
* software and associated documentation files (the “Software”), to deal in the Software 
* without restriction, including without limitation the rights to use, copy, modify, merge, 
* publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons 
* to whom the Software is furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all copies or 
* substantial portions of the Software.
* 
* THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
* INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR 
* PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE 
* FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, 
* ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE 
* SOFTWARE.
*/

#define __NS2016_H

#include <lvgl/src/lv_hal/lv_hal.h>

#ifdef __cplusplus
extern "C" {
#endif

#define NS2016_I2C_SLAVE_ADDR   0x48
#define NS2016_MSB_MASK         0xFF
#define NS2016_LSB_MASK         0xF0

#define NS2016_READ_X           0xC0
#define NS2016_READ_Y           0xD0
#define NS2016_READ_Z1          0xE0
#define NS2016_READ_Z2          0xF0

typedef struct {
    bool inited;
} ns2016_status_t;

/**
  * @brief  Initialize for NS2016 communication via I2C
  * @param  dev_addr: Device address on communication Bus (I2C slave address of NS2016).
  * @retval None
  */
void ns2016_init(uint16_t dev_addr);

/**
  * @brief  Get the touch screen X and Y positions values. Ignores multi touch
  * @param  drv:
  * @param  data: Store data here
  * @retval Always false
  */
bool ns2016_read(lv_indev_drv_t *drv, lv_indev_data_t *data);

#ifdef __cplusplus
}
#endif
#endif /* __NS2016_H */
