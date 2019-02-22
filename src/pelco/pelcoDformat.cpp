#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "osa.h"
#include "pelcoDformat.h"


void IPelcoDFormat::MakeSumCheck(pPelco Ppkt)
{
	LPPELCO_D_REQPKT pPkt = (LPPELCO_D_REQPKT) Ppkt;

	if(pPkt == NULL)
		return ;
	UINT8 *pBuf = (UINT8 *)pPkt;
	pPkt->bySynchByte = PELCO_D_SYNC;
	pPkt->byCheckSum = 0;
	pPkt->byCheckSum += pBuf[1];
	pPkt->byCheckSum += pBuf[2];
	pPkt->byCheckSum += pBuf[3];
	pPkt->byCheckSum += pBuf[4];
	pPkt->byCheckSum += pBuf[5];
}

void IPelcoDFormat::PktFormat(pPelco Ppkt, UINT8 cmd1, UINT8 cmd2, UINT8 data1, UINT8 data2, UINT8 byAddress /* = 0 */)
{
	LPPELCO_D_REQPKT pPkt = (LPPELCO_D_REQPKT) Ppkt;
	if(pPkt == NULL)
		return ;
	pPkt->byAddress = byAddress;
	pPkt->byCmd1 = cmd1;
	pPkt->byCmd2 = cmd2;
	pPkt->byData1 = data1;
	pPkt->byData2 = data2;
	MakeSumCheck(pPkt);
}

void IPelcoDFormat::MakeCameraOn(pPelco Ppkt, UINT8 byAddress /* = 0 */)
{
	LPPELCO_D_REQPKT pPkt = (LPPELCO_D_REQPKT) Ppkt;
	if(pPkt == NULL)
		return ;
	pPkt->byAddress = byAddress;
	pPkt->byCmd1 = 0x88;
	pPkt->byCmd2 = 0;
	pPkt->byData1 = 0;
	pPkt->byData2 = 0;
	MakeSumCheck(pPkt);
}

void IPelcoDFormat::MakeCameraOff(pPelco Ppkt, UINT8 byAddress /* = 0 */)
{
	LPPELCO_D_REQPKT pPkt = (LPPELCO_D_REQPKT) Ppkt;
	if(pPkt == NULL)
		return ;
	pPkt->byAddress = byAddress;
	pPkt->byCmd1 = 0x08;
	pPkt->byCmd2 = 0;
	pPkt->byData1 = 0;
	pPkt->byData2 = 0;
	MakeSumCheck(pPkt);
}
void IPelcoDFormat::MakeMove(pPelco Ppkt, INT32 iDirection, UINT8 bySpeed /* = 0x20 */, bool bClear /* = TRUE */, UINT8 byAddress /* = 0 */)
{
	LPPELCO_D_REQPKT pPkt = (LPPELCO_D_REQPKT) Ppkt;
	if(pPkt == NULL)
		return ;
	pPkt->byAddress = byAddress;
	pPkt->byCmd1 = 0;
	if(bClear){
		pPkt->byCmd2 = 0;
		pPkt->byData1 = 0;
		pPkt->byData2 = 0;
	}
	if( (pPkt->byCmd2 & 0xE1) != 0
		|| (pPkt->tCmd2.Left == 1 && pPkt->tCmd2.Right == 1)
		|| (pPkt->tCmd2.Up == 1 && pPkt->tCmd2.Down == 1) )
	{
		pPkt->byCmd2 = 0;
	}

	switch(iDirection)
	{
	case PTZ_MOVE_Stop:
		pPkt->byCmd2 = 0;
		pPkt->byData1 = 0;
		pPkt->byData2 = 0;
		break;
	case PTZ_MOVE_Up:
		pPkt->tCmd2.Up = 1;
		pPkt->tCmd2.Down = 0;
		pPkt->byData2 = bySpeed;
		break;
	case PTZ_MOVE_Down:
		pPkt->tCmd2.Up = 0;
		pPkt->tCmd2.Down = 1;
		pPkt->byData2 = bySpeed;
		break;
	case PTZ_MOVE_Left:
		pPkt->tCmd2.Left = 1;
		pPkt->tCmd2.Right = 0;
		pPkt->byData1 = bySpeed;
		break;
	case PTZ_MOVE_Right:
		pPkt->tCmd2.Left = 0;
		pPkt->tCmd2.Right = 1;
		pPkt->byData1 = bySpeed;
		break;
	}
	MakeSumCheck(pPkt);
}

void IPelcoDFormat::MakeFocusFar(pPelco Ppkt, UINT8 byAddress /* = 0 */)
{
	LPPELCO_D_REQPKT pPkt = (LPPELCO_D_REQPKT) Ppkt;
	if(pPkt == NULL)
		return ;
	pPkt->byAddress = byAddress;
	if( (pPkt->byCmd2 & 0xE1) != 0
		|| (pPkt->tCmd2.Left == 1 && pPkt->tCmd2.Right == 1)
		|| (pPkt->tCmd2.Up == 1 && pPkt->tCmd2.Down == 1) )
	{
		pPkt->byCmd2 = 0;
	}
	pPkt->tCmd1.FocusNear = 0;
	pPkt->tCmd2.FocusFar = 1;
	MakeSumCheck(pPkt);
}
void IPelcoDFormat::MakeFocusNear(pPelco Ppkt, UINT8 byAddress /* = 0 */)
{
	LPPELCO_D_REQPKT pPkt = (LPPELCO_D_REQPKT) Ppkt;
	if(pPkt == NULL)
		return ;
	pPkt->byAddress = byAddress;
	if( (pPkt->byCmd2 & 0xE1) != 0
		|| (pPkt->tCmd2.Left == 1 && pPkt->tCmd2.Right == 1)
		|| (pPkt->tCmd2.Up == 1 && pPkt->tCmd2.Down == 1) )
	{
		pPkt->byCmd2 = 0;
	}
	pPkt->tCmd1.FocusNear = 1;
	pPkt->tCmd2.FocusFar = 0;
	MakeSumCheck(pPkt);
}
void IPelcoDFormat::MakeFocusStop(pPelco Ppkt, UINT8 byAddress /* = 0 */)
{
	LPPELCO_D_REQPKT pPkt = (LPPELCO_D_REQPKT) Ppkt;
	if(pPkt == NULL)
		return ;
	pPkt->byAddress = byAddress;
	pPkt->tCmd1.FocusNear = 0;
	pPkt->tCmd2.FocusFar = 0;
	MakeSumCheck(pPkt);
}
void IPelcoDFormat::MakeIrisOpen(pPelco Ppkt, UINT8 byAddress /* = 0 */)
{
	LPPELCO_D_REQPKT pPkt = (LPPELCO_D_REQPKT) Ppkt;
	if(pPkt == NULL)
		return ;
	pPkt->byAddress = byAddress;
	pPkt->byCmd1 = 0;
	pPkt->tCmd1.IrisOpen = 1;
	MakeSumCheck(pPkt);
}
void IPelcoDFormat::MakeIrisClose(pPelco Ppkt, UINT8 byAddress /* = 0 */)
{
	LPPELCO_D_REQPKT pPkt = (LPPELCO_D_REQPKT) Ppkt;
	if(pPkt == NULL)
		return ;
	pPkt->byAddress = byAddress;
	pPkt->byCmd1 = 0;
	pPkt->tCmd1.IrisClose = 1;
	MakeSumCheck(pPkt);
}
void IPelcoDFormat::MakeZoomWide(pPelco Ppkt, UINT8 byAddress /* = 0 */)
{
	LPPELCO_D_REQPKT pPkt = (LPPELCO_D_REQPKT) Ppkt;
	if(pPkt == NULL)
		return ;
	pPkt->byAddress = byAddress;
	if( (pPkt->byCmd2 & 0xE1) != 0
		|| (pPkt->tCmd2.Left == 1 && pPkt->tCmd2.Right == 1)
		|| (pPkt->tCmd2.Up == 1 && pPkt->tCmd2.Down == 1) )
	{
		pPkt->byCmd2 = 0;
	}
	pPkt->tCmd2.ZoomWide = 1;
	pPkt->tCmd2.ZoomTele = 0;
	MakeSumCheck(pPkt);
}
void IPelcoDFormat::MakeZoomTele(pPelco Ppkt, UINT8 byAddress /* = 0 */)
{
	LPPELCO_D_REQPKT pPkt = (LPPELCO_D_REQPKT) Ppkt;
	if(pPkt == NULL)
		return ;
	pPkt->byAddress = byAddress;
	if( (pPkt->byCmd2 & 0xE1) != 0
		|| (pPkt->tCmd2.Left == 1 && pPkt->tCmd2.Right == 1)
		|| (pPkt->tCmd2.Up == 1 && pPkt->tCmd2.Down == 1) )
	{
		pPkt->byCmd2 = 0;
	}
	pPkt->tCmd2.ZoomWide = 0;
	pPkt->tCmd2.ZoomTele = 1;
	MakeSumCheck(pPkt);
}
void IPelcoDFormat::MakeZoomStop(pPelco Ppkt, UINT8 byAddress /* = 0 */)
{
	LPPELCO_D_REQPKT pPkt = (LPPELCO_D_REQPKT) Ppkt;
	if(pPkt == NULL)
		return ;
	pPkt->byAddress = byAddress;
	pPkt->tCmd2.ZoomWide = 0;
	pPkt->tCmd2.ZoomTele = 0;
	MakeSumCheck(pPkt);
}
void IPelcoDFormat::MakeAutoScan(pPelco Ppkt, UINT8 byAddress /* = 0 */)
{
	LPPELCO_D_REQPKT pPkt = (LPPELCO_D_REQPKT) Ppkt;
	if(pPkt == NULL)
		return ;
	pPkt->byAddress = byAddress;
	pPkt->byCmd1 = 0;
	pPkt->tCmd1.AutoScan = 1;
	pPkt->byCmd2 = 0;
	MakeSumCheck(pPkt);
}

//////////////////////////////////////////////////////////////////////////
//Set Preset 00 03 00 01 to 20
void IPelcoDFormat::MakePresetCtrl(pPelco Ppkt, INT32 nCtrlType, UINT8 byValue, UINT8 byAddress /* = 0 */)
{
	LPPELCO_D_REQPKT pPkt = (LPPELCO_D_REQPKT) Ppkt;
	if(pPkt == NULL)
		return ;
	pPkt->byAddress = byAddress;
	pPkt->byCmd1 = 0;
	pPkt->byCmd2 = nCtrlType;
	pPkt->byData1 = 0;
	pPkt->byData2 = byValue;
	MakeSumCheck(pPkt);
}
//////////////////////////////////////////////////////////////////////////
//Set Pattern 00 03 00 01 to 20
void IPelcoDFormat::MakePatternCtrl(pPelco Ppkt, INT32 nCtrlType, UINT8 byValue, UINT8 byAddress /* = 0 */)
{
	LPPELCO_D_REQPKT pPkt = (LPPELCO_D_REQPKT) Ppkt;
	if(pPkt == NULL)
		return ;
	pPkt->byAddress = byAddress;
	pPkt->byCmd1 = 0;
	pPkt->byCmd2 = nCtrlType;
	pPkt->byData1 = 0;
	pPkt->byData2 = byValue;
	MakeSumCheck(pPkt);
}
//Remote Reset 00 0F 00 00
void IPelcoDFormat::MakeRemoteReset(pPelco Ppkt, UINT8 byAddress /* = 0 */)
{
	LPPELCO_D_REQPKT pPkt = (LPPELCO_D_REQPKT) Ppkt;
	if(pPkt == NULL)
		return ;
	pPkt->byAddress = byAddress;
	pPkt->byCmd1 = 0;
	pPkt->byCmd2 = 0x0F;
	pPkt->byData1 = 0;
	pPkt->byData2 = 0;
	MakeSumCheck(pPkt);
}
//Set Zoom Speed 00 25 00 00 to 03
void IPelcoDFormat::MakeSetZoomSpeed(pPelco Ppkt, UINT8 byValue, UINT8 byAddress /* = 0 */)
{
	LPPELCO_D_REQPKT pPkt = (LPPELCO_D_REQPKT) Ppkt;
	if(pPkt == NULL)
		return ;
	pPkt->byAddress = byAddress;
	pPkt->byCmd1 = 0;
	pPkt->byCmd2 = 0x25;
	pPkt->byData1 = 0;
	pPkt->byData2 = byValue;
	MakeSumCheck(pPkt);
	printf("byValue = %d\n", byValue);
}
//Set Focus Speed 00 27 00 00 to 03
void IPelcoDFormat::MakeSetFocusSpeed(pPelco Ppkt, UINT8 byValue, UINT8 byAddress /* = 0 */)
{
	LPPELCO_D_REQPKT pPkt = (LPPELCO_D_REQPKT) Ppkt;
	if(pPkt == NULL)
		return ;
	pPkt->byAddress = byAddress;
	pPkt->byCmd1 = 0;
	pPkt->byCmd2 = 0x27;
	pPkt->byData1 = 0;
	pPkt->byData2 = byValue;
	MakeSumCheck(pPkt);
}
void IPelcoDFormat::MakeExtCommand(pPelco Ppkt, INT32 nCmdType, UINT8 byValue, UINT8 byDataExt /* = 0 */, UINT8 byAddress /* = 0 */)
{
	LPPELCO_D_REQPKT pPkt = (LPPELCO_D_REQPKT) Ppkt;
	if(pPkt == NULL)
		return ;
	pPkt->byAddress = byAddress;
	pPkt->byCmd1 = 0;
	pPkt->byCmd2 = nCmdType;
	pPkt->byData1 = byDataExt;
	pPkt->byData2 = byValue;
	MakeSumCheck(pPkt);
}
void IPelcoDFormat::MakeDummy(pPelco Ppkt, UINT8 byAddress /* = 0 */)
{
	LPPELCO_D_REQPKT pPkt = (LPPELCO_D_REQPKT) Ppkt;
	if(pPkt == NULL)
		return ;
	pPkt->byAddress = byAddress;
	pPkt->byCmd1 = 0;
	pPkt->byCmd2 = 0x0D;
	pPkt->byData1 = 0;
	pPkt->byData2 = 0;
	MakeSumCheck(pPkt);
}



void IPelcoDFormat::MakeSetPanPos(pPelco Ppkt, UINT16 byValue, UINT8 byAddress /* = 0 */)
{
	LPPELCO_D_REQPKT pPkt = (LPPELCO_D_REQPKT) Ppkt;
	if(pPkt == NULL)
		return ;
	pPkt->byAddress = byAddress;
	pPkt->byCmd1 = 0x00;
	pPkt->byCmd2 = 0x4B;

	if(byValue > 35999)
		byValue = 35999;

	pPkt->byData1 = (byValue>>8)&0xff;
	pPkt->byData2 = byValue&0xff;
	MakeSumCheck(pPkt);
}


void IPelcoDFormat::MakeSetTilPos(pPelco Ppkt, UINT16 byValue, UINT8 byAddress /* = 0 */)
{
	LPPELCO_D_REQPKT pPkt = (LPPELCO_D_REQPKT) Ppkt;
	if(pPkt == NULL)
		return ;
	pPkt->byAddress = byAddress;
	pPkt->byCmd1 = 0x00;
	pPkt->byCmd2 = 0x4D;

	if(byValue > 35999)
		byValue = 35999;

	
	pPkt->byData1 = (byValue>>8) & 0xff;
	pPkt->byData2 = byValue & 0xff;
	//printf("byValue = %d     >>8 : %d   data2 :%d \n",byValue,(byValue>>8),(byValue&0xff));
	MakeSumCheck(pPkt);
}
