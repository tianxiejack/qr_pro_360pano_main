
#ifndef _GYRO_STB_PROTOCOL_H_
#define _GYRO_STB_PROTOCOL_H_

#include "gyro.h"

#define PI					3.1415926
#define MAX_GYRO_LINE		3200
#define	GYRO_NUM_PER_FRAME	72

enum gstb_type
{
	EDATA_ORIGINAL  = 0x00,
	EDATA_QUARTER	= 0x01
};

typedef struct append_data
{
	unsigned int chnId;	
	unsigned int transCou;
	unsigned int datType;	
	unsigned int packet_num;
	unsigned int datLen;
	unsigned int timestamp; 
	unsigned int focus;
	unsigned int ptz_pan;
	int ptz_til;
	int zero_data_x;//¡„∆ØX
	int zero_data_y;//¡„∆ØY
	int zero_data_z;//¡„∆ØZ
	int td_data;//td≤Œ ˝
}GSTB_APPEND;

typedef struct _GYROSTB_PACKET
{
	GSTB_APPEND head;
	unsigned char data[MAX_GYRO_LINE];
}GSTB_PACKET;

typedef struct _gyro_angle_packet
{
	unsigned char sync;			//0xA8
	unsigned char x_gyro[3];
	unsigned char y_gyro[3];
	unsigned char z_gyro[3];
	unsigned char status;
	unsigned char x_temp[2];
	unsigned char y_temp[2];
	unsigned char z_temp[2];
	unsigned char counter;
	unsigned char latency[2];
	unsigned char check;
	unsigned char CR;			//0x0D
	unsigned char LF;			//0x0A
}GYRO_ANGLE_PACKET;				//size = 23

typedef struct _gyro_angle_frame
{
	GYRO_ANGLE_PACKET   data[GYRO_NUM_PER_FRAME];
}GYRO_ANGLE_FRAME;

typedef struct _gyro_quater_packet
{
#if 0
	float  gyroX;
	float  gyroY;
	float  gyroZ;
	float  quat[4];
#else
	double  gyroX;
	double  gyroY;
	double  gyroZ;
	double  quat[4];
#endif
}GYRO_QUATER_PACKET;			//size = 23

typedef struct _gyro_quater_data_t
{
	GYRO_QUATER_PACKET data[GYRO_NUM_PER_FRAME];
}GYRO_QUATER_FRAME;

int Gyro_packet_angle_insert( GYRO_HNDL gyroHndl, unsigned char* pBuf, int packet_num);
int Gyro_packet_quater_insert(GYRO_HNDL gyroHndl, unsigned char* pBuf, int packet_num);
int Gyro_packet_insert(GYRO_HNDL gyroHndl, unsigned char* pBuf);

#endif