
#ifndef __DRV_I2C_H__
#define __DRV_I2C_H__

#include "esp_err.h"
#include "driver/i2c.h"


#define _I2C_NUMBER(num)  I2C_NUM_##num
#define I2C_NUMBER(num)  _I2C_NUMBER(num)
#define DATA_LENGTH      512

#define I2C1_MASTER_SCL_IO  18//9               /*!< gpio number for I2C1 master clock */
#define I2C1_MASTER_SDA_IO  17//8               /*!< gpio number for I2C1 master data  */
#define I2C1_MASTER_NUM     I2C_NUMBER(1)    /*!< I2C1 port number for master dev */
#define I2C1_MASTER_FREQ_HZ  200000          /*!< I2C master clock frequency */
#define I2C_MASTER_TX_BUF_DISABLE 0          /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE 0  

#define I2C0_MASTER_SCL_IO  18                /*!< gpio number for I2C0 master clock */
#define I2C0_MASTER_SDA_IO  17                /*!< gpio number for I2C0 master data  */
#define I2C0_MASTER_NUM     I2C_NUMBER(0)    /*!< I2C0 port number for master dev */
#define I2C0_MASTER_FREQ_HZ  200000          /*!< I2C master clock frequency */

#define WRITE_BIT I2C_MASTER_WRITE           /*!< I2C master write */
#define READ_BIT I2C_MASTER_READ             /*!< I2C master read */
#define ACK_CHECK_EN 0x1                     /*!< I2C master will check ack from slave*/
#define ACK_CHECK_DIS 0x0                    /*!< I2C master will not check ack from slave */
#define ACK_VAL 0x0                          /*!< I2C ack value */
#define NACK_VAL 0x1                         /*!< I2C nack value */

esp_err_t i2c_master_init(uint8_t i2c_num);
bool is_i2c0_initialized(void);
bool is_i2c1_initialized(void);
esp_err_t i2c_master_read_reg(i2c_port_t i2c_num, uint8_t slaver_addr, uint8_t reg_addr, uint8_t *data);
esp_err_t i2c_master_read_mem(i2c_port_t i2c_num, uint8_t slaver_addr, uint8_t reg_addr, uint8_t *buffer, uint16_t size);
esp_err_t i2c_master_write_reg(i2c_port_t i2c_num, uint8_t slaver_addr, uint8_t reg_addr, uint8_t data);
esp_err_t i2c_master_write_mem(i2c_port_t i2c_num, uint8_t slaver_addr, uint8_t reg_addr, uint8_t *buffer, uint16_t size);




#endif
