#ifndef PELCODFORMAT_H_
#define PELCODFORMAT_H_

#include"pelcoFormat.h"
#include "pelcoBase.h"

class IPelcoDFormat : public IPelcoBaseFormat
{
public:
	IPelcoDFormat(){}
	~IPelcoDFormat(){}

	void MakeSumCheck(pPelco Ppkt);
	void PktFormat(pPelco Ppkt, UINT8 cmd1, UINT8 cmd2, UINT8 data1, UINT8 data2, UINT8 byAddress = 0);
	void MakeCameraOn(pPelco Ppkt, UINT8 byAddress = 0);
 	void MakeCameraOff(pPelco Ppkt, UINT8 byAddress = 0);
	 void MakeMove(pPelco Ppkt, INT32 iDirection, UINT8 bySpeed = 0x20,
		bool bClear = true, UINT8 byAddress = 0);
	 void MakeFocusFar(pPelco Ppkt, UINT8 byAddress = 0);
	 void MakeFocusNear(pPelco Ppkt, UINT8 byAddress = 0);
	 void MakeFocusStop(pPelco Ppkt, UINT8 byAddress = 0);
	 void MakeIrisOpen(pPelco Ppkt, UINT8 byAddress = 0);
	 void MakeIrisClose(pPelco Ppkt, UINT8 byAddress = 0);
	 void MakeZoomWide(pPelco Ppkt, UINT8 byAddress = 0);
	 void MakeZoomTele(pPelco Ppkt, UINT8 byAddress = 0);
	 void MakeZoomStop(pPelco Ppkt, UINT8 byAddress = 0);
	 void MakeAutoScan(pPelco Ppkt, UINT8 byAddress = 0);

	// EXTENDED COMMANDS
	 void MakePresetCtrl(pPelco Ppkt, INT32 nCtrlType, UINT8 byValue, UINT8 byAddress = 0);
	 void MakePatternCtrl(pPelco Ppkt, INT32 nCtrlType, UINT8 byValue, UINT8 byAddress = 0);
	 void MakeSetZoomSpeed(pPelco Ppkt, UINT8 byValue, UINT8 byAddress = 0);
	 void MakeSetFocusSpeed(pPelco Ppkt, UINT8 byValue, UINT8 byAddress = 0);
	 void MakeRemoteReset(pPelco Ppkt, UINT8 byAddress = 0);
	 void MakeExtCommand(pPelco Ppkt, INT32 nCmdType, UINT8 byValue, UINT8 byDataExt = 0, UINT8 byAddress = 0);
	 void MakeDummy(pPelco Ppkt, UINT8 byAddress = 0);

	 void MakeSetPanPos(pPelco Ppkt, UINT16 byValue, UINT8 byAddress /* = 0 */);
	 void MakeSetTilPos(pPelco Ppkt, UINT16 byValue, UINT8 byAddress /* = 0 */);


};

#endif
