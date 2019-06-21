
#ifndef IPC_CUSTOM_HEAD_HPP_
#define IPC_CUSTOM_HEAD_HPP_

#ifdef __cplusplus
extern "C"{
#endif

#include "Ipcctl.h"
#include "stdlib.h"
#include <string.h>

#define CFGID_FEILD_MAX	16
#define CFGID_BLOCK_MAX	128
#define CFGID_USEROSD_MAX	32
#define USEROSD_LENGTH	128

#define DEFAULTPATH "/"

typedef enum 
{
	a,  /*0*/
	b,
	c,
}CMD_ID;

typedef enum 
{
    IPC_1 = 0,  	// SERVER TO CLIENT
    IPC_2,		// CLIENT TO SERCER
	IPC_IMG_SHA,
	IPC_USER_SHA,
	IPC_SEM,
    IPC_MAX
}IPC_MTYPE;

typedef enum
{
	shm_rdonly,
	shm_rdwr
}shm_perms;

typedef struct
{
	unsigned int intPrm[PARAMLEN/4];
}IPC_PRM_INT;

static IPC_Handl_t * tmpIpc;

static void Ipc_init()
{
	tmpIpc = (IPC_Handl_t*)malloc(IPC_MAX* sizeof(IPC_Handl_t));
	memset(tmpIpc,0,sizeof(IPC_Handl_t)*IPC_MAX);
	char tmp[256] = {"/home/"};
	tmpIpc[0].IPCID = IPC_MAX;
	
	memcpy(tmpIpc[IPC_1].name,tmp,sizeof(tmp));
	tmpIpc[IPC_1].Identify = IPC_1;
	tmpIpc[IPC_1].Class = IPC_Class_MSG;
	tmpIpc[IPC_1].IPCID = IPC_MAX;
	tmpIpc[IPC_1].length = 0;
	tmpIpc[IPC_1].ptr = NULL;
	
	memcpy(tmpIpc[IPC_2].name,tmp,sizeof(tmp));
	tmpIpc[IPC_2].Identify = IPC_2;
	tmpIpc[IPC_2].Class = IPC_Class_MSG;
	tmpIpc[IPC_2].IPCID = IPC_MAX;
	tmpIpc[IPC_2].length = 0;
	tmpIpc[IPC_2].ptr = NULL;

	return;
}


static void *ipc_getSharedMem(IPC_MTYPE itype)
{
	if(itype == IPC_IMG_SHA || itype == IPC_USER_SHA)
		return ipc_getSharedAddress(itype);
		//return (void*)IpcHandl[itype].ptr;
	else
		return NULL;
}

#ifdef __cplusplus
}
#endif

#endif /* IPC_CUSTOM_HEAD_HPP_ */
