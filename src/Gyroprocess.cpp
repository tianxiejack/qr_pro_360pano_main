#include"Gyroprocess.hpp"
#include"osa.h"
#include"MPU9250.hpp"
#include "FileRW.hpp"
#include"Stich.hpp"
#include"config.h"

//GYRO_NINEAXIS Nineaxis[50];
GYRO_FRAME gyrofram;
double Roll=0.0;
double Pitch=0.0;
double Yaw=0.0;


double Rollbase=0.0;
double Pitchbase=0.0;
double Yawbase=0.0;

double zeroangle=0.0;
double pretimestamp=0.0;
int oddeven=0;
void kalmanfilterinit()
{


	gyrokalmanfilterinit();
	
}



void setyawbase(double base)
{
	Yawbase+=base;
	//double pi=3.141592653;
	//setgyyawbase(base*pi/180);
}

unsigned char checksum(unsigned char *data,unsigned char sum)
{
	unsigned char crc=0;
	for(int i=0;i<36;i++)
		{
			crc^=data[3+i];

		}
	if(sum==crc)
		return 1;
	else
		return 0;

}

void proptotocal(Mat& frame)
{
	static int nintcount=0;
	int gyro=0xe1234567;
	char *pdata=(char *)&gyro;
	int numpacket=0;
	double timeinterval=0;
	int width=frame.cols;
	int height=frame.rows;
	unsigned char buffdata[40];
	unsigned char *dataptr=frame.data;
	int oddenven=0;
	numpacket=dataptr[width*(height-1)*2+1];
	oddenven=dataptr[width*(height-1)*2];
	//numpacket=dataptr[width*(height-1)*2]<<8|dataptr[width*(height-1)*2+1];
	if(numpacket==7)
	{

		//OSA_printf("THE num PACKET=%d numpacket%d\n",numpacket,nintcount);
		nintcount=0;
	}
	else
		nintcount++;

	//OSA_printf("THE num PACKET=%d numpacket%d\n",numpacket,nintcount);
	/////////////////
	//numpacket=1;
	
	if(numpacket>10)
		
		return ;
	//////////////////
	gyrofram.packetnum=0;
	for(int i=0;i<numpacket;i++)
	{
		memcpy(buffdata,&dataptr[width*(height-1)*2+2+40*i],40);
		#if 1
		//OSA_printf("THE ONE=%d TWO=%d three=%d for=%d\n",buffdata[0],buffdata[1],buffdata[2],buffdata[3]);
		if(buffdata[0]!=HEAD1_FF_NEED||buffdata[1]!=HEAD1_00_NEED||buffdata[2]!=HEAD1_55_NEED)
			continue;
		if(!checksum(buffdata,buffdata[39]))
			continue;
		#endif
		
		GYRO_NINEAXIS* niaxis=&gyrofram.gyrof[gyrofram.packetnum];
		
		niaxis->gyro_ax=buffdata[3]<<8 | buffdata[4];
		niaxis->gyro_ay=buffdata[7]<<8 | buffdata[8];
		niaxis->gyro_az=buffdata[11]<<8 | buffdata[12];
		if(BUTTERFLY)
		{
			gyro=0xe1234567;
			pdata=(char *)&gyro;
			pdata[2]=buffdata[17];
			pdata[1]=buffdata[16];
			pdata[0]=buffdata[15];
			pdata[3]=0x00;
			if((buffdata[17]&0x80)==0x80)
				{
					pdata[3]=0xff;
				}
			
			niaxis->gyro_gx=gyro;
			
			gyro=0xe1234567;
			pdata=(char *)&gyro;
			pdata[2]=buffdata[21];
			pdata[1]=buffdata[20];
			pdata[0]=buffdata[19];
			pdata[3]=0x00;
			if((buffdata[21]&0x80)==0x80)
				{
					pdata[3]=0xff;
				}
			niaxis->gyro_gy=gyro;
			gyro=0xe1234567;
			pdata=(char *)&gyro;
			pdata[2]=buffdata[25];
			pdata[1]=buffdata[24];
			pdata[0]=buffdata[23];
			pdata[3]=0x00;
			if((buffdata[25]&0x80)==0x80)
				{
					pdata[3]=0xff;
				}
			niaxis->gyro_gz=gyro;
		}
	else
		{
			niaxis->gyro_gx=buffdata[15]<<8 | buffdata[16];
			niaxis->gyro_gy=buffdata[19]<<8 | buffdata[20];
			niaxis->gyro_gz=buffdata[23]<<8 | buffdata[24];
	
		}


		niaxis->gyro_mx=buffdata[27]<<8 | buffdata[28];
		niaxis->gyro_my=buffdata[29]<<8 | buffdata[30];
		niaxis->gyro_mz=buffdata[31]<<8 | buffdata[32];

		niaxis->gyro_pretimestamp=pretimestamp;


	

		niaxis->preset=buffdata[34];
		
		//buffdata[35]=0xff;
		niaxis->gyro_timestamp=buffdata[35]<<24 | buffdata[36]<<16 | buffdata[37]<<8 | buffdata[38];
		timeinterval=niaxis->gyro_timestamp-pretimestamp;
		
		if(abs(timeinterval)>33*1000)
			{
				pretimestamp=niaxis->gyro_timestamp;
				//printf("*****timeinterval=%f************error \n",timeinterval);
				continue;
			}
	

		oddeven=oddenven;
		pretimestamp=niaxis->gyro_timestamp;
		gyrofram.packetnum++;


		/*
		OSA_printf("_________________________________________________________________\n");
		OSA_printf("THE time=%u\n",niaxis->gyro_timestamp);
		OSA_printf("ax=%d ay=%d az=%d gx=%d gy=%d gz=%d mx=%d my=%d mz=%d \n",niaxis->gyro_ax,niaxis->gyro_ay,niaxis->gyro_az,niaxis->gyro_gx,niaxis->gyro_gy,niaxis->gyro_gz,niaxis->gyro_mx,niaxis->gyro_my,niaxis->gyro_mz);
		OSA_printf("_________________________________________________________________\n");
		*/
	
	}

}

int getoddenv()
{

	return oddeven;
}

int gyrostart=0;

void setgyrostart(int gyro)
{
	gyrostart=gyro;

}
int getgyrostart()
{
	return gyrostart;
}
int  GyroCalibration()
{
	int ret=0;
	GYRO_MPU mpu;
	memset(&mpu,0,sizeof(mpu));
	for(int i=0;i<gyrofram.packetnum;i++)
	{
		mpu.gyro_axi=gyrofram.gyrof[i].gyro_ax;
		mpu.gyro_ayi=gyrofram.gyrof[i].gyro_ay;
		mpu.gyro_azi=gyrofram.gyrof[i].gyro_az;
		
		mpu.gyro_gxi=gyrofram.gyrof[i].gyro_gx;
		mpu.gyro_gyi=gyrofram.gyrof[i].gyro_gy;
		mpu.gyro_gzi=gyrofram.gyrof[i].gyro_gz;
		
		mpu.gyro_mxi=gyrofram.gyrof[i].gyro_mx;
		mpu.gyro_myi=gyrofram.gyrof[i].gyro_my;
		mpu.gyro_mzi=gyrofram.gyrof[i].gyro_mz;
		mpu.gyro_timestamp=gyrofram.gyrof[i].gyro_timestamp;
		mpu.gyro_pretimestamp=gyrofram.gyrof[i].gyro_pretimestamp;
		
		mpu.preset=gyrofram.gyrof[i].preset;
		if(BUTTERFLY)
			ret=calibrateint(&mpu.gyro_gxi,&mpu.gyro_gyi,&mpu.gyro_gzi);
		else
			ret=calibrate(&mpu.gyro_gxi,&mpu.gyro_gyi,&mpu.gyro_gzi);

		//CalibrateToZero(&mpu);

	}
	return ret;
}


void Gyrorest()
{
	mpu9250reset();
	Roll=0;
	Pitch=0;
	Yaw=0;
}

void GyrogetRPY()
{
		GYRO_MPU mpu;
		double pi=3.141592653;
		memset(&mpu,0,sizeof(mpu));
		for(int i=0;i<gyrofram.packetnum;i++)
		{
			mpu.gyro_axi=gyrofram.gyrof[i].gyro_ax;
			mpu.gyro_ayi=gyrofram.gyrof[i].gyro_ay;
			mpu.gyro_azi=gyrofram.gyrof[i].gyro_az;
			
			mpu.gyro_gxi=gyrofram.gyrof[i].gyro_gx;
			mpu.gyro_gyi=gyrofram.gyrof[i].gyro_gy;
			mpu.gyro_gzi=gyrofram.gyrof[i].gyro_gz;
			
			mpu.gyro_mxi=gyrofram.gyrof[i].gyro_mx;
			mpu.gyro_myi=gyrofram.gyrof[i].gyro_my;
			mpu.gyro_mzi=gyrofram.gyrof[i].gyro_mz;
			mpu.gyro_timestamp=gyrofram.gyrof[i].gyro_timestamp;
			mpu.gyro_pretimestamp=gyrofram.gyrof[i].gyro_pretimestamp;

			mpu.preset=gyrofram.gyrof[i].preset;
			
			GetMpugyro(&mpu);

			if(mpu.preset!=0)
				{
					//OSA_printf("preset=%d angle=%f\n",mpu.preset,mpu.yaw*180/pi+Yawbase-zeroangle);

				}

				if(getfilestoreenable())
				{

				//	OSA_printf("gx=%f gy=%f gz=%f\n",mpu.gyro_gx,mpu.gyro_gy,mpu.gyro_gz);


					writefiled(mpu.gyro_timestamp,mpu.gyro_gx,mpu.gyro_gy,mpu.gyro_gz);	
				}
			#if 0
			Roll+=mpu.roll*180/pi;
			Pitch+=mpu.pitch*180/pi;
			Yaw+=mpu.yaw*180/pi;
			#endif
		}
		Roll=mpu.roll*180/pi;
		Pitch=mpu.pitch*180/pi;
		Yaw=mpu.yaw*180/pi+Yawbase-zeroangle;

		int angle_int = (int)Yaw;
		Yaw = Yaw - angle_int/360 * 360;
		
		
		if(Yaw>360)
			Yaw=Yaw-360;
		else if(Yaw<-360)
			Yaw=Yaw+360;
		
		//OSA_printf("frame roll=%f pitch=%f yaw=%f\n",Roll,Pitch,Yaw);

}


int getGyroprocess(Mat& frame,GYRO_DATA_T *gyro)
{

	//GYRO_NINEAXIS nineaxia;
	proptotocal(frame);

	if(getgyrostart()==0)
	{
		//printf("the getgyrostart error\n ");
		return 0;
	}
	#if 1
	if(GyroCalibration()==0)
	{
		printf("the GyroCalibration error\n ");
		return 0;
	}
	#endif
	GyrogetRPY();
	//CalibrateToZero(&gyrofram);

	return 1;

}

void getEuler(double *roll,double *pitch,double *yaw)
{
	*roll=-1000*Roll;
	*pitch=-1000*Pitch;
	*yaw=-1000*Yaw;
	
	//if(Yaw>0)
	//	*yaw=0;

}

void setgyrozero(double zero)
{
	zeroangle=zero;

}
