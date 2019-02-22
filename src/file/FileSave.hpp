/*
 * FileSave.hpp
 *
 *  Created on: 2018年11月22日
 *      Author: wj
 */

#ifndef FILESAVE_HPP_
#define FILESAVE_HPP_
#include <osa_thr.h>
#include"osa_sem.h"
#include"config.hpp"
typedef struct {
	bool bTrack;
	bool bMtd;
	bool bBlobDetect;
	int chId;
	int iTrackStat;
	
	//Mat frame;
}MAIN_FileSaveThrObj_cxt;

typedef struct {
	MAIN_FileSaveThrObj_cxt cxt[2];
	OSA_ThrHndl		thrHandleProc;
	OSA_SemHndl	procNotifySem;
	int pp;
	bool bFirst;
	volatile bool	exitProcThread;
	bool						initFlag;
	void 						*pParent;
}MAIN_FileSaveThrObj;

class FileSave{
public:
	FileSave();
	~FileSave();
public:
	
		MAIN_FileSaveThrObj	mainRecvThrObj;
		void main_Recv_func();
		int MAIN_threadRecvCreate(void);
		int MAIN_threadRecvDestroy(void);
		static void *mainRecvTsk(void *context)
		{
			MAIN_FileSaveThrObj  * pObj= (MAIN_FileSaveThrObj*) context;
			if(pObj==NULL)
				{

				printf("++++++++++++++++++++++++++\n");

				}
			FileSave *ctxHdl = (FileSave *) pObj->pParent;
			ctxHdl->main_Recv_func();
			
			return NULL;
		}


public:
};



#endif /* FILESAVE_HPP_ */
