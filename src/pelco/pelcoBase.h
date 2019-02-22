#ifndef _PELCO_Base_H
#define _PELCO_Base_H

#include"pelcoFormat.h"


class IPelcoBaseFormat
{
public:
	IPelcoBaseFormat(){};
	virtual ~IPelcoBaseFormat(){};
	
	virtual void MakeSumCheck(pPelco Ppkt) = 0;
	virtual void PktFormat(pPelco Ppkt, UINT8 cmd1, UINT8 cmd2, UINT8 data1, UINT8 data2, UINT8 byAddress = 0) = 0;
	virtual void MakeCameraOn(pPelco Ppkt, UINT8 byAddress = 0) = 0;
	virtual void MakeCameraOff(pPelco Ppkt, UINT8 byAddress = 0) = 0;
	virtual void MakeMove(pPelco Ppkt, INT32 iDirection, UINT8 bySpeed = 0x20,
		bool bClear = true, UINT8 byAddress = 0) = 0;
	virtual void MakeFocusFar(pPelco Ppkt, UINT8 byAddress = 0) = 0;
	virtual void MakeFocusNear(pPelco Ppkt, UINT8 byAddress = 0) = 0;
	virtual void MakeFocusStop(pPelco Ppkt, UINT8 byAddress = 0) = 0;
	virtual void MakeIrisOpen(pPelco Ppkt, UINT8 byAddress = 0) = 0;
	virtual void MakeIrisClose(pPelco Ppkt, UINT8 byAddress = 0) = 0;
	virtual void MakeZoomWide(pPelco Ppkt, UINT8 byAddress = 0) = 0;
	virtual void MakeZoomTele(pPelco Ppkt, UINT8 byAddress = 0) = 0;
	virtual void MakeZoomStop(pPelco Ppkt, UINT8 byAddress = 0) = 0;
	virtual void MakeAutoScan(pPelco Ppkt, UINT8 byAddress = 0) = 0;

	// EXTENDED COMMANDS
	virtual void MakePresetCtrl(pPelco Ppkt, INT32 nCtrlType, UINT8 byValue, UINT8 byAddress = 0) = 0;
	virtual void MakePatternCtrl(pPelco Ppkt, INT32 nCtrlType, UINT8 byValue, UINT8 byAddress = 0) = 0;
	virtual void MakeSetZoomSpeed(pPelco Ppkt, UINT8 byValue, UINT8 byAddress = 0) = 0;
	virtual void MakeSetFocusSpeed(pPelco Ppkt, UINT8 byValue, UINT8 byAddress = 0) = 0;
	virtual void MakeRemoteReset(pPelco Ppkt, UINT8 byAddress = 0) = 0;
	virtual void MakeExtCommand(pPelco Ppkt, INT32 nCmdType, UINT8 byValue, UINT8 byDataExt = 0, UINT8 byAddress = 0) = 0;
	virtual void MakeDummy(pPelco Ppkt, UINT8 byAddress = 0) = 0;
	virtual void MakeSetPanPos(pPelco Ppkt, UINT16 byValue, UINT8 byAddress /* = 0 */) = 0;
	virtual void MakeSetTilPos(pPelco Ppkt, UINT16 byValue, UINT8 byAddress /* = 0 */) = 0;


};


#endif
