/*
 * MPU9250.hpp
 *
 *  Created on: 2018年9月25日
 *      Author: wj
 */

#ifndef MPU9250_HPP_
#define MPU9250_HPP_
#include "config.h"

typedef struct 
{
	short 	gyro_axi;	
	short 	gyro_ayi;	
	short 	gyro_azi;
#if 0
	int 	gyro_gxi;	
	int 	gyro_gyi;	
	int 	gyro_gzi;
#else
	short 	gyro_gxi;	
	short 	gyro_gyi;	
	short 	gyro_gzi;

#endif
	short 	gyro_mxi;	
	short 	gyro_myi;	
	short 	gyro_mzi;

	double 	gyro_ax;	
	double 	gyro_ay;	
	double 	gyro_az;

	double 	gyro_gx;	
	double 	gyro_gy;	
	double 	gyro_gz;

	double 	gyro_mx;	
	double 	gyro_my;	
	double 	gyro_mz;

	double  roll;
	double pitch;
	double yaw;
	
	unsigned int   gyro_timestamp;
	unsigned int   gyro_pretimestamp;
	unsigned char preset;
	
}GYRO_MPU ;
//unsigned char  calibrate(short *gx, short *gy, short *gz);
unsigned char  calibrate(void *gx1, void *gy1, void *gz1);
void AHRSupdate(GYRO_MPU*mpu);
void CalibrateToZero(GYRO_MPU*mpu);
void GetMpugyro(GYRO_MPU*mpu);
void setgyyawbase(double angle);
void gyrokalmanfilterinit();
unsigned char MPU_Get_Gyroscopeint( int *gx, int *gy, int *gz);

unsigned char MPU_Get_Gyro( void *igx1, void  *igy1, void *igz1,double *gx1,double *gy1,double *gz1);
unsigned char MPU_Get_Gyroint( void *igx1, void   *igy1, void  *igz1,double *gx,double *gy,double *gz);
unsigned char  calibrateint(void *gx1, void *gy1, void *gz1);
void mpu9250reset();
#define GYRO_NOFILTER 1
#endif /* MPU9250_HPP_ */
