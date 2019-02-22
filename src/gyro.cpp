#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
//#include <io.h>
#include <fcntl.h>

#include "gyro.h"
#include "gyro_protocol.h"

#define  GYRO_REMAIN_ELEMS 2

int Gyro_create(GYRO_HNDL handle, GYRO_PRMS *pPrms, int nElem_max)
{
	int nbytes = sizeof(GYRO_OBJ) + nElem_max * sizeof(GYRO_DATA_T);

	assert(handle != NULL);

	memset(handle, 0, nbytes);

	handle->nElem_indx	= 0;
	handle->nElem_max	= nElem_max;

	handle->timestampe	= 0;
	handle->base_time   = 0;

	handle->pGyroData	= (GYRO_DATA_T*)(&handle[1]);

	handle->pPrms		= pPrms;

	handle->quat_all[0]	= 1.0;
	handle->quat_all[1]	= 0.0;
	handle->quat_all[2]	= 0.0;
	handle->quat_all[3]	= 0.0;

	return 0;
}

void Gyro_destroy(GYRO_HNDL gyroHndl)
{
}

void Gyro_parms_set(GYRO_PRMS* gyroPrms_in, GYRO_PRMS* gyroPrms_out)
{
	assert(gyroPrms_in !=NULL && gyroPrms_out != NULL);

	memcpy(gyroPrms_out,gyroPrms_in,sizeof(GYRO_PRMS));
}

void Gyro_time_interval_set(GYRO_HNDL gyroHndl, int frame_rate,int video_line)
{
	//double delta1 = 23 * 1000.0/(30*1125);
	//double delta2 = 21 * 1000.0/(30*1125);
	gyroHndl->delta1 = 23 * 1000.0/(frame_rate * video_line);
	gyroHndl->delta2 = 21 * 1000.0/(frame_rate * video_line);
}

int Gyro_insert(GYRO_HNDL gyroHndl, unsigned char* pBuf)
{
	int elemCou    = 0;

	elemCou = Gyro_packet_insert(gyroHndl,pBuf);

	return elemCou;
}

int Gyro_bias_zero_dift(GYRO_HNDL gyroHndl,int num)
{
	int i = 0;

	if(num > gyroHndl->nElem_indx)
	{
		return -1;
	}
	else
	{
		double	gyroX = 0;
		double  gyroY = 0;
		double	gyroZ = 0;

		for( i = gyroHndl->nElem_indx - num; i < gyroHndl->nElem_indx ; i++)
		{
			gyroX += gyroHndl->pGyroData[i].gyroX;
			gyroY += gyroHndl->pGyroData[i].gyroY;
			gyroZ += gyroHndl->pGyroData[i].gyroZ;
		}

		gyroHndl->pPrms->gbias_x = gyroX*PI/180/num;
		gyroHndl->pPrms->gbias_y = gyroY*PI/180/num;
		gyroHndl->pPrms->gbias_z = gyroZ*PI/180/num;

	}
	return 0;
}

int Gyro_Quat_Reset(GYRO_HNDL gyroHndl)
{
	gyroHndl->nElem_indx	= 0;
	gyroHndl->timestampe	= 0;
	gyroHndl->base_time   = 0;

	gyroHndl->quat_all[0]	= 1.0;
	gyroHndl->quat_all[1]	= 0.0;
	gyroHndl->quat_all[2]	= 0.0;
	gyroHndl->quat_all[3]	= 0.0;

	

	return 0;
}

