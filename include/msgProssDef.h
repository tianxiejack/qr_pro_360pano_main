/********************************************************************************
  Copyright (C), 2011-2012, CamRun Tech. Co., Ltd. 
  File name:	msgProssDef.h
  Author:	xavier       Version:  1.0.0      Date: 2013-04-17 
  Description: this file define the globle message process api function.
  				and message id , both form the external interface and internal
  				interface.
  Others:         
  Function List: 
  History:       
    1. Date: 
       Author: 
       Modification: 
    2. ... 
********************************************************************************/

#ifndef _MSG_PROSS_DEF_H_
#define _MSG_PROSS_DEF_H_

//#ifdef __cplusplus
//extern "C"{
//#endif

#include "msgDriv.h"
#include "demo_global_def.h"

void MSGAPI_initial(MSGDRIV_Handle handle);
void MSGAPI_unInitial(MSGDRIV_Handle handle);

void MSGAPI_init_device(LPARAM lParam		/*=NULL*/);
void MSGAPI_reset_device(LPARAM lParam		/*=NULL*/);

void MSGAPI_setSensorStat(LPARAM lParam		/*=NULL*/);
void MSGAPI_setFovStat(LPARAM lParam		/*=NULL*/);

void MSGAPI_saveConfig(LPARAM lParam		/*=NULL*/);
void MSGAPI_setCrossRefine(LPARAM lParam		/*=NULL*/);
void MSGAPI_setTrackStat(LPARAM lParam		/*=NULL*/);
void MSGAPI_setAimRefine(LPARAM lParam			/*=NULL*/);
void MSGAPI_setAimSize(LPARAM lParam	/*=NULL*/);
void MSGAPI_setForceCoast(LPARAM lParam	/*=NULL*/);
void MSGAPI_setTrackAlpha(LPARAM lParam /*=NULL*/);

void MSGAPI_setImgPicp(LPARAM lParam			/*=NULL*/);
void MSGAPI_setImgZoom(LPARAM lParam			/*=NULL*/);
void MSGAPI_setImgEnh(LPARAM lParam			/*=NULL*/);
void MSGAPI_setImgMtd(LPARAM lParam			/*=NULL*/);
void MSGAPI_setImgMtdSelect(LPARAM lParam	/*=NULL*/);

void MSGAPI_setVideoCtrl(LPARAM lParam		/*=NULL*/);

void MSGAPI_shiftToAvt(unsigned int *posx, unsigned int *posy, int sens);
void MSGAPI_shiftFromAvt(int *posx, int *posy, int sens);

//#ifdef __cplusplus
//}
//#endif

#endif
