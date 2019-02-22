#include"FileSave.hpp"



FileSave::FileSave()
{

}

FileSave::~FileSave()
{

}

void FileSave::main_Recv_func()
{
	OSA_printf("%s: Main Proc Tsk Is Entering...\n",__func__);
	
	while(mainRecvThrObj.exitProcThread ==  false)
	{

	}

}


int FileSave::MAIN_threadRecvCreate(void)
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


int FileSave::MAIN_threadRecvDestroy(void)
{
	int iRet = OSA_SOK;

	mainRecvThrObj.exitProcThread = true;
	OSA_semSignal(&mainRecvThrObj.procNotifySem);

	iRet = OSA_thrDelete(&mainRecvThrObj.thrHandleProc);

	mainRecvThrObj.initFlag = false;
	OSA_semDelete(&mainRecvThrObj.procNotifySem);

	return iRet;
}
