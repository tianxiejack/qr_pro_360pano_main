/*
 * FileCapture.hpp
 *
 *  Created on: 2018年11月22日
 *      Author: wj
 */

#ifndef FILECAPTURE_HPP_
#define FILECAPTURE_HPP_
#include <osa_thr.h>
#include"osa_sem.h"
#include"config.hpp"
#include <opencv/cv.hpp>
#include <opencv2/opencv.hpp>
typedef struct {
	bool bTrack;
	bool bMtd;
	bool bBlobDetect;
	int chId;
	int iTrackStat;
	
	//Mat frame;
}MAIN_FileCaptureThrObj_cxt;

typedef struct {
	MAIN_FileCaptureThrObj_cxt cxt[2];
	OSA_ThrHndl		thrHandleProc;
	OSA_SemHndl	procNotifySem;
	int pp;
	bool bFirst;
	volatile bool	exitProcThread;
	bool						initFlag;
	void 						*pParent;
}MAIN_FileCaptureThrObj;


typedef void (* CallBackfun)(void *data,void *info);

class FileCapture{
	
public:
	FileCapture();
	~FileCapture();
	void create();
	void destory();
	void registcallback(CallBackfun fun);
	void *readfilepicture(void *info);
private:
	//void capturecallback(void *data,void *info);
	CallBackfun capturecallback;
	double captureangle;
	int blockid;
	int fieldid;
	cv::Mat capture;
	
public:
	
		MAIN_FileCaptureThrObj	mainRecvThrObj;
		void main_Recv_func();
		int MAIN_threadRecvCreate(void);
		int MAIN_threadRecvDestroy(void);
		static void *mainRecvTsk(void *context)
		{
			MAIN_FileCaptureThrObj  * pObj= (MAIN_FileCaptureThrObj*) context;
			if(pObj==NULL)
				{

				printf("++++++++++++++++++++++++++\n");

				}
			FileCapture *ctxHdl = (FileCapture *) pObj->pParent;
			ctxHdl->main_Recv_func();
			
			return NULL;
		}


public:
};



#endif /* FILECAPTURE_HPP_ */
