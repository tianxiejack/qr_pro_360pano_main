#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "osa.h"
#include "pelcoPformat.h"

void IPelcoPFormat::MakeSumCheck(pPelco Ppkt)
{
	LPPELCO_P_REQPKT pPkt = (LPPELCO_P_REQPKT) Ppkt;

	if(pPkt == NULL)
		return ;
	UINT8 *pBuf = (UINT8 *)pPkt;
	pPkt->bySTX = PELCO_P_STX;
	pPkt->byETX = PELCO_P_ETX;
	pPkt->byCheckSum = 0;
	for(int i = 0; i < sizeof(PELCO_P_REQPKT) - 1 ; i++){
		pPkt->byCheckSum ^= pBuf[i];
	}
}

void IPelcoPFormat::PktFormat(pPelco Ppkt, UINT8 cmd1, UINT8 cmd2, UINT8 data1, UINT8 data2, UINT8 byAddress /* = 0 */)
{
	LPPELCO_P_REQPKT pPkt = (LPPELCO_P_REQPKT) Ppkt;

	if(pPkt == NULL)
		return ;
	pPkt->byAddress = byAddress;
	pPkt->byCmd1 = cmd1;
	pPkt->byCmd2 = cmd2;
	pPkt->byData1 = data1;
	pPkt->byData2 = data2;
	MakeSumCheck(pPkt);
}

void IPelcoPFormat::MakeCameraOn(pPelco Ppkt, UINT8 byAddress /* = 0 */)
{
	LPPELCO_P_REQPKT pPkt = (LPPELCO_P_REQPKT) Ppkt;

	if(pPkt == NULL)
		return ;
	pPkt->byAddress = byAddress;
	pPkt->byCmd1 = 0x88;
	pPkt->byCmd2 = 0;
	pPkt->byData1 = 0;
	pPkt->byData2 = 0;
	MakeSumCheck(pPkt);
}

void IPelcoPFormat::MakeCameraOff(pPelco Ppkt, UINT8 byAddress /* = 0 */)
{
	LPPELCO_P_REQPKT pPkt = (LPPELCO_P_REQPKT) Ppkt;
	if(pPkt == NULL)
		return ;
	pPkt->byAddress = byAddress;
	pPkt->byCmd1 = 0x08;
	pPkt->byCmd2 = 0;
	pPkt->byData1 = 0;
	pPkt->byData2 = 0;
	MakeSumCheck(pPkt);
}


void IPelcoPFormat::MakeMove(pPelco Ppkt, INT32 iDirection, UINT8 bySpeed /* = 0x20 */, bool bClear /* = TRUE */, UINT8 byAddress /* = 0 */)
{
	LPPELCO_P_REQPKT pPkt = (LPPELCO_P_REQPKT) Ppkt;
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
void IPelcoPFormat::MakeFocusFar(pPelco Ppkt, UINT8 byAddress /* = 0 */)
{
	LPPELCO_P_REQPKT pPkt = (LPPELCO_P_REQPKT) Ppkt;
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
	pPkt->tCmd1.FocusFar = 1;
	MakeSumCheck(pPkt);
}
void IPelcoPFormat::MakeFocusNear(pPelco Ppkt, UINT8 byAddress /* = 0 */)
{
	LPPELCO_P_REQPKT pPkt = (LPPELCO_P_REQPKT) Ppkt;
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
	pPkt->tCmd1.FocusFar = 0;
	MakeSumCheck(pPkt);
}
void IPelcoPFormat::MakeFocusStop(pPelco Ppkt, UINT8 byAddress /* = 0 */)
{
	LPPELCO_P_REQPKT pPkt = (LPPELCO_P_REQPKT) Ppkt;
	if(pPkt == NULL)
		return ;
	pPkt->byAddress = byAddress;
	pPkt->byCmd1 = 0;
	MakeSumCheck(pPkt);
}
void IPelcoPFormat::MakeIrisOpen(pPelco Ppkt, UINT8 byAddress /* = 0 */)
{
	LPPELCO_P_REQPKT pPkt = (LPPELCO_P_REQPKT) Ppkt;
	if(pPkt == NULL)
		return ;
	pPkt->byAddress = byAddress;
	pPkt->byCmd1 = 0;
	pPkt->tCmd1.IrisOpen = 1;
	MakeSumCheck(pPkt);
}
void IPelcoPFormat::MakeIrisClose(pPelco Ppkt, UINT8 byAddress /* = 0 */)
{
	LPPELCO_P_REQPKT pPkt = (LPPELCO_P_REQPKT) Ppkt;
	if(pPkt == NULL)
		return ;
	pPkt->byAddress = byAddress;
	pPkt->byCmd1 = 0;
	pPkt->tCmd1.IrisClose = 1;
	MakeSumCheck(pPkt);
}
void IPelcoPFormat::MakeZoomWide(pPelco Ppkt, UINT8 byAddress /* = 0 */)
{
	LPPELCO_P_REQPKT pPkt = (LPPELCO_P_REQPKT) Ppkt;
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
void IPelcoPFormat::MakeZoomTele(pPelco Ppkt, UINT8 byAddress /* = 0 */)
{
	LPPELCO_P_REQPKT pPkt = (LPPELCO_P_REQPKT) Ppkt;
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
void IPelcoPFormat::MakeZoomStop(pPelco Ppkt, UINT8 byAddress /* = 0 */)
{
	LPPELCO_P_REQPKT pPkt = (LPPELCO_P_REQPKT) Ppkt;
	if(pPkt == NULL)
		return ;
	pPkt->byAddress = byAddress;
	pPkt->tCmd2.ZoomWide = 0;
	pPkt->tCmd2.ZoomTele = 0;
	MakeSumCheck(pPkt);
}
void IPelcoPFormat::MakeAutoScan(pPelco Ppkt, UINT8 byAddress /* = 0 */)
{
	LPPELCO_P_REQPKT pPkt = (LPPELCO_P_REQPKT) Ppkt;
	if(pPkt == NULL)
		return ;
	pPkt->byAddress = byAddress;
	pPkt->byCmd1 = 0;
	pPkt->tCmd1.AutoScan = 1;
	pPkt->byCmd2 = 0;
	MakeSumCheck(pPkt);
}
void IPelcoPFormat::MakePresetCtrl(pPelco Ppkt, INT32 nCtrlType, UINT8 byValue, UINT8 byAddress /* = 0 */)
{
	LPPELCO_P_REQPKT pPkt = (LPPELCO_P_REQPKT) Ppkt;
	if(pPkt == NULL)
		return ;
	pPkt->byAddress = byAddress;
	pPkt->byCmd1 = 0x00;
	pPkt->byCmd2 = nCtrlType;
	pPkt->byData1 = 0;
	pPkt->byData2 = byValue;
	MakeSumCheck(pPkt);
}
void IPelcoPFormat::MakeSetZoomSpeed(pPelco Ppkt, UINT8 byValue, UINT8 byAddress /* = 0 */)
{
	LPPELCO_P_REQPKT pPkt = (LPPELCO_P_REQPKT) Ppkt;
	if(pPkt == NULL)
		return ;
	pPkt->byAddress = byAddress;
	pPkt->byCmd1 = 0x00;
	pPkt->byCmd2 = 0x25;
	pPkt->byData1 = 0;
	pPkt->byData2 = byValue;
	MakeSumCheck(pPkt);
}
void IPelcoPFormat::MakeSetFocusSpeed(pPelco Ppkt, UINT8 byValue, UINT8 byAddress /* = 0 */)
{
	LPPELCO_P_REQPKT pPkt = (LPPELCO_P_REQPKT) Ppkt;
	if(pPkt == NULL)
		return ;
	pPkt->byAddress = byAddress;
	pPkt->byCmd1 = 0x00;
	pPkt->byCmd2 = 0x27;
	pPkt->byData1 = 0;
	pPkt->byData2 = byValue;
	MakeSumCheck(pPkt);
}
void IPelcoPFormat::MakeRemoteReset(pPelco Ppkt, UINT8 byAddress /* = 0 */)
{
	LPPELCO_P_REQPKT pPkt = (LPPELCO_P_REQPKT) Ppkt;
	if(pPkt == NULL)
		return ;
	pPkt->byAddress = byAddress;
	pPkt->byCmd1 = 0x00;
	pPkt->byCmd2 = 0x0F;
	pPkt->byData1 = 0;
	pPkt->byData2 = 0;
	MakeSumCheck(pPkt);
}
void IPelcoPFormat::MakeExtCommand(pPelco Ppkt, INT32 nCmdType, UINT8 byValue, UINT8 byDataExt /* = 0 */, UINT8 byAddress /* = 0 */)
{
	LPPELCO_P_REQPKT pPkt = (LPPELCO_P_REQPKT) Ppkt;
	if(pPkt == NULL)
		return ;
	pPkt->byAddress = byAddress;
	pPkt->byCmd1 = 0x00;
	pPkt->byCmd2 = nCmdType;
	pPkt->byData1 = byDataExt;
	pPkt->byData2 = byValue;
	MakeSumCheck(pPkt);
}

void IPelcoPFormat::MakeDummy(pPelco Ppkt, UINT8 byAddress)
{
	LPPELCO_P_REQPKT pPkt = (LPPELCO_P_REQPKT) Ppkt;
	cout<<"P-> No Func"<<endl;
}

void IPelcoPFormat::MakeSetPanPos(pPelco Ppkt, UINT16 byValue, UINT8 byAddress /* = 0 */)
{
	LPPELCO_P_REQPKT pPkt = (LPPELCO_P_REQPKT) Ppkt;
	cout<<"P-> No Func"<<endl;
}

void IPelcoPFormat::MakeSetTilPos(pPelco Ppkt, UINT16 byValue, UINT8 byAddress /* = 0 */)
{
	LPPELCO_P_REQPKT pPkt = (LPPELCO_P_REQPKT) Ppkt;
	cout<<"P-> No Func"<<endl;
}
void IPelcoPFormat::MakePatternCtrl(pPelco Ppkt, INT32 nCtrlType, UINT8 byValue, UINT8 byAddress)
{
	LPPELCO_P_REQPKT pPkt = (LPPELCO_P_REQPKT) Ppkt;
	cout<<"P-> No Func"<<endl;
}


