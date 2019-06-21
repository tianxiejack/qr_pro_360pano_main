#ifndef __IPCCTL_H_
#define __IPCCTL_H_

#define PARAMLEN 164

typedef enum 
{
	IPC_Class_INVALID,
	IPC_Class_MSG,  
	IPC_Class_SHA,
	IPC_Class_SEM,
	IPC_ClassMAX
}IPC_Class;

typedef struct{
	char name[256];
	int Identify;
	int Class;
	int IPCID;
	int RWmode;
	int length;
	void* ptr;	
}IPC_Handl_t;

typedef struct{
    unsigned char cmd_ID;
	unsigned char param[PARAMLEN];
}SENDST;


int Ipc_create(IPC_Handl_t * in); //success: 0 , error occur : -1
void Ipc_destory();
int ipc_sendmsg(int HandlID, SENDST* Param);
int ipc_recvmsg(int HandlID ,SENDST* Param);
void ipc_status_P();
void ipc_status_V();
int sharedMemoryLock(int shmid);
int sharedMemoryUnlock(int shmid);

void *ipc_getSharedAddress(int index);

//used to compability the old version
//int ipc_sendmsg(SENDST* Param,int HandlID);
//int ipc_recvmsg(SENDST* Param,int HandlID);

#endif
