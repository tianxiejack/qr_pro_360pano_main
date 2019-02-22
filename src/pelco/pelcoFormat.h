/*
 * pelcoFormat.h
 *
 *  Created on: 2018年9月14日
 *      Author: jet
 */

#ifndef PELCOFORMAT_H_
#define PELCOFORMAT_H_

typedef int				INT32, *PINT32;
typedef short			INT16, *PINT16;
//typedef char			INT8, *PINT8;
typedef unsigned int	UINT32, *PUINT32;
typedef unsigned short	UINT16, *PUINT16;
typedef unsigned char   UINT8, *PUINT8;
typedef long			LONG, *PLONG;
typedef unsigned long   ULONG, *PULONG;
typedef void*		pPelco;


enum{
	PTZ_MOVE_Stop = 0,PTZ_MOVE_Up,PTZ_MOVE_Down,PTZ_MOVE_Left, PTZ_MOVE_Right
};


//////////////////////////////////////////////////////////////////////////
//  pelco_d
const int PELCO_D_SYNC = 0xFF;
const int PELCO_D_SPEED_PEAKVALUE = 0x3F;
//  pelco_p
const int PELCO_P_STX = 0xA0;
const int PELCO_P_ETX = 0xAF;

const int PELCO_PRESET_SCAN_AUTO	= 99	;//Begin Auto-scan
const int PELCO_PRESET_SCAN_FRAME = 98;	//Begin Frame Scan
const int PELCO_PRESET_SCAN_RANDOM = 97;//Begin Random Scan
const int PELCO_PRESET_SCAN_STOP = 96;	//Stop Scan
const int PELCO_PRESET_ENTERMENU	= 95;	//Enter Menu Mode
const int PELCO_PRESET_REMOTERESET	= 94;	//Remote Reset
const int PELCO_PRESET_RIGHTLIMIT	= 93;	//Set Right Limit Stop
const int PELCO_PRESET_LEFTLIMIT	= 92;	//Set Left Limit Stop
const int PELCO_PRESET_ZEROPAN	= 34;	//Home (return to 0 pan position)
const int PELCO_PRESET_FLIP = 33;	//Flip

enum{
	PTZ_PRESET_SET = 3, PTZ_PRESET_CLEAR = 5, PTZ_PRESET_GOTO = 7
};

enum{
	PTZ_Pattern_SetStart = 31, PTZ_Pattern_SetStop=33, PTZ_Pattern_Run = 35
};

enum{
	PTZ_QUERY_Pan = 0x51,
	PTZ_QUERY_Tilt = 0x53,
	PTZ_QUERY_Zoom = 0x55,
	PTZ_QUERY_Magnification = 0x61,
	PTZ_QUERY_DeviceType = 0x6B,
	PTZ_QUERY_DiagnosticInfo = 0x6F
};

const int PELCO_EXTCOMMAND_SetAux = 0x09;
const int PELCO_EXTCOMMAND_ClearAux = 0x0B;
const int PELCO_EXTCOMMAND_RemoteReset	= 0x0F;
const int PELCO_EXTCOMMAND_ZoneStart = 0x11;
const int PELCO_EXTCOMMAND_ZoneEnd	= 0x13;
const int PELCO_EXTCOMMAND_Writechar = 0x15;
const int PELCO_EXTCOMMAND_ClearScreen = 0x17;
const int PELCO_EXTCOMMAND_AlarmAck = 0x19;
const int PELCO_EXTCOMMAND_ZoneScanOn	= 0x1B;
const int PELCO_EXTCOMMAND_ZoneScanOff	= 0x1D;
const int PELCO_EXTCOMMAND_PatternStart = 0x1F;
const int PELCO_EXTCOMMAND_PatternStop = 0x21;
const int PELCO_EXTCOMMAND_PatternRun	 = 0x23;

#pragma pack(1)

typedef struct {
	UINT8 bySynchByte;
	UINT8 byAddress;
	union{
		UINT8 byCmd1;
		struct{
			UINT8 FocusNear : 1;  //��������
			UINT8 IrisOpen  : 1;  //��Ȧ��
			UINT8 IrisClose : 1;  //��Ȧ�ر�
			UINT8 CameraOn  : 1;
			UINT8 AutoScan  : 1;
			UINT8 Reserved2 : 1;
			UINT8 Reserved1 : 1;
			UINT8 Sense     : 1;
		}tCmd1;
	};
	union{
		UINT8 byCmd2;
		struct{
			UINT8 Reserved  : 1;
			UINT8 Right     : 1;
			UINT8 Left      : 1;
			UINT8 Up        : 1;
			UINT8 Down      : 1;
			UINT8 ZoomTele  : 1;  //�ӽǱ�խ
			UINT8 ZoomWide  : 1;  //�ӽǱ��
			UINT8 FocusFar  : 1;  //������Զ
		}tCmd2;
	};
	UINT8 byData1;
	UINT8 byData2;
	UINT8 byCheckSum;
}PELCO_D_REQPKT, *LPPELCO_D_REQPKT;

typedef struct {
	UINT8 bySynchByte;
	UINT8 byAddress;
	union{
		UINT8 byValue;
		struct{
			UINT8 Alarm8  : 1;
			UINT8 Alarm7  : 1;
			UINT8 Alarm6  : 1;
			UINT8 Alarm5  : 1;
			UINT8 Alarm4  : 1;
			UINT8 Alarm3  : 1;
			UINT8 Alarm2  : 1;
			UINT8 Alarm1  : 1;
		}tValue;
	};
	UINT8 byCheckSum;
}PELCO_D_RESPPKT, *LPPELCO_D_RESPPKT;


typedef struct {
	UINT8 bySTX;
	UINT8 byAddress;
	union{
		UINT8 byCmd1;
		struct{
			UINT8 FocusFar  : 1;  //������Զ
			UINT8 FocusNear : 1;  //��������
			UINT8 IrisOpen  : 1;  //��Ȧ��
			UINT8 IrisClose : 1;  //��Ȧ�ر�
			UINT8 CameraOn  : 1;
			UINT8 AutoScan  : 1;
			UINT8 Camera    : 1;
			UINT8 Reserved  : 1;
		}tCmd1;
	};
	union{
		UINT8 byCmd2;
		struct{
			UINT8 PanTilt   : 1;
			UINT8 Right     : 1;
			UINT8 Left      : 1;
			UINT8 Up        : 1;
			UINT8 Down      : 1;
			UINT8 ZoomTele  : 1;  //�ӽǱ�խ
			UINT8 ZoomWide  : 1;  //�ӽǱ��
			UINT8 Reserved  : 1;
		}tCmd2;
	};
	UINT8 byData1;
	UINT8 byData2;
	UINT8 byETX;
	UINT8 byCheckSum;
}PELCO_P_REQPKT, *LPPELCO_P_REQPKT;
#pragma pack()



#endif /* PELCOFORMAT_H_ */
