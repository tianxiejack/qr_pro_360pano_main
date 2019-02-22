
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
//#include <io.h>
//#include <fcntl.h >
//#include "ialg_debug.h"
#include "gyro.h"
//#include "gyro_frame.h"
#include "gyro_quater.h"
#include "gyro_protocol.h"

static double angle[3];

int Gyro_packet_angle_insert(GYRO_HNDL gyroHndl, unsigned char* pBuf, int packet_num)
{
	int i = 0;
	int elemCou    = 0;

	int gyroValueX = 0xE1234567;
	int gyroValueY = 0xE1234567;
	int gyroValueZ = 0xE1234567;

	double timestampe = 0;
	
	double delta1 = gyroHndl->delta1;
	double delta2 = gyroHndl->delta2;//microsecond unit
	double gyroX, gyroY, gyroZ, gyroT;

	GYRO_ANGLE_FRAME * pFrame;
	GYRO_ANGLE_PACKET* pPacket;
	GYRO_DATA_T* pCurElem;

	unsigned char *pValue;

	assert(gyroHndl != NULL );
	assert(packet_num > 0 && packet_num < GYRO_NUM_PER_FRAME);

	pFrame = (GYRO_ANGLE_FRAME*)pBuf;

	for (i = 0; i < packet_num; i++)
	{
		pPacket = (GYRO_ANGLE_PACKET*)(&pFrame->data[i]);

		if (   pPacket->sync	  == 0x0A 
			&& pPacket->x_gyro[0] == 0x0A 
			&& pPacket->x_gyro[1] == 0x0A
			&& pPacket->x_gyro[2] == 0x0A)
		{
			break;
		}

		assert( pPacket->sync == 0xA8 );
		assert( pPacket->LF   == 0x0A ); 
		assert( pPacket->CR   == 0x0D );

		//X-VALUE
		pValue = (unsigned char*)&gyroValueX;
		pValue[2] = pPacket->x_gyro[0];
		pValue[1] = pPacket->x_gyro[1];
		pValue[0] = pPacket->x_gyro[2];
		pValue[3] = 0x00;

		if ((pPacket->x_gyro[0]&0x80)==0x80)
			pValue[3] = 0xFF;

		gyroX = gyroValueX*PI/(16384*180);

		//Y-VALUE
		pValue = (unsigned char*)&gyroValueY;
		pValue[2] = pPacket->y_gyro[0];
		pValue[1] = pPacket->y_gyro[1];
		pValue[0] = pPacket->y_gyro[2];
		pValue[3] = 0x00;

		if ((pPacket->y_gyro[0]&0x80)==0x80)
			pValue[3] = 0xFF;

		gyroY = gyroValueY * PI/(16384*180);

		//Z-VALUE
		pValue = (unsigned char*)&gyroValueZ;
		pValue[2] = pPacket->z_gyro[0];
		pValue[1] = pPacket->z_gyro[1];
		pValue[0] = pPacket->z_gyro[2];
		pValue[3] = 0x00;

		if ((pPacket->z_gyro[0]&0x80)==0x80)
			pValue[3] = 0xFF;

		gyroZ = gyroValueZ*PI/(16384*180);

		//TIME-SAMPLE
		gyroT = gyroHndl->timestampe;
		gyroHndl->timestampe += delta1;

		//FILL DATA

		pCurElem = &gyroHndl->pGyroData[gyroHndl->nElem_indx];
		pCurElem->gyroX = gyroX;
		pCurElem->gyroY = gyroY;
		pCurElem->gyroZ = gyroZ;
		pCurElem->timestamp = gyroT;

		angle[0] += gyroX/1470*180/3.14;
		angle[1] += gyroY/1470*180/3.14;
		angle[2] += gyroZ/1470*180/3.14;

		//Get QUATERATION
		Gyro_gen_quateration(gyroHndl,pCurElem);

		gyroHndl->nElem_indx++;
		elemCou++;

		//All ELEM USED
		if(gyroHndl->nElem_indx >= gyroHndl->nElem_max)
		{	
			//����ʹ����,��Ҫ�Ƴ�������������,��ÿ֡��������Ϊ��λ
			int nElem_remain = 200;
			int nElem_pos    = gyroHndl->nElem_indx - nElem_remain;

			memcpy((void*)gyroHndl->pGyroData,(void*)&gyroHndl->pGyroData[nElem_pos],sizeof(GYRO_DATA_T) * nElem_remain);

			gyroHndl->nElem_indx = nElem_remain;

			gyroHndl->base_time = gyroHndl->pGyroData[0].timestamp;
		}
	}		

	gyroHndl->timestampe += (delta2-delta1);
	return elemCou;
}

int Gyro_packet_quater_insert(GYRO_HNDL gyroHndl, unsigned char* pBuf, int packet_num)
{
	int i = 0;
	int elemCou    = 0;

	int gyroValueX = 0xE1234567;
	int gyroValueY = 0xE1234567;
	int gyroValueZ = 0xE1234567;

	double timestampe = 0;
	double delta1 = gyroHndl->delta1;
	double delta2 = gyroHndl->delta2;//microsecond unit
	double gyroT;

	GYRO_QUATER_FRAME * pFrame;
	GYRO_QUATER_PACKET* pPacket;
	GYRO_DATA_T* pCurElem;

	assert(gyroHndl != NULL );
	assert(packet_num > 0 && packet_num < GYRO_NUM_PER_FRAME);

	pFrame = (GYRO_QUATER_FRAME*)pBuf;

	for (i = 0; i < packet_num; i++)
	{
		pPacket = (GYRO_QUATER_PACKET*)(&pFrame->data[i]);

		//TIME-SAMPLE
		gyroT = gyroHndl->timestampe;
		gyroHndl->timestampe += delta1;

#if 1
		//FILL DATA
		pCurElem = &gyroHndl->pGyroData[gyroHndl->nElem_indx];
		pCurElem->gyroX = pPacket->gyroX;
		pCurElem->gyroY = pPacket->gyroY;
		pCurElem->gyroZ = pPacket->gyroZ;

		//Get QUATERATION
		pCurElem->quat[0] = (double)pPacket->quat[0];
		pCurElem->quat[1] = pPacket->quat[1];
		pCurElem->quat[2] = pPacket->quat[2];
		pCurElem->quat[3] = pPacket->quat[3];
#else
		pCurElem = &gyroHndl->pGyroData[gyroHndl->nElem_indx];
		pCurElem->gyroX = 0;
		pCurElem->gyroY = 0;
		pCurElem->gyroZ = 0;
		Gyro_gen_quateration(gyroHndl,pCurElem);
#endif
		pCurElem->timestamp = gyroT;

		gyroHndl->nElem_indx++;
		elemCou++;

		//All ELEM USED
		if(gyroHndl->nElem_indx >= gyroHndl->nElem_max)
		{	
			//����ʹ����,��Ҫ�Ƴ�������������,��ÿ֡��������Ϊ��λ
			int nElem_remain = 200;

			int nElem_pos    = gyroHndl->nElem_indx - nElem_remain - 1;

			memcpy((void*)gyroHndl->pGyroData,(void*)&gyroHndl->pGyroData[nElem_pos],sizeof(GYRO_DATA_T) * nElem_remain);

			gyroHndl->nElem_indx = nElem_remain;

			gyroHndl->base_time = gyroHndl->pGyroData[0].timestamp;

		//	IALG_OutputDebugPrintf("memmove Gyro_packet_quater_insert gyroHndl->nElem_max %d nElem_pos %d, gyroHndl->base_time %f\n",gyroHndl->nElem_max,nElem_pos,gyroHndl->base_time);

		}
	}		

	gyroHndl->timestampe += (delta2-delta1);

	return elemCou;
}

int Gyro_packet_insert(GYRO_HNDL gyroHndl, unsigned char* pBuf)
{

	int i = 0;
	int iRet = 0;
	int packet_num    = 0;
	
	GSTB_PACKET* pPacket = (GSTB_PACKET*)pBuf;

	assert(gyroHndl != NULL && pBuf!=NULL);

	packet_num = pPacket->head.packet_num;

	if(pPacket->head.datType == EDATA_ORIGINAL)
	{
		iRet = Gyro_packet_angle_insert(gyroHndl, (unsigned char* )pPacket->data, packet_num);
	}
	else if(pPacket->head.datType == EDATA_QUARTER)
	{
		iRet = Gyro_packet_quater_insert(gyroHndl, (unsigned char* )pPacket->data, packet_num);
	}
	else
	{
		assert(0);
	}
	gyroHndl->datType = pPacket->head.datType;

	return iRet;
}
