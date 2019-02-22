#include "CMessage.hpp"
#include <string.h>

CMessage::CMessage()
{
    memset(g_MsgDrvObj.msgTab, 0, sizeof(MSGTAB_Class) * MAX_MSG_NUM);
}
CMessage::CMessage(const CMessage&)
{

}
CMessage& CMessage::operator=(const CMessage&)
{

}

CMessage::~CMessage()
{

}

CMessage *CMessage::instance = new CMessage();
CMessage *CMessage::getInstance()
{
    return instance;
}

MSGDRIV_Handle CMessage::MSGDRIV_create()
{
    int status = SDK_SOK;

    MSGDRIV_Class* pMsgDrvObj = &g_MsgDrvObj;
    memset(pMsgDrvObj, 0, sizeof(MSGDRIV_Class));

    pMsgDrvObj->tskLoop = true;
    pMsgDrvObj->bIntial = true;

    OSA_msgqCreate(&pMsgDrvObj->msgQue);
    status = OSA_thrCreate(&pMsgDrvObj->tskHndl,
        pMSGDRIV_ProcTask,
        OSA_THR_PRI_DEFAULT+10,
        0,
        (void*)this);

    OSA_assert(status == OSA_SOK);
    pMsgDrvObj->istskStopDone = false;
    OSA_printf(" %s done!!! \r\n", __func__);
    return pMsgDrvObj;
}

void CMessage::MSGDRIV_destroy(MSGDRIV_Handle handle)
{
    if (handle != NULL)
    {
        handle->tskLoop = false;
        Uint16 cmd = MSGDRIV_FREE_CMD;
        OSA_msgqSendMsg(&handle->msgQue, &handle->msgQue, cmd, NULL, 0, NULL);

        while (!handle->istskStopDone)
        {
            OSA_waitMsecs(5);
        }

        OSA_thrDelete(&handle->tskHndl);
        OSA_msgqDelete(&handle->msgQue );
        handle = NULL;
    }
    OSA_printf(" %s done!!! \r\n", __func__);
}

void CMessage::MSGDRIV_register(int msgId, MsgApiFun pRtnFun, int context)
{
    assert(msgId < MAX_MSG_NUM);
    g_MsgDrvObj.msgTab[msgId].msgId = msgId;
    g_MsgDrvObj.msgTab[msgId].pRtnFun = pRtnFun;
    g_MsgDrvObj.msgTab[msgId].refContext = context;
}

void CMessage::MSGDRIV_send(int msgId, void *prm)
{
    assert(msgId < MAX_MSG_NUM);
    SDK_MSGQUE_SEND(&g_MsgDrvObj.msgQue, &g_MsgDrvObj.msgQue, msgId, prm, 0, NULL);
}

void* CMessage::MSGDRIV_ProcTask()
{
    MSGDRIV_Class* pMsgDrvObj = &g_MsgDrvObj;
    MSG_ID msg_recv;
    MSGTAB_Class msg;
    unsigned int icou = 0;

    assert(pMsgDrvObj != NULL);
    OSA_printf("%s start. \n", __func__);

    while (pMsgDrvObj->tskLoop)
    {
        icou++;
        int iret = OSA_msgqRecvMsgEx(&pMsgDrvObj->msgQue, &msg_recv, SDK_TIMEOUT_FOREVER);

        if ((iret != SDK_EFAIL) && (msg_recv.cmd != MSGDRIV_FREE_CMD))
        {
            msg.msgId = msg_recv.cmd;
            msg.refContext = (long)msg_recv.pPrm;
            SDK_ASSERT(msg.msgId >= 0 && msg.msgId < MAX_MSG_NUM);

            if (pMsgDrvObj->msgTab[msg.msgId].pRtnFun != NULL)
            {
                pMsgDrvObj->msgTab[msg.msgId].pRtnFun(msg.refContext);
            }
        }
        else if (msg_recv.cmd == MSGDRIV_FREE_CMD)
        {
            OSA_printf(" %s recieve MSGDRIV_FREE_CMD !!! ", __func__);
        }
    }

    pMsgDrvObj->istskStopDone = true;
    OSA_printf(" %s exit. ", __func__);

    return NULL;
}
