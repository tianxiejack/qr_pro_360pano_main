

#ifndef _OSA_BUF_H_
#define _OSA_BUF_H_

#include <stdint.h>
#include <osa.h>
#include <osa_que.h>
#include <osa_mutex.h>

#define OSA_BUF_NUM_MAX       (8*4)

#define OSA_BUF_ID_INVALID    (-1)

typedef struct {
  int 		bufferId;
  int 		memtype;
  uint64_t	timestamp;//ns
  uint64_t      timestampCap;
  uint32_t  pbo;
  struct cudaGraphicsResource *resource;
  int 		width;
  int 		height;
  int       channels;
  int 		size;
  unsigned int rotaionstamp;
  double     framegyroyaw;
  double     framegyropitch;
  double     framegyroroll;
  int rotationvalid;
  int currentbufid;
  int       flags;
  int calibration;
  int tailcut;
  
  void 		*physAddr;
  void 		*virtAddr;

  

} OSA_BufInfo;

typedef struct {

  OSA_BufInfo bufInfo[OSA_BUF_NUM_MAX];
  
  OSA_QueHndl emptyQue;
  OSA_QueHndl fullQue;
      
  int numBuf;
  bool bCreate;

} OSA_BufHndl;

typedef struct {

  void *bufPhysAddr[OSA_BUF_NUM_MAX];
  void *bufVirtAddr[OSA_BUF_NUM_MAX];  
  
  int numBuf;
  int width;
  int height;
  int stride;

} OSA_BufCreate;

int  OSA_bufCreate(OSA_BufHndl *hndl, OSA_BufCreate *bufInit);
int  OSA_bufDelete(OSA_BufHndl *hndl);

int  OSA_bufGetFull(OSA_BufHndl *hndl, int *bufId, Uint32 timeout);
int  OSA_bufPutEmpty(OSA_BufHndl *hndl, int bufId);

int  OSA_bufGetEmpty(OSA_BufHndl *hndl, int *bufId, Uint32 timeout);
int  OSA_bufPutFull(OSA_BufHndl *hndl, int bufId);

int  OSA_bufSwitchFull (OSA_BufHndl *hndl, int *bufId);
int  OSA_bufSwitchEmpty(OSA_BufHndl *hndl, int *bufId);

OSA_BufInfo *OSA_bufGetBufInfo(OSA_BufHndl *hndl, int bufId);

int OSA_bufGetEmptyCount(OSA_BufHndl *hndl);
int OSA_bufGetFullCount(OSA_BufHndl *hndl);
int OSA_bufGetBufcount(OSA_BufHndl *hndl,int mod);

#endif /* _OSA_BUF_H_ */



