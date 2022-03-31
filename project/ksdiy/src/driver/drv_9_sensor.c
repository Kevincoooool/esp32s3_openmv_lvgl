//9轴数据融合
#include "drv_qmi8658.h"
#include "drv_qmc6310.h"
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include "drv_9_sensor.h"

#define SIX_ENABLE      0

volatile float q0 = 1.0f;
volatile float  q1, q2, q3; 
void IMUupdateq(float gx, float gy, float gz, float ax, float ay, float az);
void MadgwickAHRSupdateIMU(float gx, float gy, float gz, float ax, float ay, float az);

float	mag_data[3];

void test_sensor_init(void)
{
    //Qmi8658_init();
    //qmc6310_init();

    q0 = 1.0f;  
  	q1 = 0.0f;
  	q2 = 0.0f;
  	q3 = 0.0f;
}
#define Kp 2.00f//4.50f   // proportional gain governs rate of convergence to accelerometer/magnetometer
#define Ki 0.8f//1.0f    // integral gain governs rate of convergence of gyroscope biases

/**
  * @brief  Updata attitude and heading 
  * @param  ax: accelerometer X
  * @param  ay: accelerometer Y
  * @param  az: accelerometer Z
  * @param  gx: gyroscopes X
  * @param  gy: gyroscopes Y
  * @param  gz: gyroscopes Z
  * @param  mx: magnetometer X
  * @param  my: magnetometer Y
  * @param  mz: magnetometer Z
  * @retval None
  */
void IMU_AHRSupdate(float gx, float gy, float gz, float ax, float ay, float az, float mx, float my, float mz) 
{

    static float now_time, last_time;

	float norm;
	float hx, hy, hz, bx, bz;
	float vx, vy, vz, wx, wy, wz;
	float exInt = 0.0, eyInt = 0.0, ezInt = 0.0;
	float ex, ey, ez, halfT;// = 0.202f;    

    now_time = mp_hal_ticks_ms();
    halfT =  ( now_time - last_time ) / 2000;
    last_time = now_time;

	float q0q0 = q0 * q0;
	float q0q1 = q0 * q1;
	float q0q2 = q0 * q2;
	float q0q3 = q0 * q3;
	float q1q1 = q1 * q1;
	float q1q2 = q1 * q2;
	float q1q3 = q1 * q3;
	float q2q2 = q2 * q2;   
	float q2q3 = q2 * q3;
	float q3q3 = q3 * q3;          
	norm = sqrt(ax * ax + ay * ay + az * az);
	ax = ax / norm;
	ay = ay / norm;
	az = az / norm;

	norm = sqrt(mx * mx + my * my + mz * mz);
	mx = mx / norm;
	my = my / norm;
	mz = mz / norm;

	// compute reference direction of flux
	hx = 2 * mx * (0.5f - q2q2 - q3q3) + 2 * my * (q1q2 - q0q3) + 2 * mz * (q1q3 + q0q2);
	hy = 2 * mx * (q1q2 + q0q3) + 2 * my * (0.5f - q1q1 - q3q3) + 2 * mz * (q2q3 - q0q1);
	hz = 2 * mx * (q1q3 - q0q2) + 2 * my * (q2q3 + q0q1) + 2 * mz * (0.5f - q1q1 - q2q2);
	bx = sqrt((hx * hx) + (hy * hy));
	bz = hz;     

	// estimated direction of gravity and flux (v and w)
	vx = 2 * (q1q3 - q0q2);
	vy = 2 * (q0q1 + q2q3);
	vz = q0q0 - q1q1 - q2q2 + q3q3;//1-2(q1q1-q2q2)
	wx = 2 * bx * (0.5 - q2q2 - q3q3) + 2 * bz * (q1q3 - q0q2);
	wy = 2 * bx * (q1q2 - q0q3) + 2 * bz * (q0q1 + q2q3);
	wz = 2 * bx * (q0q2 + q1q3) + 2 * bz * (0.5 - q1q1 - q2q2);  

	// error is sum of cross product between reference direction of fields and direction measured by sensors
	ex = (ay * vz - az * vy) + (my * wz - mz * wy);
	ey = (az * vx - ax * vz) + (mz * wx - mx * wz);
	ez = (ax * vy - ay * vx) + (mx * wy - my * wx);

	if(ex != 0.0f && ey != 0.0f && ez != 0.0f)
	{
		exInt = exInt + ex * Ki * halfT;
		eyInt = eyInt + ey * Ki * halfT;	
		ezInt = ezInt + ez * Ki * halfT;

		gx = gx + Kp * ex + exInt;
		gy = gy + Kp * ey + eyInt;
		gz = gz + Kp * ez + ezInt;
	}

	q0 = q0 + (-q1 * gx - q2 * gy - q3 * gz) * halfT;
	q1 = q1 + (q0 * gx + q2 * gz - q3 * gy) * halfT;
	q2 = q2 + (q0 * gy - q1 * gz + q3 * gx) * halfT;
	q3 = q3 + (q0 * gz + q1 * gy - q2 * gx) * halfT;  

	norm = sqrt(q0 * q0 + q1 * q1 + q2 * q2 + q3 * q3);
	q0 = q0 / norm;
	q1 = q1 / norm;
	q2 = q2 / norm;
	q3 = q3 / norm;
}

void IMU_GetQuater(void)
{
	float MotionVal[9];

	get_acc_gyro_angle();
    qmc6310_read_mag_xyz(mag_data);

    MotionVal[0] = gyro_x;
    MotionVal[1] = gyro_y;
    MotionVal[2] = gyro_z;
    MotionVal[3] = acc_x;
    MotionVal[4] = acc_y;
    MotionVal[5] = acc_z;
    MotionVal[6] = mag_data[0];
    MotionVal[7] = mag_data[1];
    MotionVal[8] = mag_data[2];

#if SIX_ENABLE

    IMUupdateq((float)MotionVal[0] * 0.0174  , (float)MotionVal[1] * 0.0174, (float)MotionVal[2] * 0.0174,
   	(float)MotionVal[3], (float)MotionVal[4], (float)MotionVal[5]);
    
#else

 	IMU_AHRSupdate((float)MotionVal[0] * 0.0174  , (float)MotionVal[1] * 0.0174, (float)MotionVal[2] * 0.0174,
   	(float)MotionVal[3], (float)MotionVal[4], (float)MotionVal[5], (float)MotionVal[6], (float)MotionVal[7], MotionVal[8]);

#endif
}

void IMU_GetYawPitchRoll(float *Angles) 
{
    IMU_GetQuater(); 

	const double epsilon = 0.0009765625f;//0.00048828125//0.0009765625f
    const double threshold = 0.5f - epsilon;
    double threshold_state = q0*q2 - q1*q3;

    if (threshold_state < -threshold || threshold_state > threshold) // 奇异姿态,俯仰角为±90°
    {
        int sign;// = Sign(threshold_state);
		if( threshold_state > 0 )
		{
			sign = 1;
		}
		else if( threshold_state < 0 )
		{
			sign = -1;
		}
		if( threshold_state == 0 )
		{
			sign = 0;
		}
        Angles[0] = -2 * sign * (double)atan2(q1, q0)*57.3; // yaw

        Angles[1] = sign * 90; // pitch

        Angles[2] = 0; // roll

    }
    else
	{

		float t11,t12,t13,t21,t22,t23,t31,t32,t33;
		t11 =  q0 * q0 + q1 * q1-q2 * q2-q3 * q3;
		t12 = 2.0 *( q1 * q2 + q0 * q3 );
		t13 = 2.0 *( q1 * q3 - q0 * q2 );
		t21 = 2.0 *( q1 * q2- q0 * q3 );
		t22 =  q0 * q0 - q1 * q1 + q2 * q2- q3 * q3;
		t23 = 2.0 *( q2 * q3 + q0 * q1 );
		t31 = 2.0 *( q1 * q3 + q0 * q2 );
		t32 = 2.0 *( q2 * q3 - q0 * q1);
		t33 =  q0 * q0- q1 * q1 - q2 * q2 + q3 * q3;


		Angles[2] = atan2( t23 , t33 ) * 57.3;

        Angles[1] = - asin( t13 ) * 57.3;

        Angles[0] = atan2( t12 , t11 ) * 57.3;

	//Angles[1] = asin(-2 * q1 * q3 + 2 * q0* q2)* 57.3; // pitch
	//Angles[2] = atan2(2 * q2 * q3 + 2 * q0 * q1, -2 * q1 * q1 - 2 * q2* q2 + 1)* 57.3; // roll
	//Angles[0] = atan2(-2 * q1 * q2 - 2 * q0 * q3, 2 * q2 * q2 + 2 * q3 * q3 - 1) * 57.3;
	}
	//printf("threshold_state = %f\n",threshold_state);

}

void print_poll_pithc_yaw(void)
{
    float Angles[3];
    IMU_GetYawPitchRoll(Angles);
    
    printf("roll = %f\n",Angles[2]);
    printf("pitch = %f\n",Angles[1]);
    printf("yaw = %f\n",Angles[0]);
}


#if SIX_ENABLE

void IMUupdateq(float gx, float gy, float gz, float ax, float ay, float az)

{
	float exInt = 0, eyInt = 0, ezInt = 0;    // scaled integral error

	static float now_time, last_time;

	float Yaw, pitch, roll;

	double halfT;
	
	float t11, t12, t13, t21, t22, t23, t31, t32, t33;

	const double epsilon = 0.0009765625f;

    const double threshold = 0.5f - epsilon;

    float norm;

    //  float hx, hy, hz, bx, bz;

    float vx, vy, vz;// wx, wy, wz;

    float ex, ey, ez;



    // 先把这些用得到的值算好

    float q0q0 = q0*q0;

    float q0q1 = q0*q1;

    float q0q2 = q0*q2;

    //  float q0q3 = q0*q3;

    float q1q1 = q1*q1;

    //  float q1q2 = q1*q2;

    float q1q3 = q1*q3;

    float q2q2 = q2*q2;

    float q2q3 = q2*q3;

    float q3q3 = q3*q3;


    now_time = mp_hal_ticks_ms();

    halfT = (now_time - last_time)/2000;
	
    last_time = now_time;

    if(ax*ay*az==0)

        return;

    norm = sqrt(ax*ax + ay*ay + az*az);       //acc数据归一化

    ax = ax /norm;

    ay = ay / norm;

    az = az / norm;

    // estimated direction of gravity and flux (v and w)估计重力方向和流量/变迁

    vx = 2*(q1q3 - q0q2);                                             //四元素中xyz的表示

    vy = 2*(q0q1 + q2q3);

    vz = q0q0 - q1q1 - q2q2 + q3q3 ;

    // error is sum of cross product between reference direction of fields and direction measured by sensors

    ex = (ay*vz - az*vy) ;                                             //向量外积在相减得到差分就是误差

    ey = (az*vx - ax*vz) ;

    ez = (ax*vy - ay*vx) ;


    exInt = exInt + ex * Ki;         //对误差进行积分

    eyInt = eyInt + ey * Ki;

    ezInt = ezInt + ez * Ki;


    // adjusted gyroscope measurements

    gx = gx + Kp*ex + exInt;         //将误差PI后补偿到陀螺仪，即补偿零点漂移

    gy = gy + Kp*ey + eyInt;

    gz = gz + Kp*ez + ezInt;         //这里的gz由于没有观测者进行矫正会产生漂移，表现出来的就是积分自增或自减


    // integrate quaternion rate and normalise   //四元素的微分方程

    q0 = q0 + (-q1*gx - q2*gy - q3*gz)*halfT;

    q1 = q1 + (q0*gx + q2*gz - q3*gy)*halfT;

    q2 = q2 + (q0*gy - q1*gz + q3*gx)*halfT;

    q3 = q3 + (q0*gz + q1*gy - q2*gx)*halfT;


    // normalise quaternion

    norm = sqrt(q0*q0 + q1*q1 + q2*q2 + q3*q3);

    q0 = q0 / norm;

    q1 = q1 / norm;

    q2 = q2 / norm;

    q3 = q3 / norm;

#if 0

	t11=q0 * q0 + q1 * q1-q2 * q2-q3 * q3;

	t12=2.0*( q1* q2+ q0* q3);
	
	t13=2.0*( q1* q3- q0* q2);
	
	t21=2.0*( q1* q2- q0* q3);
	
	t22= q0* q0- q1* q1+ q2* q2- q3* q3;
	
	t23=2.0*( q2* q3+ q0* q1);
	
	t31=2.0*( q1* q3+ q0* q2);
	
	t32=2.0*( q2* q3- q0* q1);
	
	t33= q0* q0- q1* q1- q2* q2+ q3* q3;

	//求出欧拉角

    double threshold_state = q0*q2 - q1*q3;

    if (threshold_state < -threshold || threshold_state > threshold) // 奇异姿态,俯仰角为±90°
    {
        int sign;// = Sign(threshold_state);
		if(threshold_state > 0)
		{
			sign = 1;
		}
		else if(threshold_state < 0)
		{
			sign = -1;
		}
		if(threshold_state == 0)
		{
			sign = 0;
		}
        Yaw = -2 * sign * (double)atan2(q1, q0)*57.3; // yaw

        pitch = sign * 90; // pitch

        roll = 0; // roll

    }
    else
    {
        //roll = atan2(2 * (q2*q3 + q3*q1), q0*q0 - q1*q1 - q2*q2 + q3*q3)*57.3;

        //pitch = asin(-2 * (q1*q3 - q0*q2))*57.3;

        //Yaw = atan2(2 * (q1*q2 + q0*q3), q0*q0 + q1*q1 - q2*q2 - q3*q3)*57.3;

        roll = atan2(t23,t33)*57.3;

        pitch = -asin(t13)*57.3;

        Yaw = atan2(t12,t11)*57.3;
	}
		/*
    Yaw = atan2(2 * q1 * q2 + 2 * q0 * q3, -2 * q2*q2 - 2 * q3* q3 + 1)* 57.3; // yaw

    pitch  = asin(-2 * q1 * q3 + 2 * q0* q2)* 57.3; // pitch

    roll = atan2(2 * q2 * q3 + 2 * q0 * q1, -2 * q1 * q1 - 2 * q2* q2 + 1)* 57.3; // roll
	*/
    printf("roll = %f\n",roll);

    printf("pitch = %f\n",pitch);

    printf("yaw = %f\n",Yaw);
#endif
}
#endif

