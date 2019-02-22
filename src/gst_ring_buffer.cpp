#include <iostream>
#include <stdio.h>
#include <gst_ring_buffer.h>
#include <demo_global_def.h>

//using namespace std;


#define MAX_BUF_LEN 0x200000

OSA_BufCreate tskBufCreate;
OSA_BufHndl tsk_Buf;

typedef struct _get_buf_list
{
	char* tsk_get_addr;
	int rngId;
	int size;
}GET_BUF_LIST;

GET_BUF_LIST g_buf_list;

void create_ring_buffer()
{
	/** create sender ringbuf **/
	int status = SDK_SOK;
	int i;
	tskBufCreate.numBuf = OSA_BUF_NUM_MAX;
	for (i = 0; i < tskBufCreate.numBuf; i++)
	{
		tskBufCreate.bufVirtAddr[i] = SDK_MEM_MALLOC(MAX_BUF_LEN);
		OSA_assert(tskBufCreate.bufVirtAddr[i] != NULL);
		memset(tskBufCreate.bufVirtAddr[i], 0, MAX_BUF_LEN);
	}
	status = OSA_bufCreate(&tsk_Buf, &tskBufCreate);
	OSA_assert(status == OSA_SOK);
}


void put_ring_buffer(char *pData, int datalen)
{
	int rngId = 0;
	int rngstat = SDK_EFAIL;
	char *pIn = NULL, *pOut = NULL;
	
	rngstat = OSA_bufGetEmpty(&tsk_Buf, &rngId, SDK_TIMEOUT_NONE/*OSA_TIMEOUT_FOREVER*/);
	if (rngstat != OSA_SOK)
	{
		//OSA_printf(" datarng get failed timeout lostpkt.");
		//OSA_printf("  lostpkt.\n");
		return ;
	}

	pOut = (char *)tsk_Buf.bufInfo[rngId].virtAddr;
	pIn = pData;

	if (pIn != NULL)
		memcpy(pOut, pIn, datalen);
	else
		memset(pOut, 0, datalen);

	//////////////////////////////
	tsk_Buf.bufInfo[rngId].size = datalen;

	OSA_bufPutFull(&tsk_Buf, rngId);
}

int get_ring_buf()
{
	int rngstat = SDK_EFAIL;
	int rngId = 0;
	rngstat = OSA_bufGetFull(&tsk_Buf, &rngId, SDK_TIMEOUT_NONE/*OSA_TIMEOUT_FOREVER*/);
	if(rngstat != OSA_SOK)
	{
		//OSA_printf(" datarng get failed timeout empty.");
		//OSA_printf("   empty. \n");
		return SDK_EFAIL;
	}

	g_buf_list.rngId = rngId;
	g_buf_list.tsk_get_addr = (char *)tsk_Buf.bufInfo[rngId].virtAddr;
	g_buf_list.size = tsk_Buf.bufInfo[rngId].size;

	return OSA_SOK;
}

char* get_ring_buf_ptr()
{
	return g_buf_list.tsk_get_addr;
}

int get_ring_buf_length()
{
	return g_buf_list.size;
}

void get_ring_buf_release()
{
	OSA_bufPutEmpty(&tsk_Buf, g_buf_list.rngId);
}

void destory_ring_buffer( )
{
	int rngId;

	/* stop receive task loop */
	OSA_bufGetEmpty(&tsk_Buf, &rngId, SDK_TIMEOUT_FOREVER);
	tsk_Buf.bufInfo[rngId].size = 0;
	OSA_bufPutFull(&tsk_Buf, rngId);

	int i=0;
	for (i = 0; i < tskBufCreate.numBuf; i++)
	{
		if (tskBufCreate.bufVirtAddr[i] != NULL)
		{
			SDK_MEM_FREE(tskBufCreate.bufVirtAddr[i]);
			tskBufCreate.bufVirtAddr[i] = NULL;
		}
	}

	OSA_printf(" %s done.\r\n", __func__);
	return ;
}

