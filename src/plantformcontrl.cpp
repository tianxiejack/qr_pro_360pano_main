#include "plantformcontrl.hpp"
#include <sys/time.h>
#include <errno.h>
#include "gpio_rdwr.h"
#include"demo_global_def.h"
#include"Status.hpp"
#include "CMessage.hpp"
#define MAX_RECV_BUF_LEN 256
Plantformpzt *Plantformpzt::instance=NULL;

static IPelcoBaseFormat *PlantformContrl;

#define UART422NAME "/dev/ttyTHS1"


#define HALFUSE 0

#define GPIP485S 186
#define GPIP485R 65

#define UART485SEND 1
#define UART485RECV 0
#define SENDDELAY (0)
Plantformpzt::Plantformpzt():fd(0),mainloop(1),address(1),ptzpd(0),panangle(0),titleangle(0),calibration(0),plantformpan(720),plantformtitle(720),
plantinitflag(0),speedpan(30),speedtitle(30),titlpanangle(-7.3),plantformpanforever(0),plantformtitleforever(0),scanflag(0)
{
	
	memset(&platformcom,0,sizeof(platformcom));
	memset(callbackeable,0,sizeof(callbackeable));
	memset(callbackpan,0,sizeof(callbackpan));
	memset(callbacktitle,0,sizeof(callbacktitle));
	memset(callback,0,sizeof(callbacktitle));
	OSA_mutexCreate(&lock);
	
	
}
Plantformpzt::~Plantformpzt()
{
	GPIO_close(GPIP485S);
	
	GPIO_close(GPIP485R);

	OSA_mutexDelete(&lock);
}

Plantformpzt* Plantformpzt::getinstance()
{
	if(instance==NULL)
		instance=new Plantformpzt();
	
	return instance;
}


void Plantformpzt::create()
{
	memset(timeout,0,sizeof(timeout));
	if(HALFUSE)
		{
			GPIO_create(GPIP485S,GPIO_DIRECTION_OUT);
			GPIO_create(GPIP485R,GPIO_DIRECTION_OUT);
			GPIO_set(GPIP485S,UART485RECV);
			GPIO_set(GPIP485R,UART485RECV);
		}
	platformcom.recvBuf=recvbuf;
	platformcom.recvBuf=sendbuf;
	titlpanangle=Config::getinstance()->getpanozeroptztitle();
	speedtitle=speedpan=Config::getinstance()->getptzspeed();
	timeoutflag[PLANTFORMINITPAN]=-1;
	timeoutflag[PLANTFORMINITTITLE]=-1;
	plantformcontrlinit();
	MAIN_threadRecvCreate();
	MAIN_contrlthreadCreate();
	registorfun();
	

}

void Plantformpzt::destery()
{
	MAIN_threadRecvDestroy();
	MAIN_contrlthreadDestroy();
	plantformcontrluninit();

}


int Plantformpzt::MAIN_threadRecvCreate(void)
{
	int iRet = OSA_SOK;
	iRet = OSA_semCreate(&mainRecvThrObj.procNotifySem ,1,0) ;
	OSA_assert(iRet == OSA_SOK);
	mainRecvThrObj.exitProcThread = false;
	mainRecvThrObj.initFlag = true;
	mainRecvThrObj.pParent = (void*)this;
	iRet = OSA_thrCreate(&mainRecvThrObj.thrHandleProc, mainRecvTsk, 0, 0, &mainRecvThrObj);
	

	return iRet;
}


int Plantformpzt::MAIN_threadRecvDestroy(void)
{
	int iRet = OSA_SOK;

	mainRecvThrObj.exitProcThread = true;
	OSA_semSignal(&mainRecvThrObj.procNotifySem);

	iRet = OSA_thrDelete(&mainRecvThrObj.thrHandleProc);

	mainRecvThrObj.initFlag = false;
	OSA_semDelete(&mainRecvThrObj.procNotifySem);

	return iRet;
}

int Plantformpzt::MAIN_contrlthreadCreate(void)
{
	int iRet = OSA_SOK;
	iRet = OSA_semCreate(&mainContrlThrdetectObj.procNotifySem ,1,0) ;
	OSA_assert(iRet == OSA_SOK);


	mainContrlThrdetectObj.exitProcThread = false;

	mainContrlThrdetectObj.initFlag = true;

	mainContrlThrdetectObj.pParent = (void*)this;
	
	iRet = OSA_thrCreate(&mainContrlThrdetectObj.thrHandleProc, maincontrlTsk, 0, 0, &mainContrlThrdetectObj);
	

	return iRet;
}


int Plantformpzt::MAIN_contrlthreadDestroy(void)
{
	int iRet = OSA_SOK;

	mainContrlThrdetectObj.exitProcThread = true;
	OSA_semSignal(&mainContrlThrdetectObj.procNotifySem);

	iRet = OSA_thrDelete(&mainContrlThrdetectObj.thrHandleProc);

	mainContrlThrdetectObj.initFlag = false;
	OSA_semDelete(&mainContrlThrdetectObj.procNotifySem);

	return iRet;
}
int Plantformpzt::COMCTRL_lookupSync(ComObj*pObj)
{
	int stat 		= -1;
	Uint8 *pCur 	= pObj->recvBuf;
	unsigned char  *pMsg	= (unsigned char *)pCur;

	//OSA_printf("%x  %x  %x  %x  %x  %x  %x  \n",pMsg[0],pMsg[1],pMsg[2],pMsg[3],pMsg[4],pMsg[5],pMsg[6]);
	
	while(pObj->recvLen >= 3)
	{
		if(pMsg[0]==0xFF && (pMsg[1]==0x01) && (pMsg[2]==0x00)){
			stat = 0;
			break;
		}
		else{
			pCur++;
			pObj->recvLen--;
			pMsg= (unsigned char *)pCur;
		}
	}
	
	if(pCur!=pObj->recvBuf){
		memcpy(pObj->recvBuf, pCur, pObj->recvLen);
	}

	return stat;
}

int Plantformpzt::COMCTRL_checkSum(ComObj* pObj)
{
	UINT iCou=0;
	unsigned char checkSum=0;
	unsigned char  *pMsg= (unsigned char  *)pObj->recvBuf;
	//return SDK_SOK;
	if(pMsg[6]!=chechsum(pMsg))
			{
			
				return -1 ;
			}
	
	return SDK_SOK;
}


void Plantformpzt::main_Recv_func()
{
	OSA_printf("%s: Main Proc Tsk Is Entering...\n",__func__);
	unsigned int framecount=0;

	int queueid=0;

	int xoffset,yoffset;
	double imageangle=0;
	int buflen=0;
	unsigned short pana=0;
	unsigned short titlea=0;


	int headLenNetMsg=3;
	int LenNetMsg =7;
	char *pMsgHead=NULL;
	pMsgHead 	= (char *)platformcom.recvBuf;
	int iLen=0, nWait=0, recvLen=0, result=0, stat=0;
	
	while(mainRecvThrObj.exitProcThread ==  false)
	{

			buflen=Uart.UartRecv( fd,  platformcom.recvBuf, MAX_RECV_BUF_LEN-platformcom.recvLen);
			/*
			for(int i=0;i<buflen;i++)
			OSA_printf("[%d]=%x\t",i,platformcom.recvBuf[i]);
			OSA_printf("buflen=%d\n",buflen);
			*/

			if(buflen<=0)
				continue;

			platformcom.recvLen += buflen;
			if(platformcom.recvLen == MAX_RECV_BUF_LEN)
			{
				//printf(" [DEBUG:] %s recv buf is full, clean !!!\n",__func__);
				//platformcom.recvLen = 0;
			}


			//OSA_printf("platformcom.recvLen=%d headLenNetMsg=%d\n",platformcom.recvLen,headLenNetMsg);
			while(platformcom.recvLen >= headLenNetMsg)
			{
				// check msg head and data len
				if(nWait == 0)
				{
					stat = COMCTRL_lookupSync(&platformcom);
					if(stat==SDK_SOK)
					{
						nWait = platformcom.recvLen - LenNetMsg;
						iLen = LenNetMsg;
						//printf(" [DEBUG:] %s lookup head ok type=%02x size=%02x\r\n", 
						//		__func__, pMsgHead->eCmdType, pMsgHead->uiSize);
					}
					else
						break;
				}
				// deal data part

				
				//printf("********LINE=%d*****nWait=%d******************\n",__LINE__,nWait);
				if(nWait >= 0)
				{
						if(COMCTRL_checkSum(&platformcom) == SDK_SOK)
				                {
									if(platformcom.recvBuf[3]==0x59)
							{
								pana=platformcom.recvBuf[4]<<8|platformcom.recvBuf[5];
								panangle=pana*1.0/100;
							}
						else if(platformcom.recvBuf[3]==0x5b)
							{
								titlea=platformcom.recvBuf[4]<<8|platformcom.recvBuf[5];
								titleangle=titlea*1.0/100;

							}
						plantformpan=panangle;
						plantformtitle=titleangle;
						
						//printf("***plantformpan=%f**plantformtitle=%f******************\n",plantformpan,plantformtitle);
						
						if(timeoutflag[PLANTFORMGETTITLE]==1||timeoutflag[PLANTFORMGETPAN]==1)
							{
								printf("*****************ptz OSA_semSignal********************\n");
								if(timeoutflag[PLANTFORMGETTITLE]==1)
									timeoutflag[PLANTFORMGETTITLE]=0;
								if(timeoutflag[PLANTFORMGETPAN]==1)
									timeoutflag[PLANTFORMGETPAN]=0;
								OSA_semSignal(&mainRecvThrObj.procNotifySem);
								printf("timeoutflag[PLANTFORMGETTITLE]=%d  timeoutflag[PLANTFORMGETPAN]=%d\n",timeoutflag[PLANTFORMGETTITLE],timeoutflag[PLANTFORMGETPAN]);
							}
		                }
				else
						;
							//printf(" [DEBUG:] %s check sum error type\n", __func__);
	                    
					platformcom.recvLen = nWait;
					if(platformcom.recvLen > 0)
					{
						memcpy(platformcom.recvBuf, platformcom.recvBuf+iLen, platformcom.recvLen);
						nWait = 0;
					}
				}
				else
				{
					nWait = 0;
					break;
				}
			}// while(packet - recvbuf <= buffLen)     




/*
			
			if(buflen!=SENDLEN)
				{
				
					continue;
				}
			if(recvbuff[6]!=chechsum(recvbuff))
				{
				
					continue ;
				}
			if(recvbuff[0]==0xff&&recvbuff[1]==0x01&&recvbuff[2]==0x00)
				{
					if(recvbuff[3]==0x59)
						{
							pana=recvbuff[4]<<8|recvbuff[5];
							panangle=pana*1.0/100;
						}
					else if(recvbuff[3]==0x5b)
						{
							titlea=recvbuff[4]<<8|recvbuff[5];
							titleangle=titlea*1.0/100;

						}
					plantformpan=panangle;
					plantformtitle=titleangle;
					printf("*****************ptz OSA_semSignal********************\n");
					if(timeoutflag[PLANTFORMGETTITLE]==1||timeoutflag[PLANTFORMGETPAN]==1)
						OSA_semSignal(&mainRecvThrObj.procNotifySem);
				}
			
			
			*/
			//puts(recvbuff);
		}



}
void Plantformpzt::milliseconds_sleep(unsigned long mSec)
{
    struct timeval tv;
    tv.tv_sec=mSec/1000;
    tv.tv_usec=(mSec%1000)*1000;
    int err;
    do{
       err=select(0,NULL,NULL,NULL,&tv);
    }while(err<0 && errno==EINTR);
}
void Plantformpzt::main_contrl_func()
{
	OSA_printf("%s: Main Proc Tsk Is Entering...\n",__func__);


	double anglepan=0;
	double angletitle=0;
	angletitle=360+titlpanangle;
	if(angletitle>=360)
		angletitle=angletitle-360;
	else if(angletitle<0)
		angletitle=angletitle+360;
	anglepan=Config::getinstance()->getpanozeroptz();

	while(mainContrlThrdetectObj.exitProcThread ==  false)
	{
		milliseconds_sleep(330);
		if(timeoutflag[PLANTFORMGETPAN])
			{
				timeout[PLANTFORMGETPAN]++;
				if(timeout[PLANTFORMGETPAN]>TIMEOUT)
					OSA_semSignal(&mainRecvThrObj.procNotifySem);

			}
		else
			timeout[PLANTFORMGETPAN]=0;


		if(timeoutflag[PLANTFORMGETTITLE])
			{
				timeout[PLANTFORMGETTITLE]++;
				if(timeout[PLANTFORMGETTITLE]>TIMEOUT)
					OSA_semSignal(&mainRecvThrObj.procNotifySem);

			}
		else
			timeout[PLANTFORMGETTITLE]=0;


		if(timeoutflag[PLANTFORMINITPAN]==1)
			{
				double angle=0;
				angle=getpanangle();
				double angleoffet=angle-anglepan;
				if(angleoffet>300)
					angleoffet=angleoffet-360;
				if(angleoffet<-300)
					angleoffet=angleoffet+360;
				if(abs(angleoffet)<0.2)
					{
						timeoutflag[PLANTFORMINITPAN]=0;
						continue;
					}
				printf("angle=%f anglepan=%f\n",angle,anglepan);
				
				OSA_waitMsecs(1000);
				//initptzpos(anglepan,angletitle);

				setpanopanpos(anglepan);
				getpanopanpos();

			}

		if(timeoutflag[PLANTFORMINITTITLE]==1)
			{
				double angle=0;
				angle=gettitleangle();
				double angleoffet=angle-angletitle;
				if(angleoffet>300)
					angleoffet=angleoffet-360;
				if(angleoffet<-300)
					angleoffet=angleoffet+360;
				if(abs(angleoffet)<0.2)
					{
						timeoutflag[PLANTFORMINITTITLE]=0;
						continue;
					}
				
				printf("angle=%f angletitle=%f\n",angle,angletitle);
				
				OSA_waitMsecs(1000);
				//initptzpos(anglepan,angletitle);
				setpanotitlepos(angletitle);
				getpanotitlepos();
				

			}
////////////////////////
		if(timeoutflag[PLANTFORMPANFOREVER]==1)
			{
				double angle=0;
				getpanopanpos();
				angle=getpanangle();
				double angleoffetpan=angle-plantformpanforever;
				if(angleoffetpan>300)
					angleoffetpan=angleoffetpan-360;
				if(angleoffetpan<-300)
					angleoffetpan=angleoffetpan+360;
				
				if(abs(angleoffetpan)<0.1)
					{
						timeoutflag[PLANTFORMPANFOREVER]=0;			
					}
				setpanopanpos(plantformpanforever);

				//printf("***********PLANTFORMPANFOREVER******************\n");

			}
		if(timeoutflag[PLANTFORMTITLEFOREVER]==1)
			{
				double angle=0;
				getpanotitlepos();
				angle=gettitleangle();
				double angleoffettitle=angle-plantformtitleforever;
				if(angleoffettitle>300)
					angleoffettitle=angleoffettitle-360;
				if(angleoffettitle<-300)
					angleoffettitle=angleoffettitle+360;
				if(abs(angleoffettitle)<0.1)
					{
						timeoutflag[PLANTFORMTITLEFOREVER]=0;	
						
					}
				setpanotitlepos(plantformtitleforever);

				//printf("***********PLANTFORMTITLEFOREVER******************\n");
			}



/////////////////////////////


		//if(timeoutflag[PLANTFORMGETCALLBACK]==1)
			{
				if(callbackeable[RENDERPANO]==1)
					{
						double angle=0;
						getpanopanpos();
						getpanotitlepos();
						angle=gettitleangle();
						double angleoffettitle=angle-callbacktitle[RENDERPANO];
						if(angleoffettitle>300)
							angleoffettitle=angleoffettitle-360;
						if(angleoffettitle<-300)
							angleoffettitle=angleoffettitle+360;
						angle=getpanangle();
						double angleoffetpan=angle-callbackpan[RENDERPANO];
						if(angleoffetpan>300)
							angleoffetpan=angleoffetpan-360;
						if(angleoffetpan<-300)
							angleoffetpan=angleoffetpan+360;
						
						if(abs(angleoffetpan)>0.1)
							setpanopanpos(callbackpan[RENDERPANO]);
						else if(abs(angleoffettitle)>0.1)
							setpanotitlepos(callbacktitle[RENDERPANO]);
						
						if(abs(angleoffetpan)<0.1&&abs(angleoffettitle)<0.1)
							{
								callbackeable[RENDERPANO]=0;
								//timeoutflag[PLANTFORMGETCALLBACK]=0;
								if(callback[RENDERPANO]!=NULL)
									callback[RENDERPANO](NULL);
								
							}
						

					}



				if(callbackeable[RENDERSIGNALPANO]==1)
					{
						double angle=0;
						getpanopanpos();
						getpanotitlepos();
						angle=gettitleangle();
						double angleoffettitle=angle-callbacktitle[RENDERSIGNALPANO];
						if(angleoffettitle>300)
							angleoffettitle=angleoffettitle-360;
						if(angleoffettitle<-300)
							angleoffettitle=angleoffettitle+360;
						angle=getpanangle();
						double angleoffetpan=angle-callbackpan[RENDERSIGNALPANO];
						if(angleoffetpan>300)
							angleoffetpan=angleoffetpan-360;
						if(angleoffetpan<-300)
							angleoffetpan=angleoffetpan+360;
						
						if(abs(angleoffetpan)>0.1)
							setpanopanpos(callbackpan[RENDERSIGNALPANO]);
						else if(abs(angleoffettitle)>0.1)
							setpanotitlepos(callbacktitle[RENDERSIGNALPANO]);
						
						if(abs(angleoffetpan)<0.1&&abs(angleoffettitle)<0.1)
							{
								callbackeable[RENDERSIGNALPANO]=0;
								//timeoutflag[PLANTFORMGETCALLBACK]=0;
								if(callback[RENDERSIGNALPANO]!=NULL)
									callback[RENDERSIGNALPANO](NULL);
								
							}
						

					}


				if(callbackeable[PRESETGO]==1)
					{
						double angle=0;
						getpanopanpos();
						getpanotitlepos();
						angle=gettitleangle();
						double angleoffettitle=angle-callbacktitle[PRESETGO];
						if(angleoffettitle>300)
							angleoffettitle=angleoffettitle-360;
						if(angleoffettitle<-300)
							angleoffettitle=angleoffettitle+360;
						angle=getpanangle();
						double angleoffetpan=angle-callbackpan[PRESETGO];
						if(angleoffetpan>300)
							angleoffetpan=angleoffetpan-360;
						if(angleoffetpan<-300)
							angleoffetpan=angleoffetpan+360;
						
						if(abs(angleoffetpan)>0.1)
							setpanopanpos(callbackpan[PRESETGO]);
						else if(abs(angleoffettitle)>0.1)
							setpanotitlepos(callbacktitle[PRESETGO]);
						
						if(abs(angleoffetpan)<0.1&&abs(angleoffettitle)<0.1)
							{
								callbackeable[PRESETGO]=0;
								//timeoutflag[PLANTFORMGETCALLBACK]=0;
								if(callback[PRESETGO]!=NULL)
									callback[PRESETGO](NULL);
								
							}
						

					}


				if(callbackeable[MVDETECTGO]==1)
					{
						double angle=0;
						getpanopanpos();
						getpanotitlepos();
						angle=gettitleangle();
						double angleoffettitle=angle-callbacktitle[MVDETECTGO];
						if(angleoffettitle>300)
							angleoffettitle=angleoffettitle-360;
						if(angleoffettitle<-300)
							angleoffettitle=angleoffettitle+360;
						angle=getpanangle();
						double angleoffetpan=angle-callbackpan[MVDETECTGO];
						if(angleoffetpan>300)
							angleoffetpan=angleoffetpan-360;
						if(angleoffetpan<-300)
							angleoffetpan=angleoffetpan+360;
						
						if(abs(angleoffetpan)>0.1)
							setpanopanpos(callbackpan[MVDETECTGO]);
						else if(abs(angleoffettitle)>0.1)
							setpanotitlepos(callbacktitle[MVDETECTGO]);
						
						if(abs(angleoffetpan)<0.1&&abs(angleoffettitle)<0.1)
							{
								callbackeable[MVDETECTGO]=0;

								testangle=angle;
								//timeoutflag[PLANTFORMGETCALLBACK]=0;
								if(callback[MVDETECTGO]!=NULL)
									callback[MVDETECTGO](&testangle);
								
							}
						

					}


			}

		if(timeoutflag[PLANTFORMINITTITLE]==0&&timeoutflag[PLANTFORMINITPAN]==0)
			plantinitflag=1;
		else
			plantinitflag=0;


		timeout[PLANTFORMGETPAN]++;
		if(timeout[PLANTFORMGETPAN]%5000==0)
			{
				if((timeoutflag[PLANTFORMGETTITLE]==0)&&(timeoutflag[PLANTFORMGETPAN]==0))
					{
						getpanotitlepos();
						getpanopanpos();
					}
			}
		timeout[PLANTSCAN]++;
		if(timeout[PLANTSCAN]%5==0)
			{
				if(scanflag)
					{
						if(timeout[PLANTSCAN]==100)
							scanflag=0;
						setpanoscan();
					}
			}
		
		
		
		
	}



}


void Plantformpzt::setplantformcalibration(int flag)
{
	calibration=flag;
}
int  Plantformpzt::getplantformcalibration()
{
	return calibration;
}
unsigned char Plantformpzt::chechsum(unsigned char *pelcodbuf)
{
	unsigned char sum=0;
	sum+=pelcodbuf[1];
	sum+=pelcodbuf[2];
	sum+=pelcodbuf[3];
	sum+=pelcodbuf[4];
	sum+=pelcodbuf[5];
	return sum;
	

}





void Plantformpzt::plantformcontrlinit()
{
	int iRet = OSA_SOK;
	address=Config::getinstance()->getptzaddres();
	ptzpd=Config::getinstance()->getptzdp();
	if(ptzpd==0)
	PlantformContrl=IPelcoFactory::createIpelco(pelco_D);
	else
	PlantformContrl=IPelcoFactory::createIpelco(pelco_P);
	
	fd=Uart.UartOpen(UART422NAME);
	int boad=Config::getinstance()->getptzbroad();
	Uart.UartSet(fd, boad, 8, 'n', 1);



	double anglepan=0;
	double angletitle=0;
	angletitle=360+titlpanangle;
	if(angletitle>=360)
		angletitle=angletitle-360;
	else if(angletitle<0)
		angletitle=angletitle+360;
	anglepan=Config::getinstance()->getpanozeroptz();


	initptzpos(anglepan,angletitle);
	timeoutflag[PLANTFORMINITPAN]=1;
	timeoutflag[PLANTFORMINITTITLE]=1;
	
	/*
	while(PTZOK)
		{
			double angle=0;
			angle=getpanotitle();
			double angleoffet=angle-angletitle;
			if(angleoffet>300)
				angleoffet=angleoffet-360;
			if(angleoffet<-300)
				angleoffet=angleoffet+360;
			if(abs(angleoffet)<0.2)
				break;
			OSA_waitMsecs(1000);
			initptzpos(anglepan,angletitle);
			OSA_printf("angle=%f angletitle=%f ",angle,angletitle);
		}
	
	while(PTZOK)
		{
			double angle=0;
			angle=getpanopan();
			double angleoffet=angle-anglepan;
			if(angleoffet>300)
				angleoffet=angleoffet-360;
			if(angleoffet<-300)
				angleoffet=angleoffet+360;
			if(abs(angleoffet)<0.2)
				break;
			OSA_waitMsecs(1000);
			initptzpos(anglepan,angletitle);
			OSA_printf("angle=%f anglepan=%f ",angle,anglepan);
		}
		*/
	//Uart.UartSet(fd, 1, 8, 'n', 1);

	


}


void Plantformpzt::setpanoscan()
{
	int len=0;
	PlantformContrl->MakeMove(&PELCO_D, PTZ_MOVE_Right,speedpan,true, address);


	unsigned char *pelcodbuf=( unsigned char *) &PELCO_D;
	OSA_waitMsecs(50);
	//len=Uart.UartSend(fd,( unsigned char *) buf, strlen(buf));
	if(HALFUSE)
		{
			GPIO_set(GPIP485S,UART485SEND);
			GPIO_set(GPIP485R,UART485SEND);
			len=Uart.UartSend(fd,pelcodbuf,SENDLEN);
			OSA_waitMsecs(SENDDELAY);
			GPIO_set(GPIP485S,UART485RECV);
			GPIO_set(GPIP485R,UART485RECV);
		}
	else
		{
			OSA_mutexLock(&instance->lock);
			len=Uart.UartSend(fd,pelcodbuf,SENDLEN);
			OSA_mutexUnlock(&instance->lock);

		}
	scanflag=1;
	OSA_waitMsecs(50);
	//printf("********************ok*****************send len=%d\n",len);
}

void Plantformpzt::setpanoantiscan()
{
	int len=0;
	PlantformContrl->MakeMove(&PELCO_D, PTZ_MOVE_Left,speedpan,true, address);


	unsigned char *pelcodbuf=( unsigned char *) &PELCO_D;

	//len=Uart.UartSend(fd,( unsigned char *) buf, strlen(buf));
	if(HALFUSE)
		{
			GPIO_set(GPIP485S,UART485SEND);
			GPIO_set(GPIP485R,UART485SEND);
			len=Uart.UartSend(fd,pelcodbuf,SENDLEN);
			OSA_waitMsecs(SENDDELAY);
			GPIO_set(GPIP485S,UART485RECV);
			GPIO_set(GPIP485R,UART485RECV);
		}
	else
		{
			OSA_mutexLock(&instance->lock);
			len=Uart.UartSend(fd,pelcodbuf,SENDLEN);
			OSA_mutexUnlock(&instance->lock);

		}
	OSA_waitMsecs(10);
	//printf("********************ok*****************send len=%d\n",len);
}
void Plantformpzt::setpanoscanstop()
{
	scanflag=0;
	PlantformContrl->MakeMove(&PELCO_D, PTZ_MOVE_Stop,0x10,true, address);
	if(HALFUSE)
		{
			GPIO_set(GPIP485S,UART485SEND);
			GPIO_set(GPIP485R,UART485SEND);
			Uart.UartSend(fd,( unsigned char *) &PELCO_D, SENDLEN);
			OSA_waitMsecs(SENDDELAY);
			GPIO_set(GPIP485S,UART485RECV);
			GPIO_set(GPIP485R,UART485RECV);
		}
	else
		{
			OSA_mutexLock(&instance->lock);
			Uart.UartSend(fd,( unsigned char *) &PELCO_D, SENDLEN);
			OSA_mutexUnlock(&instance->lock);

		}
	
	OSA_waitMsecs(10);
}


void Plantformpzt::registcall(PLANTFORMNOTIFYFUNClk fun,int index)
{

	callback[index]=fun;

}
void Plantformpzt::Enbalecallback(int index,double pan,double title)
{
	callbackpan[index]=pan;
	callbacktitle[index]=title;
	callbackeable[index]=1;
	timeoutflag[PLANTFORMGETCALLBACK]=1;

}

void Plantformpzt::setpanopanpos(double value)
{
	if(Config::getinstance()->getptzpaninverse())
		{
			value=-value;
		}
	if(value>360)
		value=value-360;
	if(value<0)
		value=value+360;
	
	if(getplantformcalibration()==0)
		return ;

	//printf("*****************************\n");
	//printf("******%s******value=%f*****************\n",__func__,value);
	unsigned short panvalue=value*100;
	PlantformContrl->MakeSetPanPos(&PELCO_D, panvalue,address);
	if(HALFUSE)
		{
			GPIO_set(GPIP485S,UART485SEND);
			GPIO_set(GPIP485R,UART485SEND);
			Uart.UartSend(fd,( unsigned char *)& PELCO_D, SENDLEN);
			OSA_waitMsecs(SENDDELAY);
			GPIO_set(GPIP485S,UART485RECV);
			GPIO_set(GPIP485R,UART485RECV);
		}
	else
		{
			OSA_mutexLock(&instance->lock);
			Uart.UartSend(fd,( unsigned char *)& PELCO_D, SENDLEN);
			OSA_mutexUnlock(&instance->lock);
		}
	
	OSA_waitMsecs(50);
	
	//printf("getpanopan=%f\n",getpanopan());

	//getpanopanpos();

}

void Plantformpzt::setpanotitlepos(double value)
{

	if(Config::getinstance()->getptztitleinverse())
		{
			;//value=-value;
		}
	if(value>360)
		value=value-360;
	if(value<0)
		value=value+360;

	
      printf("******%s******value=%f*****************\n",__func__,value);
	
	if(getplantformcalibration()==0)
		return ;
	unsigned short panvalue=value*100;
	PlantformContrl->MakeSetTilPos(&PELCO_D, panvalue,address);
	if(HALFUSE)
		{
			GPIO_set(GPIP485S,UART485SEND);
			GPIO_set(GPIP485R,UART485SEND);
			Uart.UartSend(fd,( unsigned char *) &PELCO_D, SENDLEN);
			OSA_waitMsecs(SENDDELAY);
			GPIO_set(GPIP485S,UART485RECV);
			GPIO_set(GPIP485R,UART485RECV);
		}
	else
		{
			OSA_mutexLock(&instance->lock);
			Uart.UartSend(fd,( unsigned char *) &PELCO_D, SENDLEN);
			OSA_mutexUnlock(&instance->lock);
		}
	OSA_waitMsecs(50);
	

}

int Plantformpzt::getpanopanforeverstatus()
{

	return timeoutflag[PLANTFORMPANFOREVER];

}

int Plantformpzt::getpanotitleforeverstatus()
{

	return timeoutflag[PLANTFORMTITLEFOREVER];

}
void Plantformpzt::setpanopanforever(double value)
{
	if(Config::getinstance()->getptzpaninverse())
		{
			value=-value;
		}
	if(value>360)
		value=value-360;
	if(value<0)
		value=value+360;
	
	if(getplantformcalibration()==0)
		return ;

	plantformpanforever=value;
	timeoutflag[PLANTFORMPANFOREVER]=1;


}

void Plantformpzt::setpanotitleforever(double value)
{

	if(Config::getinstance()->getptztitleinverse())
		{
			value=-value;
		}
	if(value>360)
		value=value-360;
	if(value<0)
		value=value+360;

	printf("******%s******value=%f*****************\n",__func__,value);
	if(getplantformcalibration()==0)
		return ;
	
	plantformtitleforever=value;
	timeoutflag[PLANTFORMTITLEFOREVER]=1;
	

}

void Plantformpzt::initptzpos(double pan,double title)
{
	if(Config::getinstance()->getptzpaninverse())
		{
			pan=-pan;
		}
	if(pan>360)
		pan=pan-360;
	if(pan<0)
		pan=pan+360;


	if(Config::getinstance()->getptztitleinverse())
		{
			title=-title;
		}
	if(title>360)
		title=title-360;
	if(title<0)
		title=title+360;
 //  printf("******%s******value=%f*****************\n",__func__,pan);
	
	unsigned short panvalue=title*100;
	PlantformContrl->MakeSetTilPos(&PELCO_D, panvalue,address);
	if(HALFUSE)
		{
			GPIO_set(GPIP485S,UART485SEND);
			GPIO_set(GPIP485R,UART485SEND);
			Uart.UartSend(fd,( unsigned char *) &PELCO_D, SENDLEN);
			OSA_waitMsecs(SENDDELAY);
			GPIO_set(GPIP485S,UART485RECV);
			GPIO_set(GPIP485R,UART485RECV);
		}
	else
		{
			OSA_mutexLock(&instance->lock);
			Uart.UartSend(fd,( unsigned char *) &PELCO_D, SENDLEN);
			OSA_mutexUnlock(&instance->lock);
		}

	OSA_waitMsecs(10);
	panvalue=pan*100;
	PlantformContrl->MakeSetPanPos(&PELCO_D, panvalue,address);
	if(HALFUSE)
		{
			GPIO_set(GPIP485S,UART485SEND);
			GPIO_set(GPIP485R,UART485SEND);
			Uart.UartSend(fd,( unsigned char *) &PELCO_D, SENDLEN);
			OSA_waitMsecs(SENDDELAY);
			GPIO_set(GPIP485S,UART485RECV);
			GPIO_set(GPIP485R,UART485RECV);
		}
	else
		{
			OSA_mutexLock(&instance->lock);
			Uart.UartSend(fd,( unsigned char *) &PELCO_D, SENDLEN);
			OSA_mutexUnlock(&instance->lock);

		}
	OSA_waitMsecs(10);



	
	

}

double Plantformpzt::getpanopan()
{
	unsigned char *pelcodbuf=( unsigned char *) &PELCO_D;
	pelcodbuf[0]=0xff;
	pelcodbuf[1]=0x01;
	pelcodbuf[2]=0x00;
	pelcodbuf[3]=0x51;
	pelcodbuf[4]=0x00;
	pelcodbuf[5]=0x00;
	pelcodbuf[6]=0x52;
	if(HALFUSE)
		{
			GPIO_set(GPIP485S,UART485SEND);
			GPIO_set(GPIP485R,UART485SEND);
			Uart.UartSend(fd,( unsigned char *) &PELCO_D, SENDLEN);
			OSA_waitMsecs(SENDDELAY);
			GPIO_set(GPIP485S,UART485RECV);
			GPIO_set(GPIP485R,UART485RECV);
		}
	else
		{
			OSA_mutexLock(&instance->lock);
			Uart.UartSend(fd,( unsigned char *) &PELCO_D, SENDLEN);
			OSA_mutexUnlock(&instance->lock);

		}
	timeoutflag[PLANTFORMGETPAN]=0;
	printf("*******%s*******\n",__func__);
	//if(Config::getinstance()->getptzwait())
	//OSA_semWait(&mainRecvThrObj.procNotifySem,OSA_TIMEOUT_FOREVER);
	OSA_waitMsecs(100);
	timeoutflag[PLANTFORMGETPAN]=0;
	return panangle;

}

double Plantformpzt::getpanotitle()
{
	unsigned char *pelcodbuf=( unsigned char *) &PELCO_D;
	pelcodbuf[0]=0xff;
	pelcodbuf[1]=0x01;
	pelcodbuf[2]=0x00;
	pelcodbuf[3]=0x53;
	pelcodbuf[4]=0x00;
	pelcodbuf[5]=0x00;
	pelcodbuf[6]=0x54;
	if(HALFUSE)
		{
			GPIO_set(GPIP485S,UART485SEND);
			GPIO_set(GPIP485R,UART485SEND);
			Uart.UartSend(fd,( unsigned char *) &PELCO_D, SENDLEN);
			OSA_waitMsecs(SENDDELAY);
			GPIO_set(GPIP485S,UART485RECV);
			GPIO_set(GPIP485R,UART485RECV);
		}
	else
		{
			OSA_mutexLock(&instance->lock);
			Uart.UartSend(fd,( unsigned char *) &PELCO_D, SENDLEN);
			OSA_mutexUnlock(&instance->lock);

		}
	timeoutflag[PLANTFORMGETTITLE]=0;
	printf("*******%s*******\n",__func__);
	//if(Config::getinstance()->getptzwait())
	//OSA_semWait(&mainRecvThrObj.procNotifySem,OSA_TIMEOUT_FOREVER);
	OSA_waitMsecs(100);
	timeoutflag[PLANTFORMGETTITLE]=0;
	return titleangle;
}

void Plantformpzt::getpanopanpos()
{

	unsigned char *pelcodbuf=( unsigned char *) &PELCO_D;
	pelcodbuf[0]=0xff;
	pelcodbuf[1]=0x01;
	pelcodbuf[2]=0x00;
	pelcodbuf[3]=0x51;
	pelcodbuf[4]=0x00;
	pelcodbuf[5]=0x00;
	pelcodbuf[6]=0x52;
	if(HALFUSE)
		{
			GPIO_set(GPIP485S,UART485SEND);
			GPIO_set(GPIP485R,UART485SEND);
			Uart.UartSend(fd,( unsigned char *) &PELCO_D, SENDLEN);
			OSA_waitMsecs(SENDDELAY);
			GPIO_set(GPIP485S,UART485RECV);
			GPIO_set(GPIP485R,UART485RECV);
		}
	else
		{
			OSA_mutexLock(&instance->lock);
			Uart.UartSend(fd,( unsigned char *) &PELCO_D, SENDLEN);
			OSA_mutexUnlock(&instance->lock);

		}
	//printf("*******%s*******\n",__func__);
	OSA_waitMsecs(10);

}

void Plantformpzt::getpanotitlepos()
{
	
	
	unsigned char *pelcodbuf=( unsigned char *) &PELCO_D;
	pelcodbuf[0]=0xff;
	pelcodbuf[1]=0x01;
	pelcodbuf[2]=0x00;
	pelcodbuf[3]=0x53;
	pelcodbuf[4]=0x00;
	pelcodbuf[5]=0x00;
	pelcodbuf[6]=0x54;
	if(HALFUSE)
		{
		
			GPIO_set(GPIP485S,UART485SEND);
			GPIO_set(GPIP485R,UART485SEND);
			Uart.UartSend(fd,( unsigned char *) &PELCO_D, SENDLEN);
			OSA_waitMsecs(SENDDELAY);
			GPIO_set(GPIP485S,UART485RECV);
			GPIO_set(GPIP485R,UART485RECV);
		}
	else
		{
			OSA_mutexLock(&instance->lock);
			Uart.UartSend(fd,( unsigned char *) &PELCO_D, SENDLEN);
			OSA_mutexUnlock(&instance->lock);
		}
	//printf("*******%s*******\n",__func__);
	OSA_waitMsecs(10);

}

void Plantformpzt::registorfun()
{
	CMessage::getInstance()->MSGDRIV_register(MSGID_EXT_INPUT_PLATCTRL,ptzcontrl,0);
	CMessage::getInstance()->MSGDRIV_register(MSGID_EXT_INPUT_PlantfromConfig,plantfromcontrl,0);
	
}

void Plantformpzt::ptzcontrl(long lParam)
{
/*
	printf("*******%s*****lParam=%d**ptzpanspeed=%d  ptzpanodirection=%d ptztitledirection=%d ptztitlespeed=%d\n",__func__,lParam,Status::getinstance()->ptzpanspeed,Status::getinstance()->ptzpanodirection,
		Status::getinstance()->ptztitledirection,Status::getinstance()->ptztitlespeed);
		*/
	if(lParam==Status::PTZPANOMOV||lParam==Status::PTZTWOMOV)
		{
			if(Status::getinstance()->ptzpanodirection==0)
					PlantformContrl->MakeMove(&instance->PELCO_D, PTZ_MOVE_Stop,Status::getinstance()->ptzpanspeed,true, instance->address);
			else if(Status::getinstance()->ptzpanodirection==1)
				     PlantformContrl->MakeMove(&instance->PELCO_D, PTZ_MOVE_Left,Status::getinstance()->ptzpanspeed,true, instance->address);
			else if(Status::getinstance()->ptzpanodirection==2)
				     PlantformContrl->MakeMove(&instance->PELCO_D, PTZ_MOVE_Right,Status::getinstance()->ptzpanspeed,true, instance->address);
			
			OSA_mutexLock(&instance->lock);
			instance->Uart.UartSend(instance->fd,( unsigned char *) &instance->PELCO_D, SENDLEN);
			OSA_mutexUnlock(&instance->lock);
			OSA_waitMsecs(50);
		}
	if(lParam==Status::PTZTITLEMOV||lParam==Status::PTZTWOMOV)
		{
			if(Status::getinstance()->ptztitledirection==0)
					PlantformContrl->MakeMove(&instance->PELCO_D, PTZ_MOVE_Stop,Status::getinstance()->ptztitlespeed,true, instance->address);
			else if(Status::getinstance()->ptztitledirection==1)
				     PlantformContrl->MakeMove(&instance->PELCO_D, PTZ_MOVE_Up,Status::getinstance()->ptztitlespeed,true, instance->address);
			else if(Status::getinstance()->ptztitledirection==2)
				     PlantformContrl->MakeMove(&instance->PELCO_D, PTZ_MOVE_Down,Status::getinstance()->ptztitlespeed,true, instance->address);
			OSA_mutexLock(&instance->lock);
			instance->Uart.UartSend(instance->fd,( unsigned char *) &instance->PELCO_D, SENDLEN);
			OSA_mutexUnlock(&instance->lock);
			OSA_waitMsecs(10);
		}

	



}
void Plantformpzt::focuscontrl(long lParam)
{
	if(lParam==Status::PTZFOCUSSTOP)
			;
		//PlantformContrl->MakeFocusStop(&instance->PELCO_D, instance->address);
	else if(lParam==Status::PTZFOCUSSNEAR)
			;
		//PlantformContrl->MakeFocusNear(&instance->PELCO_D, instance->address);
	else if(lParam==Status::PTZFOCUSSFAR)
			;
		//PlantformContrl->MakeFocusFar(&instance->PELCO_D, instance->address);


}
void Plantformpzt::iriscontrl(long lParam)
{
	if(lParam==Status::PTZIRISSTOP)
		;
		//	PlantformContrl->MakeFocusStop(&instance->PELCO_D, instance->address);
	else if(lParam==Status::PTZIRISDOWN)
		;//	PlantformContrl->MakeFocusNear(&instance->PELCO_D, instance->address);
	else if(lParam==Status::PTZIRISUP)
		;//	PlantformContrl->MakeFocusFar(&instance->PELCO_D, instance->address);


}

void Plantformpzt::plantfromcontrl(long lParam)
{
	int ptzaddress=Status::getinstance()->ptzaddress;
	int protocal=Status::getinstance()->ptzprotocal;
	int brudrate=Status::getinstance()->ptzbaudrate;
	int speed=Status::getinstance()->ptzspeed;
	OSA_printf("the ptzaddress=%d protocal=%d brudrate=%d speed=%d\n",ptzaddress,protocal,brudrate,speed);
	

	if(protocal==0)
	PlantformContrl=IPelcoFactory::createIpelco(pelco_D);
	else
	PlantformContrl=IPelcoFactory::createIpelco(pelco_P);

	OSA_mutexLock(&instance->lock);
	if(instance->fd!=0)
		instance->Uart.UartClose(instance->fd);
	instance->fd=instance->Uart.UartOpen(UART422NAME);
	instance->Uart.UartSet(instance->fd, instance->Boardrate[brudrate], 8, 'n', 1);
	instance->address=ptzaddress;
	OSA_mutexUnlock(&instance->lock);
	
	
	

}


void Plantformpzt::plantformcontrluninit()
{
	Uart.UartClose(fd);

}
