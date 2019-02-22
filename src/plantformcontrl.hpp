/*
 * plantformcontrl.hpp
 *
 *  Created on: 2018年10月25日
 *      Author: wj
 */

#ifndef PLANTFORMCONTRL_HPP_
#define PLANTFORMCONTRL_HPP_
#include "pelcoFactory.h"
#include "pelcoBase.h"
#include "CUartBase.hpp"
#include <osa_thr.h>
#include"osa_sem.h"
#include"osa_mutex.h"
#include"config.hpp"

typedef struct {
	bool bTrack;
	bool bMtd;
	bool bBlobDetect;
	int chId;
	int iTrackStat;
	
	//Mat frame;
}MAIN_plantfromThrObj_cxt;

typedef struct {
	MAIN_plantfromThrObj_cxt cxt[2];
	OSA_ThrHndl		thrHandleProc;
	OSA_SemHndl	procNotifySem;
	int pp;
	bool bFirst;
	volatile bool	exitProcThread;
	bool						initFlag;
	void 						*pParent;
}MAIN_plantfromThrObj;


typedef struct  {
	int  devId;
	int  recvLen;
	int  sendLen;
	//eUartStat uartStat;
	//int  devPortType;		// Enum_MuxPortType

	
	Bool tskLoop;
	Bool istskStopDone;

	UInt8* sendBuf;
	UInt8* recvBuf;
}ComObj;

typedef void ( *PLANTFORMNOTIFYFUNClk)(void *contex);

class Plantformpzt
{


	enum{
		PLANTFORMGETPAN,
		PLANTFORMGETTITLE,
		PLANTFORMINITPAN,
		PLANTFORMINITTITLE,
		PLANTFORMGETANGEL,
		PLANTFORMGETCALLBACK,
		PLANTFORMPANFOREVER,
		PLANTFORMTITLEFOREVER,
		PLANTSCAN,
		PLANTFORMMAX,


		};


	
	#define SENDLEN (7)
	public:
	enum{
		RENDERPANO,
		RENDERSIGNALPANO,
		PRESETGO,
		MVDETECTGO,
		PLANTFORPANOMMAX,
		};
		static Plantformpzt *getinstance();
		void create();
		void destery();
		void registcall(PLANTFORMNOTIFYFUNClk fun,int index);
		void Enbalecallback(int index,double pan,double title);
		PLANTFORMNOTIFYFUNClk callback[PLANTFORMMAX];
		int callbackeable[PLANTFORMMAX];
		double callbackpan[PLANTFORMMAX];
		double callbacktitle[PLANTFORMMAX];
		double getpanangle(){return plantformpan;};
		double gettitleangle(){return plantformtitle;};

		int getpanopanforeverstatus();
		int getpanotitleforeverstatus();
	private:
		Plantformpzt();
		~Plantformpzt();
		static Plantformpzt *instance;

		void registorfun();
		int COMCTRL_lookupSync(ComObj*pObj);
		int COMCTRL_checkSum(ComObj* pObj);

	public:
		static void ptzcontrl(long lParam);
		static void focuscontrl(long lParam);
		static void iriscontrl(long lParam);

		static void plantfromcontrl(long lParam);


	public:
		void setspeed(int speed)
			{
				speedpan=speedtitle=speed;
			}

	private:
		CUartBase Uart;
		int speedpan;
		int speedtitle;
		
		double titlpanangle;
		ComObj platformcom;
		unsigned char recvbuf[2000];
		unsigned char sendbuf[300];
		int scanflag;

		int fd;
		#define TIMEOUTCOUNT (5)
		

		//unsigned char recvbuff[2000];

		PELCO_D_REQPKT PELCO_D;
		int mainloop ;
		int address;
		int ptzpd;

		OSA_MutexHndl lock;

		double plantformpan;
		double plantformtitle;

		double plantformpanforever;
		double plantformtitleforever;

		double panangle;
		double titleangle;


		OSA_SemHndl	procNotifySem;

		int calibration ;


		MAIN_plantfromThrObj	mainRecvThrObj;
		MAIN_plantfromThrObj	mainContrlThrdetectObj;

		double testangle=0;


		void main_Recv_func();
		int MAIN_threadRecvCreate(void);
		int MAIN_threadRecvDestroy(void);
		static void *mainRecvTsk(void *context)
		{
			MAIN_plantfromThrObj  * pObj= (MAIN_plantfromThrObj*) context;
			if(pObj==NULL)
				{

				printf("++++++++++++++++++++++++++\n");

				}
			Plantformpzt *ctxHdl = (Plantformpzt *) pObj->pParent;
			ctxHdl->main_Recv_func();
			
			return NULL;
		}

		void main_contrl_func();
		int MAIN_contrlthreadCreate(void);
		int MAIN_contrlthreadDestroy(void);
		static void *maincontrlTsk(void *context)
		{
			MAIN_plantfromThrObj  * pObj= (MAIN_plantfromThrObj*) context;
			if(pObj==NULL)
				{

				printf("++++++++++++++++++++++++++\n");

				}
			Plantformpzt *ctxHdl = (Plantformpzt *) pObj->pParent;
			ctxHdl->main_contrl_func();
			
			return NULL;
		}
	public:
		#define TIMEOUT (1000)
		int timeout[PLANTFORMMAX];
		int timeoutflag[PLANTFORMMAX];
		int plantinitflag;
		const int Boardrate[4]={2400,4800,9600,19200};


	public :
		int getplantinitflag(){return plantinitflag;};
		void milliseconds_sleep(unsigned long mSec);
		void plantformcontrluninit();
		void plantformcontrlinit();
		void setpanoscan();
		void setpanoscanstop();
		void setpanopanpos(double value);
		void setpanotitlepos(double value);
		void getpanopanpos();
		void getpanotitlepos();
		double getpanopan();
		double getpanotitle();
		void initptzpos(double pan,double title);
		void setplantformcalibration(int flag);
		int  getplantformcalibration();
		void setpanoantiscan();
		void setpanopanforever(double value);
		void setpanotitleforever(double value);
		unsigned char chechsum(unsigned char *pelcodbuf);
};
#endif /* PLANTFORMCONTRL_HPP_ */
