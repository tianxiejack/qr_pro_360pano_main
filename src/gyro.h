
#ifndef _GYRO_H_
#define _GYRO_H_

#include "gyro_calibrate.h"

#define MAX_ELEM_NUM 20000

/*
# Fg, offset, rot_x, rot_y, rot_z, gbias_x, gbias_y, gbias_z, gyro_ts, video_ts
1469.958860, 2.006640, 1.200442, -1.195122, 1.190261, 0.000367, -0.000118, 0.000515, 238.459000, 240.428000
*/
typedef struct _gyro_prms_
{
	double	gyro_rate;	//1469.958860/30fps ->1225/25fps 
	double	gbias_x;	//0.000367
	double	gbias_y;	//-0.000118,
	double	gbias_z;	//0.000515
}GYRO_PRMS ;


typedef struct 
{
	short 	gyro_ax;	
	short 	gyro_ay;	
	short 	gyro_az;
#if 0
	int  	gyro_gx;	
	int  	gyro_gy;	
	int  	gyro_gz;
	
#else
	short 	gyro_gx;	
	short 	gyro_gy;	
	short 	gyro_gz;

#endif
	short 	gyro_mx;	
	short 	gyro_my;	
	short 	gyro_mz;
	unsigned char preset;
	unsigned int   gyro_timestamp;
	unsigned int   gyro_pretimestamp;
	
}GYRO_NINEAXIS ;


typedef struct 
{
	GYRO_NINEAXIS gyrof[50];
	unsigned int packetnum;

}GYRO_FRAME;

typedef struct _gyro_data_t
{
	double  timestamp;
	double	gyroX;
	double  	gyroY;
	double	gyroZ;

	double oddeven;
	double quat[4];
}GYRO_DATA_T;

typedef struct _gyro_data_obj
{
	int datType;
	int nElem_indx;
	int nElem_max;

	double quat_all[4];
	double timestampe;
	double base_time;
	double refer_time;

	double delta1;
	double delta2;

	GYRO_PRMS	*pPrms;	
	GYRO_DATA_T ReferData;
	GYRO_DATA_T	*pGyroData;

}GYRO_OBJ, *LPGYRO_OBJ,*GYRO_HNDL;

int Gyro_create(GYRO_HNDL handle, GYRO_PRMS *pPrms, int nElem_max);

void Gyro_destroy(GYRO_HNDL gyroHndl);

void Gyro_parms_set(GYRO_PRMS* gyroPrms_in, GYRO_PRMS* gyroPrms_out);

void Gyro_time_interval_set(GYRO_HNDL gyroHndl, int frame_rate,int video_line);

void Gyro_calibPrms_set(CALIB_PRMS* calib_prm_in, CALIB_PRMS* calib_prm_out);

int Gyro_insert(GYRO_HNDL gyroHndl, unsigned char* pBuf);

int Gyro_bias_zero_dift(GYRO_HNDL gyroHndl,int num);

int Gyro_Quat_Reset(GYRO_HNDL gyroHndl);

#endif
