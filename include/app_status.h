/********************************************************************************
  Copyright (C), 2011-2012, CamRun Tech. Co., Ltd.
  File name:    app_status.h
  Author:   xavier       Version:  1.0.0      Date: 2013-04-17
  Description: this file define the globle externel input state struct
              together with the avt ouput state struct.
  Others:
  Function List:
  History:
    1. Date:
       Author:
       Modification:
    2. ...
  History:
    <author>  <time>      <version >   <desc>
      aloysa     17/04/25   1.0     
********************************************************************************/

#ifndef _GLOBAL_STATUS_H_
#define _GLOBAL_STATUS_H_

//#ifdef __cplusplus
//extern "C" {
//#endif

#include "demo_global_def.h"

/*
* *PROJ ITEM
*/
//#define PROJ_BASE
#define PROJ_XGS021	1
#define PROJ_XGS026	0

#if PROJ_XGS026
#include "app_proj_xgs026.h"
#endif

#if PROJ_XGS021
#include "app_proj_xgs040.h"
#endif

#if PROJ_T18_205
#define MMT_TARGET_NUM  8
#else
#define MMT_TARGET_NUM  5
#endif

typedef enum Dram_SysMode1 {
    INIT_MODE           = 0x00,
    CHECK_MODE      = 0x01,
    AXIS_MODE           = 0x02,
    NORMAL_MODE     = 0x03,
}
eSysMode1;

typedef enum Dram_SenserStat1
{
    eSen_TV_Stat        = 0x00,
    eSen_FLIR_Stat      = 0x01,
    eSen_Max
} eSenserStat1;

typedef enum Dram_zoomCtrl1
{
    eZoom_Ctrl_Pause    = 0x00,
    eZoom_Ctrl_SCAL2    = 0x01,
    eZoom_Ctrl_SCAL4    = 0x02,
} eZoomCtrl1;

typedef enum Dram_ImgAlg1
{
    eImgAlg_Disable     = 0x00,
    eImgAlg_Enable      = 0x01,
} eImgAlgStat1;  // use by img enh/stb/mtd

typedef enum Dram_MMTSelect1
{
    eMMT_No     = 0x00,
    eMMT_Next       = 0x01,
    eMMT_Prev       = 0x02,
    eMMT_Select = 0x03,
} eMMTSelect1;

typedef enum Dram_DispGradeStat1
{
    eDisp_hide      = 0x00,
    eDisp_show_rect     = 0x01,
    eDisp_show_text     = 0x02,
    eDisp_show_dbg      = 0x04,
} eDispGrade1;

typedef enum Dram_DispGradeColor1
{
    ecolor_Default  = 0x0,
    ecolor_Black 	= 0x1,
    ecolor_White    = 0x2,
    ecolor_Red 	= 0x03,
    ecolor_Yellow 	= 0x04,
    ecolor_Blue 	= 0x05,
    ecolor_Green 	= 0x06,
} eOSDColor1;

typedef enum
{
    eTrk_Acq        = 0x00,
    eTrk_Normal 	= 0x01,
    eTrk_Assi       = 0x02,
    eTrk_Lost       = 0x03,
} eTrkStat1;

typedef enum Dram_trkMode1
{
    eTrk_mode_acq       = 0x00,
    eTrk_mode_target    = 0x01,
    eTrk_mode_mtd       = 0x02,
    eTrk_mode_sectrk    = 0x03,
    eTrk_mode_search    = 0x04,
#if (PROJ_T18_205|PROJ_T18_613)
    eTrk_mode_roam      = 0x05,
#endif
    eTrk_mode_trkalg          = 0x07,

    eTrk_mode_switch      = 0x100,
} eTrkMode1;

typedef enum Dram_trkRefine1
{
    eTrk_ref_no     = 0x00,
    eTrk_ref_left   = 0x01,
    eTrk_ref_right  = 0x02,
    eTrk_ref_up     = 0x01,
    eTrk_ref_down   = 0x02,
} eTrkRefine1;

typedef enum Dram_workMode1
{
    eMode_svr       = 0x00,
#if PROJ_T18_205
    eMode_search    = 0x08,
    eMode_sectrk    = 0x06,
    eMode_trk       = 0x05,
    eMode_axis      = 0x09,
    eMode_roam  = 0x0C,
#else
    eMode_search,
    eMode_sectrk,
    eMode_trk,
#endif
} eWorkMode1;

typedef enum Dram_saveMode1
{
    eSave_Disable       = 0x00,
    eSave_Enable        = 0x01,
    eSave_Cancel        = 0x02,
} eSaveMode1;

/** universal status **/
typedef struct
{
    /***** self stat part *****/
    unsigned int  unitVerYear;      // 14-99(2014-2099)
    unsigned int  unitVerMonth; // 1-12
    unsigned int  unitVerDay;       // 1-31
    unsigned int  unitVerNum;       // 1.23=>0x0123

    volatile unsigned char  unitFaultStat;  // bit0:tv input bit1:fr input bit2:avtctrl
    volatile unsigned char  unitWorkMode;   // eSysMode

    volatile unsigned int  unitTimePrm;     // bit0-bit7 second  bit8-bit15 minute  bit16-bit23 hours

    volatile unsigned int  unitTrkStat;     // cur avt21 stat (acp/trk/assi/lost)
    volatile unsigned int  unitAimFlicker;  // for assi count
    volatile float  unitAvtFov[eSen_Max];
    volatile unsigned int  unitAvtAlpha[eSen_Max];  // trk alpha level 0-3
    volatile unsigned int  unitZoomClass;     // for zoom class

    volatile int  unitAlgTrkPixelX;
    volatile int  unitAlgTrkPixelY;
    volatile int  unitAlgTrkCount;  // for alg trk timeout count
    volatile int  unitAlgTrkStat;   // cur alg trk stat(stop/in place)
    volatile float  unitAlgTrkTbeX;    // cur alg trk err same as avtctrl-> trk_tbe_x
    volatile float  unitAlgTrkTbeY;    // cur alg trk err same as avtctrl-> trk_tbe_y
    volatile float  unitAlgTrkPosX;    // cur alg trk pos same as avtctrl-> trk_pos_x
    volatile float  unitAlgTrkPosY;    // cur alg trk pos same as avtctrl-> trk_pos_ys

#if PROJ_T18_205
    volatile unsigned int  unitMtdValid;
    volatile unsigned int  unitRoamStat;    // for roam
    volatile unsigned int  unitSensorSwitch;	// sens stat switch

    volatile unsigned int  AxisCalibX[eSen_Max];     // Axis
    volatile unsigned int  AxisCalibY[eSen_Max];     // Axis

    volatile unsigned char  DispMtdValid;
    volatile unsigned char  CurSysMode;
    volatile unsigned char  CurWorkMode;
    volatile unsigned char  DispMtdEnable;
    volatile unsigned char  AxisSaving;	// 1- saving
#endif


    volatile unsigned short  unitAxisX; // pixel
    volatile unsigned short  unitAxisY; // pixel
    volatile unsigned short  unitAimW;  // aim size
    volatile unsigned short  unitAimH;  // aim size
    volatile unsigned short  unitAimX;  // pos for osd draw
    volatile unsigned short  unitAimY;  // pos for osd draw
    volatile float unitTrkX;    // err for report and osd text
    volatile float unitTrkY;    // err for report and osd text

    volatile unsigned int  unitLaserFlicker;	// for laser * count


    /***** cmd stat part *****/
    volatile unsigned char  SensorStat;     // eSenserStat
    volatile unsigned char  PicpSensorStat; // sensor src id range 0~3 or 0xFF no picp sens
    volatile unsigned int  	 PicpStat;        // eImgAlgStat
    volatile unsigned int  	 PicpZoomStat;	// eImgAlgStat

    volatile unsigned char  ImgZoomStat;    // eZoomCtrl
    volatile unsigned char  ImgEnhStat; // eImgAlgStat
    volatile unsigned char  ImgMtdStat; // eImgAlgStat
    volatile unsigned char  ImgMtdSelect;   // eMMTSelect or range 0-8(0-clean)
    volatile unsigned char  ImgStbStat;     // eImgAlgStat
    volatile unsigned char  ImgSerchTrkOsd; // 0-disable 1-enable
    volatile unsigned char  ImgSerchTrkReportVal;   // 0-disable 1-enable

    volatile unsigned char  TransUartRate;  // 00/01, 02, 03
    volatile unsigned char  TransEncMask;   //01, 02, 03
    volatile unsigned char  TransEncFps[eSen_Max];

    volatile unsigned char  AvtTrkStat;     // eTrkMode
    volatile unsigned char  AvtTrkAimSize;  // 0-3
    volatile unsigned char  AvtCfgSave; // eSaveMode
    volatile unsigned char  AvtTrkAlgStat;  // eTrkAlg
    volatile unsigned char  AvtTrkCoast;
    volatile unsigned char  AvtTrkAlpha;    // trk alpha level 0-3

    volatile unsigned char  AvtMoveX;       // eTrkRefine
    volatile unsigned char  AvtMoveY;       // eTrkRefine

    volatile unsigned short  AvtPixelX;     // for ext trk pos
    volatile unsigned short  AvtPixelY;     // for ext trk pos

    volatile unsigned int  AxisCalibStat;
    volatile unsigned int  AxisMoveX;        // eTrkRefine (axis)
    volatile unsigned int  AxisMoveY;        // eTrkRefine (axis)
    volatile unsigned int  AxisSave;      // eSaveMode

    /***** cmd osd part *****/
    volatile unsigned char  DispGrp;//[DS_SCREEN_NUM];        // eDispGrade
    volatile unsigned char  DispColor;//[DS_SCREEN_NUM];      // eOSDColor or eGRPColor
    volatile unsigned char  DispWorkMode;   // eWorkMode

    volatile float  DispPosAngle[2];    // [1] is pitch
    volatile float  DispFovAngle[eSen_Max];

    volatile unsigned char  DispZoomStat[eSen_Max];
    volatile unsigned char  DispEnhStat[eSen_Max];

    volatile unsigned int  DispFlirStat;        // bit16 powerstat 0-off 1-on  bit0-bit15 modestat
    volatile unsigned int  DispLaserStat;   // bit16 powerstat 0-off 1-on  bit0-bit15 modestat 000-stop 001-wait 010-once measure ...
    volatile unsigned int  DispDistance;
    volatile unsigned int  DispLaserCode;   // laser code values

    volatile unsigned int DispRadiateStat;  // bit0-7 radiate enable bit8-15 count down enable  1-enable
    volatile unsigned int DispRadiateCount; // count down value

#if PROJ_T18_205
    volatile unsigned int  DispOffset;
    volatile unsigned char  DispBomen;

    volatile unsigned int  DispTimePrm;    // bit0-bit7 second  bit8-bit15 minute  bit16-bit31 hours
    volatile unsigned int  DispDatePrm;    // bit0-bit7 day  bit8-bit15 month  bit16-bit31 year

    volatile short int TurretXAngle;
    volatile short int TurretYAngle;

    volatile unsigned char  LaserPowerStat;
	volatile unsigned char  LaserStat;
	volatile unsigned char  LaserEnable;
	volatile unsigned char  LaserEncode;
	volatile unsigned short LaserDistance;
	volatile char  LaserTemp;
	volatile unsigned char  LaserTempWarn;
	volatile int  LaserTime;

	volatile unsigned char  ErrorMsg;
	volatile unsigned char  sysModeChange;
	volatile unsigned char  DriftCPS;               // drift compensation

	volatile unsigned char  DispAvtMoveX;
	volatile unsigned char  DispAvtMoveY;

	volatile unsigned char  TrkDmesgStat;

#endif

} CMD_EXT1, CMD_ext1;

typedef struct
{
    union
    {
        unsigned char c[4];
        float         f;
        int           i;
    } un_data;
} ACK_REG;

#pragma pack (1)
typedef struct
{
    unsigned  char   syn;
    unsigned  char   id;

    //unsigned char data2-3;
    unsigned short targetpointx;

    //unsigned char data4-5;
    unsigned short targetpointy;

    //unsigned char data6;
    unsigned  int   sensor: 1;
    unsigned  int   alphastattv: 2;
    unsigned  int   alphastatfr: 2;
    unsigned  int   retain1: 3;

    //unsigned char data7-8;
    unsigned  short   crossx;
    //unsigned char data9-10;
    unsigned  short   crossy;

    //unsigned char data11;
    unsigned  int   trackstat: 2;
    unsigned  int   mtdstat: 2;
    unsigned  int   manyou: 1;
    unsigned  int   invidstat: 2;
    unsigned  int   autocheck: 1;

    //unsigned char data12;
    unsigned  int   bite: 2;
    unsigned  int   retain2: 2;
    unsigned  int   startstat: 1;
    unsigned  int   transport: 2;
    unsigned  int   retain4: 1;

    //unsigned char data13;
    unsigned  int   tvfps: 2;
    unsigned  int   frfps: 2;
    unsigned  int   retain5: 4;

    unsigned char check;
} ACK_EXT1;
#pragma pack ()


typedef enum 
{
   // MSGID_SYS_INIT  = 0x00000000,           ///< internal cmd system init.
  //  MSGID_SYS_RESET,
    //MSGID_AVT_RESET,

    MSGID_EXT_INPUT_SYSMODE = 0x00000010,   ///< external cmd, system work mode.
    MSGID_EXT_INPUT_SENSOR,                 ///< external cmd, switch sensor.

    // AVT21 track
    MSGID_EXT_INPUT_FOVSTAT,                ///< external cmd, switch fov.
    MSGID_EXT_INPUT_CFGSAVE,                ///< external cmd, config save(here only for axis save)
    MSGID_EXT_INPUT_AXISPOS,                ///< external cmd, set pos of cross axis.
    MSGID_EXT_INPUT_TRACKALG,               ///< external cmd, set track alg.
    MSGID_EXT_INPUT_TRACK,                  ///< external cmd, start track or stop.
    MSGID_EXT_INPUT_AIMPOS,                 ///< external cmd, set pos of aim area.
    MSGID_EXT_INPUT_AIMSIZE,                ///< external cmd, set size of aim area.
    //MSGID_EXT_INPUT_SCENELOCK,            ///< external cmd, scene set or clear
    //MSGID_EXT_INPUT_TRKBLEED,             ///< external cmd, close bleed
    MSGID_EXT_INPUT_COAST,              	///< external cmd, coast set or cannel
    MSGID_EXT_INPUT_TRKALPHA,				///< external cmd, set trk alpha level

    // img control
    MSGID_EXT_INPUT_ENPICP = 0x00000020,    ///< external cmd, open picture close.
    MSGID_EXT_INPUT_ENZOOM,                 ///< external cmd, zoom near or far.
    MSGID_EXT_INPUT_ENENHAN,                ///< external cmd, open image enhan or close.
    MSGID_EXT_INPUT_ENMTD,                  ///< external cmd, open mtd or close.
    MSGID_EXT_INPUT_MTD_SELECT,             ///< external cmd, select mtd target.
    MSGID_EXT_INPUT_ENSTB,                  ///< external cmd, open image stb or close.
    MSGID_EXT_INPUT_ENRST,                  ///< external cmd, open image rst or close.
    MSGID_EXT_INPUT_RST_THETA,              ///< external cmd, open image rst or close.
    MSGID_EXT_INPUT_BLOB_DETECT,            ///< external cmd, blob detect.

    // osd control
    MSGID_EXT_INPUT_DISPGRADE = 0x00000030, ///< external cmd, osd show or hide
    MSGID_EXT_INPUT_DISPCOLOR,              ///< external cmd, osd color

    // video control
    MSGID_EXT_INPUT_VIDEOCTRL,              ///< external cmd, video record or replay.
    MSGID_EXT_INPUT_GSTCTRL,              ///< external cmd, gst.

}MSG_PROC_ID1;


//#ifdef __cplusplus

//}
//#endif

#endif


