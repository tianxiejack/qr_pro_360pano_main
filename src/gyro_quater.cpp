#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#include "gyro.h"
#include "gyro_math.h"
#include "gyro_quater.h"

int sample_at_time(double t, double rate, double *tau)
{
	double s = t * rate/1000 - 0.5 ;
	int n = (int)(floor(s));
	*tau = s - n;
	return n;
}

int frames_at_time(double t, double rate)
{
	double s = t * rate/1000 + 0.5 ;
	int n = (int)(floor(s));
	return n;
}

void Gyro_gen_quateration(GYRO_HNDL gyroHndl, GYRO_DATA_T* pGyroElem)
{
	int		i = 0;
	double	deltaT = 0.0; //mill second unit
	double  dt_half = 0.0;
	double  quat_last[4];
	double  quat_new[4];
	double  qnorm;

	GYRO_PRMS   *lpParams;
	GYRO_DATA_T	*lpGyroData;
	GYRO_DATA_T RectifyGyroData;

	assert(gyroHndl!=NULL);
	assert(pGyroElem!=NULL);

	deltaT = 1000.0 / gyroHndl->pPrms->gyro_rate; //mill second unit
	dt_half = deltaT / 2;

	lpParams   = gyroHndl->pPrms;
	lpGyroData = (GYRO_DATA_T*)pGyroElem;

	RectifyGyroData.gyroX = (lpGyroData->gyroX - lpParams->gbias_x);//
	RectifyGyroData.gyroY = (lpGyroData->gyroY - lpParams->gbias_y);//
	RectifyGyroData.gyroZ = (lpGyroData->gyroZ - lpParams->gbias_z);//

	memcpy(quat_last,gyroHndl->quat_all,sizeof(quat_last));

	quat_new[0] = quat_last[0] + dt_half*0.001*(-RectifyGyroData.gyroX * quat_last[1] - RectifyGyroData.gyroY * quat_last[2] - RectifyGyroData.gyroZ * quat_last[3]);
	quat_new[1] = quat_last[1] + dt_half*0.001*( RectifyGyroData.gyroX * quat_last[0] + RectifyGyroData.gyroZ * quat_last[2] - RectifyGyroData.gyroY * quat_last[3]);
	quat_new[2] = quat_last[2] + dt_half*0.001*( RectifyGyroData.gyroY * quat_last[0] - RectifyGyroData.gyroZ * quat_last[1] + RectifyGyroData.gyroX * quat_last[3]);
	quat_new[3] = quat_last[3] + dt_half*0.001*( RectifyGyroData.gyroZ * quat_last[0] + RectifyGyroData.gyroY * quat_last[1] - RectifyGyroData.gyroX * quat_last[2]);

	// Normalize
	qnorm = sqrt(quat_new[0]*quat_new[0] + quat_new[1]*quat_new[1] + quat_new[2]*quat_new[2] + quat_new[3]*quat_new[3]);
	for (i = 0; i < 4; i++)
	{
		quat_new[i] /=qnorm;
	}

	lpGyroData->quat[0] = quat_new[0];
	lpGyroData->quat[1] = quat_new[1];
	lpGyroData->quat[2] = quat_new[2];
	lpGyroData->quat[3] = quat_new[3];

	// New prev values
	memcpy(gyroHndl->quat_all,quat_new,sizeof(quat_new));
}

void Gyro_set_ref_quateration(GYRO_HNDL gyroHndl, double read_out_time, double Fg, double frame_ts, double td_time/*ms*/)
{

	GYRO_DATA_T	*lpGyroData = gyroHndl->pGyroData;
	GYRO_DATA_T *pRefQuat   = &gyroHndl->ReferData;

	double refptimes = frame_ts + read_out_time * 0.5 - td_time;

	double ref_tau;

	int refIdx = 0;

	int frameIdx;

	refptimes -= gyroHndl->base_time;

	gyroHndl->refer_time = frame_ts - td_time;

#if 1
	refIdx = sample_at_time(refptimes, Fg, &ref_tau);/*前提是陀螺数据没有丢失*/

/********************added 20170102*******************
	refptimes = frame_ts - td_time - gyroHndl->base_time;
	frameIdx = frames_at_time(refptimes, 25);
	refIdx = frameIdx*49+49/2;
/********************************************************/
	assert(gyroHndl!=NULL);

	memcpy(pRefQuat, &gyroHndl->pGyroData[refIdx],sizeof(GYRO_DATA_T));

	//slerp(lpGyroData[refIdx].quat, lpGyroData[refIdx+1].quat, ref_tau, pRefQuat->quat);
#else
	refIdx = sample_at_time(refptimes, Fg, &ref_tau);

	assert(gyroHndl!=NULL);

	memcpy(pRefQuat, &gyroHndl->pGyroData[refIdx],sizeof(GYRO_DATA_T));

	slerp(lpGyroData[refIdx].quat, lpGyroData[refIdx+1].quat, ref_tau, pRefQuat->quat);
#endif
	printf("set QuatId %d\r\n", refIdx);
	printf("timeStamp:%f  \r\n",pRefQuat->timestamp);
	printf("gyro[X,Y,Z]:(%f,%f,%f)  \r\n",pRefQuat->gyroX,pRefQuat->gyroY,pRefQuat->gyroZ);
	printf("Quat[0~3]:(%f,%f,%f,%f) \r\n",pRefQuat->quat[0],pRefQuat->quat[1],pRefQuat->quat[2],pRefQuat->quat[3]);
}

void Gyro_quat_to_rotation_matrix( GYRO_DATA_T *lpGyroData, double Fg, double basetime ,double curptimes,double *curRot)
{
	double cur_tau;
//	double cur_quat[4];

	int curIdx = 0;

	curptimes -= basetime;
	curIdx = sample_at_time(curptimes, Fg, &cur_tau);

#if 1
	quat_to_rotation_matrix(lpGyroData[curIdx].quat, curRot);
#else
	slerp(lpGyroData[curIdx].quat, lpGyroData[curIdx+1].quat, cur_tau, cur_quat);

	quat_to_rotation_matrix(cur_quat, curRot);
#endif
}

void Gyro_quat_to_rotation_matrix_new( GYRO_DATA_T *lpGyroData, double Fg, double basetime ,double curptimes, double curstarttime, double *curRot)
{
	double cur_tau;
	//	double cur_quat[4];

	int curIdx = 0;
	int interIdx = 0;
	int frameIdx;

	interIdx = sample_at_time((curptimes-curstarttime+0.5), Fg, &cur_tau);

	curstarttime -= basetime;
//	curIdx = sample_at_time(curptimes, Fg, &cur_tau);
/********************added 20170102*******************/
	frameIdx = frames_at_time(curstarttime, 25);
	curIdx = frameIdx*49;
	curIdx += interIdx;
/********************************************************/

#if 1
	quat_to_rotation_matrix(lpGyroData[curIdx].quat, curRot);
#else
	slerp(lpGyroData[curIdx].quat, lpGyroData[curIdx+1].quat, cur_tau, cur_quat);

	quat_to_rotation_matrix(cur_quat, curRot);
#endif
}