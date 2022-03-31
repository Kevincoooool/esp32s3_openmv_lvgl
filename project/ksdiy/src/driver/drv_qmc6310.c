/**
  ******************************************************************************
  * @file    qmdata[AXIS_X]983.c
  * @author  STMicroelectronics
  * @version V1.0
  * @date    2013-xx-xx
  * @brief    qmdata[AXIS_X]983����
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:���� ָ���� ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 
/*
#include "qmc6310.h"
#include "../i2c/qst_sw_i2c.h"
#include <math.h>
*/
#include "drv_qmc6310.h"
//#include "aos/kernel.h"
//#include "ulog/ulog.h"
#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include "drv_i2c.h"
#include "esp_err.h"
#include "esp_log.h"
#define I2C_MASTER_NUM I2C_NUM_1
uint8_t chipid = 0;
uint8_t mag_addr;
float magn[3];
static unsigned char sensor_mask[4] = {
	0x80,
	0xA0,
	0xB0,
	0xC0
};

static QMC6310_map c_map;

int qmc6310_read_block(uint8_t addr, uint8_t *data, uint8_t len)
{
	int ret;
	ret = i2c_master_read_mem(I2C_MASTER_NUM, mag_addr, addr, data, len);
	if(ret == 0)
		return 1;
	else
		return 0;
}



int qmc6310_write_reg(uint8_t addr, uint8_t data)
{	
	int ret;
	ret = i2c_master_write_reg(I2C_MASTER_NUM, mag_addr, addr, data);
	if(ret == 0)
		return 1;
	else
		return 0;

}

static void qmc6310_set_layout(int layout)
{
	if(layout == 0)
	{
		c_map.sign[AXIS_X] = 1;
		c_map.sign[AXIS_Y] = 1;
		c_map.sign[AXIS_Z] = 1;
		c_map.map[AXIS_X] = AXIS_X;
		c_map.map[AXIS_Y] = AXIS_Y;
		c_map.map[AXIS_Z] = AXIS_Z;
	}
	else if(layout == 1)
	{
		c_map.sign[AXIS_X] = -1;
		c_map.sign[AXIS_Y] = 1;
		c_map.sign[AXIS_Z] = 1;
		c_map.map[AXIS_X] = AXIS_Y;
		c_map.map[AXIS_Y] = AXIS_X;
		c_map.map[AXIS_Z] = AXIS_Z;
	}
	else if(layout == 2)
	{
		c_map.sign[AXIS_X] = -1;
		c_map.sign[AXIS_Y] = -1;
		c_map.sign[AXIS_Z] = 1;
		c_map.map[AXIS_X] = AXIS_X;
		c_map.map[AXIS_Y] = AXIS_Y;
		c_map.map[AXIS_Z] = AXIS_Z;
	}
	else if(layout == 3)
	{
		c_map.sign[AXIS_X] = 1;
		c_map.sign[AXIS_Y] = -1;
		c_map.sign[AXIS_Z] = 1;
		c_map.map[AXIS_X] = AXIS_Y;
		c_map.map[AXIS_Y] = AXIS_X;
		c_map.map[AXIS_Z] = AXIS_Z;
	}	
	else if(layout == 4)
	{
		c_map.sign[AXIS_X] = -1;
		c_map.sign[AXIS_Y] = 1;
		c_map.sign[AXIS_Z] = -1;
		c_map.map[AXIS_X] = AXIS_X;
		c_map.map[AXIS_Y] = AXIS_Y;
		c_map.map[AXIS_Z] = AXIS_Z;
	}
	else if(layout == 5)
	{
		c_map.sign[AXIS_X] = 1;
		c_map.sign[AXIS_Y] = 1;
		c_map.sign[AXIS_Z] = -1;
		c_map.map[AXIS_X] = AXIS_Y;
		c_map.map[AXIS_Y] = AXIS_X;
		c_map.map[AXIS_Z] = AXIS_Z;
	}
	else if(layout == 6)
	{
		c_map.sign[AXIS_X] = 1;
		c_map.sign[AXIS_Y] = -1;
		c_map.sign[AXIS_Z] = -1;
		c_map.map[AXIS_X] = AXIS_X;
		c_map.map[AXIS_Y] = AXIS_Y;
		c_map.map[AXIS_Z] = AXIS_Z;
	}
	else if(layout == 7)
	{
		c_map.sign[AXIS_X] = -1;
		c_map.sign[AXIS_Y] = -1;
		c_map.sign[AXIS_Z] = -1;
		c_map.map[AXIS_X] = AXIS_Y;
		c_map.map[AXIS_Y] = AXIS_X;
		c_map.map[AXIS_Z] = AXIS_Z;
	}
	else		
	{
		c_map.sign[AXIS_X] = 1;
		c_map.sign[AXIS_Y] = 1;
		c_map.sign[AXIS_Z] = 1;
		c_map.map[AXIS_X] = AXIS_X;
		c_map.map[AXIS_Y] = AXIS_Y;
		c_map.map[AXIS_Z] = AXIS_Z;
	}
}
static int qmc6310_get_chipid(void)
{
	int ret = 0;
	int i;

	ESP_LOGI("SENSOR","qmc6310_get_chipid addr=0x%x\n",mag_addr);
	for(i=0; i<10; i++)
	{
		ret = qmc6310_read_block(QMC6310_CHIP_ID_REG, &chipid , 1);
		ESP_LOGI("SENSOR","qmc6310_get_chipid chipid = 0x%x\n", chipid);
		if(chipid == 0x80)
		{
			break;
		}
	}
	if(i>=10)
	{
		return 0;
	}
	return 1;
}


float X_max = -32768;
float X_min = 32767;
float Y_max = -32768;
float Y_min = 32767;
float Z_max = -32768;
float Z_min = 32767;
float X_abs,Y_abs,Z_abs; 
float X_excursion = 326.5f,Y_excursion = -15.0f,Z_excursion = -83.5f;
float X_gain = 1,Y_gain = 1.030498f,Z_gain = 0.781701f;
//float X_excursion = 337.00f,Y_excursion = -44.0f,Z_excursion = -86.5f;
//float X_gain = 1,Y_gain = 1.107558f,Z_gain =  0.789244f;
// float X_excursion = 358.5f,Y_excursion = -46.0f,Z_excursion = -136.0f;
// float X_gain = 1,Y_gain = 1.076006f,Z_gain =  0.766021f;
uint8_t Rotation_Axis;

#define COMPASS_PI 3.14159265F
uint8_t qmc6310_read_mag_xyz(float *data)
{
    int res;
    unsigned char mag_data[6];
    short hw_d[3] = {0};
    short raw_c[3];
    int t1 = 0;
    unsigned char rdy = 0;
	static	float mag_x = 8.0f,mag_y = 6.8f,mag_z = 32.0f;
    /* Check status register for data availability */
    while (!(rdy & 0x01) && (t1 < 5)) {
        rdy = QMC6310_STATUS_REG;
        res = qmc6310_read_block(QMC6310_STATUS_REG, &rdy, 1);
        t1++;
    }

    mag_data[0] = QMC6310_DATA_OUT_X_LSB_REG;

    res = qmc6310_read_block(QMC6310_DATA_OUT_X_LSB_REG, mag_data, 6);
    if (res == 0) {
        return 0;
    }


    hw_d[0] = (short)(((mag_data[1]) << 8) | mag_data[0]);
    hw_d[1] = (short)(((mag_data[3]) << 8) | mag_data[2]);
    hw_d[2] = (short)(((mag_data[5]) << 8) | mag_data[4]);


    // Unit:mG  1G = 100uT = 1000mG
    // printf("Hx=%d, Hy=%d, Hz=%d\n",hw_d[0],hw_d[1],hw_d[2]);
    raw_c[AXIS_X] = (int)(c_map.sign[AXIS_X] * hw_d[c_map.map[AXIS_X]]);
    raw_c[AXIS_Y] = (int)(c_map.sign[AXIS_Y] * hw_d[c_map.map[AXIS_Y]]);
    raw_c[AXIS_Z] = (int)(c_map.sign[AXIS_Z] * hw_d[c_map.map[AXIS_Z]]);
#if 1
	/*
    data[AXIS_X] = (float)raw_c[AXIS_X];
    data[AXIS_Y] = (float)raw_c[AXIS_Y];
    data[AXIS_Z] = (float)raw_c[AXIS_Z];
	*/
	
	
	magn[AXIS_X] = (float)raw_c[AXIS_X];
	magn[AXIS_Y] = (float)raw_c[AXIS_Y];
	magn[AXIS_Z] = (float)raw_c[AXIS_Z];
	
	//magne_calibration();//磁力计校准
	
	data[AXIS_X] = (magn[AXIS_X] + X_excursion) * X_gain;
    data[AXIS_Y] = (magn[AXIS_Y] + Y_excursion) * Y_gain;
    data[AXIS_Z] = (magn[AXIS_Z] + Z_excursion) * Z_gain;


	/*
	data[AXIS_X] = magn[AXIS_X];
    data[AXIS_Y] = magn[AXIS_Y];
    data[AXIS_Z] = magn[AXIS_Z];
	*/
    if(Rotation_Axis == 1)  //X_Axis
    {
      compass_angle = atan2( (double)data[AXIS_Y], (double)data[AXIS_Z] );
    }
    else if(Rotation_Axis == 2)  //Y_Axis
    {
      compass_angle = atan2( (double)data[AXIS_X], (double)data[AXIS_Z] );
    }
    else if(Rotation_Axis == 3)  //Z_Axis
    {
      compass_angle = atan2( (double)data[AXIS_Y], (double)data[AXIS_X] );
    }
	else
	{
		compass_angle = atan2( (double)data[AXIS_Y], (double)data[AXIS_X] );
	}

	if(compass_angle < 0)
	{
		compass_angle = (compass_angle + 2 * COMPASS_PI) * 180 / COMPASS_PI;
	}
	else
	{
		compass_angle = compass_angle * 180 / COMPASS_PI;
	}
	//printf("compass_angle = %f\n",compass_angle);
	//printf("magn[AXIS_X] = %f\n",magn[AXIS_X]);
	//printf("magn[AXIS_Y] = %f\n",magn[AXIS_Y]);
	//printf("magn[AXIS_Z] = %f\n",magn[AXIS_Z]);
#else


    data[AXIS_X] = (float)raw_c[AXIS_X] / 10.0f;
    data[AXIS_Y] = (float)raw_c[AXIS_Y] / 10.0f;
    data[AXIS_Z] = (float)raw_c[AXIS_Z] / 10.0f;
	
    printf("data[x] = %f\n",data[AXIS_X]);
    printf("data[y] = %f\n",data[AXIS_Y]);
    printf("data[z] = %f\n",data[AXIS_Z]);
    printf("\n");
#endif
	//float Angle;
	//#define QMC_PI	(3.14159265358979323846f)
	//Angle = atan2(data[AXIS_X],data[AXIS_Y])*180/QMC_PI;
	//printf("Angle = %f\n",Angle);
/*
    printf("data[0] = %f\n",hw_d[0] );
    printf("data[1] = %f\n",hw_d[1] );
    printf("data[2] = %f\n",hw_d[2] );
*/
    return 0;
}

void magne_calibration(void)
{
	float  magndate[3];
	static int i;
	static double previous_time,current_time,elapsed_time;

	for(i=0;i<999;)
	{
		current_time = mp_hal_ticks_ms();            // Current time actual time read
		elapsed_time = (current_time - previous_time);
		if(elapsed_time < 5)
		{
			continue;
		}

		previous_time = current_time;        // Previous time is stored before the actual time read
		qmc6310_read_mag_xyz(magndate);
		i++;

		if(magn[AXIS_X] < X_min)
		{
		X_min = magn[AXIS_X];
		}
		else if(magn[AXIS_X] > X_max)
		{
		X_max = magn[AXIS_X];
		}  
		if(magn[AXIS_Y] < Y_min)
		{
		Y_min = magn[AXIS_Y];
		}
		else if(magn[AXIS_Y] > Y_max)
		{
		Y_max = magn[AXIS_Y];
		}  
		if(magn[AXIS_Z] < Z_min)
		{
		Z_min = magn[AXIS_Z];
		}
		else if(magn[AXIS_Z] > Z_max)
		{
		Z_max = magn[AXIS_Z];
		}
	}
	if(i == 999)
	{
		i = 0;
		X_excursion = -( (float)X_max + (float)X_min ) / 2;
		Y_excursion = -( (float)Y_max + (float)Y_min ) / 2;
		Z_excursion = -( (float)Z_max + (float)Z_min ) / 2;
		X_gain = 1;
		Y_gain = ( (float)Y_max - (float)Y_min ) / ( (float)X_max - (float)X_min );
		Z_gain = ( (float)Z_max - (float)Z_min ) / ( (float)X_max - (float)X_min );  
		X_abs = abs(X_max-X_min);
		Y_abs = abs(Y_max-Y_min);
		Z_abs = abs(Z_max-Z_min);  

		if(X_abs<=Y_abs && X_abs<=Z_abs)
		{
			Rotation_Axis=1;  //X_Axis
		}
		else if(Y_abs<=X_abs && Y_abs<=Z_abs)
		{
			Rotation_Axis=2;  //Y_Axis
		}
		else
		{
			Rotation_Axis=3;  //Z_Axis
		}  
		
		X_max = -32768;
		X_min = 32767;
		Y_max = -32768;
		Y_min = 32767;
		Z_max = -32768;
		Z_min = 32767;
		
		printf("X_excursion = %f\n",X_excursion);
		printf("Y_excursion = %f\n",Y_excursion);
		printf("Z_excursion = %f\n",Z_excursion);
		printf("X_gain = %f\n",X_gain);
		printf("Y_gain = %f\n",Y_gain);
		printf("Z_gain = %f\n",Z_gain);

	}

}

/* Set the sensor mode */
int qmc6310_set_mode(unsigned char mode)
{
	int err = 0;
	unsigned char ctrl1_value = 0;
	
	err = qmc6310_read_block(QMC6310_CTL_REG_ONE, &ctrl1_value, 1);
	ctrl1_value = (ctrl1_value&(~0x03))|mode;
	printf("QMC6310_set_mode, 0x0A = [%02x]->[%02x] \r\n", QMC6310_CTL_REG_ONE,ctrl1_value);
	err = qmc6310_write_reg(QMC6310_CTL_REG_ONE, ctrl1_value);

	return err;
}

int qmc6310_set_output_data_rate(unsigned char rate){
	
	int err = 0;
	unsigned char ctrl1_value = 0;
	
	err = qmc6310_read_block(QMC6310_CTL_REG_ONE, &ctrl1_value, 1);
	ctrl1_value = (ctrl1_value& (~0xE8)) | (rate << 5);
	printf("QMC6310_set_output_data_rate, 0x0A = [%02x]->[%02x] \r\n", QMC6310_CTL_REG_ONE,ctrl1_value);
	err = qmc6310_write_reg(QMC6310_CTL_REG_ONE, ctrl1_value);

	return err;	
}

static int qmc6308_enable(void)
{
	int ret = 0;
	if(chipid == 0x80)
	{
		qmc6310_write_reg(0x0a, 0xc3);
		qmc6310_write_reg(0x0b, 0x00);
		qmc6310_write_reg(0x0d, 0x40);
	}
	else
	{
		qmc6310_write_reg(0x0a, 0x63);
		qmc6310_write_reg(0x0b, 0x08);
		qmc6310_write_reg(0x0d, 0x40);

	}
	return ret;
}
static int qmc6310_enable(void)
{
	int ret = 0;

	qmc6310_write_reg(0x0d, 0x40);
	qmc6310_write_reg(0x29, 0x06);
	qmc6310_write_reg(0x0a, 0x0F);//0XA9=ODR =100HZ 0XA5 = 50HZ
	qmc6310_write_reg(0x0b, 0x00); //30 GS
/*

    qmc6310_write_reg(0x0d, 0x40);
    qmc6310_write_reg(0x29, 0x06);
    qmc6310_write_reg(0x0a, 0xC3); // 0XA9=ODR =100HZ 0XA5 = 50HZ
    qmc6310_write_reg(0x0b, 0x08); // 30 GS
*/

	printf("qmc6310_enable\n");
	return 1;
}

int qmc6310_init(void)
{
	i2c_master_init(I2C_MASTER_NUM);
	if(!is_i2c1_initialized())
    {
        i2c_master_init(I2C_MASTER_NUM);
    }
	int ret = 0;
	mag_addr = QMC6310U_IIC_ADDR;
	ret = qmc6310_get_chipid();
	
	if(ret==0)
	{
		return 0;
	}
	
	qmc6310_set_layout(0);
	//qmc6310_set_layout(0);
	if(mag_addr == QMC6308_IIC_ADDR)
	{
		qmc6308_enable();
	}
	else
	{
		qmc6310_enable();
	}
	

	{
		unsigned char ctrl_value;
		qmc6310_read_block(QMC6310_CTL_REG_ONE, &ctrl_value, 1);
		printf("QMC6310  0x%x=0x%x \r\n", QMC6310_CTL_REG_ONE, ctrl_value);
		qmc6310_read_block(QMC6310_CTL_REG_TWO, &ctrl_value, 1);
		printf("QMC6310  0x%x=0x%x \r\n", QMC6310_CTL_REG_TWO, ctrl_value);
		qmc6310_read_block(0x0d, &ctrl_value, 1);
		printf("QMC6310  0x%x=0x%x \r\n", 0x0d, ctrl_value);
	}
	return 1;
}
