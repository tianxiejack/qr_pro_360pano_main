/***************************************************************************************************** 
  Copyright (C), 2011-2012, CamraRun Tech. Co., Ltd. 
  File name:	netMsgDef.h
  Author:	xavier       Version:  1.0.0      Date: 2013-4-17 
  Description:    
  Others:
  Struct list:
  History:       
    1. Date: 
       Author: 
       Modification: 
    2. ... 
******************************************************************************************************/

#ifndef _NET_MSG_DEF_H_
#define _NET_MSG_DEF_H_

typedef enum
{
	eUserData205				= 13,	// 0x0D
	eAVTMain					= 14,	// 0x0E
	eAVTLogdata					= 15,	// 0x0F
	eDebug						= 16,	// 0x10
	eMsg						= 17,	// 0x11
	eUpgradeFirmware    		= 18 	// 0x12
} eFUNC;

#define REPLY_TYPE				1000
#define SOFTWARE_VERSION		0x00010000

typedef enum
{
	eCT_UserData205				= eUserData205,			
	eCT_AVTMain					= eAVTMain,				
	eCT_AVTLogdata				= eAVTLogdata,			
	eCT_Debug					= eDebug,				
	eCT_Msg						= eMsg,					
	eCT_Update        			= eUpgradeFirmware
} eCMD_TYPE;

typedef struct _pve_msg_header{
	unsigned int 	SyFlag;
	unsigned int 	uiVersion;		
	eCMD_TYPE 		eCmdType;			
	unsigned int 	uiRequestId;	
	unsigned int 	uiSize;		// data with msghead and crc
}NET_MSG_HEADER, *PNET_MSG_HEADER;

typedef struct _net_msg{
	NET_MSG_HEADER head;
	union{
		unsigned char data[20];
		int  iReplyValue;
	}UN_MSG;
}NET_MSG, *PNET_MSG;

#endif
