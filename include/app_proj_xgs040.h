/*****************************************************************************
 * Copyright (C), 2011-2012, ChamRun Tech. Co., Ltd.
 * FileName:        APP_proj_xgs021.h
 * Author:          aloysa
 * Date:            2017-08-01
 * Description:     // description
 * Version:         // version
 * Function List:   // funs and description
 * History:         // histroy modfiy record
 *     <author>  <time>   <version >   <desc>
 *
*****************************************************************************/

#ifndef __APP_PROJ_XGS040_H_
#define __APP_PROJ_XGS040_H_

#define PLAT_TX1		0
#define PLAT_TX2		1

#define DS_SCREEN_NUM	1
#define MTD_TARGET_NUM  3

#if PLAT_TX1
#define GPIO_CAP_CHECK_COLTV 		(186)
#define GPIO_CAP_CHECK_BLACKTV 	(187)
#define GPIO_CAP_CHECK_FR 			(84)
#define GPIO_CAP_CHECK_SHORT_FR 	(38)
#elif PLAT_TX2
#define GPIO_CAP_CHECK_COLTV 		(298)
#define GPIO_CAP_CHECK_BLACKTV 	(388)
#define GPIO_CAP_CHECK_FR 			(391)
#define GPIO_CAP_CHECK_SHORT_FR 	(397)
#endif

#define GPIO_INSPECT_TIMER		(0x0)
#define GPIO_INSPECT_TICKS		(500) //ms

#define GRPX_SHOW_TIMER		(0x1)
#define GRPX_SHOW_TICKS		(100) //ms

#define COM_422_TIMER		(0x2)
#define COM_422_TICKS		(2000) //ms

#define TRK_ASSI_TIME 		(60)//ms
#define TRK_LOST_TIME 		(TRK_ASSI_TIME+5000)//ms

static int vcapWH[2/*eSen_Max*/][2] = {{1920, 1080}, {1920, 1080}};
static int vdisWH[2][2] = {{1920, 1080}, {640, 360}};		// main and picp
static int trkWinWH[2/*eSen_Max*/][4][2] = 
{
	// TV
	{
	{80, 60},
	{112, 84},
	{80, 60},
	{54, 40},
	},
	// FR
	{
	{80, 60},
	{112, 84},
	{80, 60},
	{54, 40},
	},
};

static int fovSize[2][5] = 
{
	{
		2400,
		1000,
		330,
		100,
		50
	},
	{
		4000,
		1000,
		330,
		83,
		41
	}

};


typedef enum
{
	GRPX_WINID_SYS_TIMER_TEXT 		= 0x0,
	GRPX_WINID_SYS_MODE_TEXT,
	GRPX_WINID_SENSOR_TEXT,
	GRPX_WINID_ALG_MARK_TEXT,
	GRPX_WINID_PLAT_ANGLE_TEXT,
	GRPX_WINID_ORIENTATE_TEXT		= 0x5,
	GRPX_WINID_LASER_STAT_TEXT,
	GRPX_WINID_LASER_WORK_TEXT,
	GRPX_WINID_LASER_COUNT_TEXT,
	GRPX_WINID_POSE_ANGLE_TEXT,
	GRPX_WINID_INFO_MSG_TEXT,
	GRPX_WINID_CROSS_TEXT			= 0xB,
	GRPX_WINID_TRKRTS_TEXT,
	GRPX_WINID_SENSOR_RECT,
	GRPX_WINID_FOVMATCH_RECT,
	GRPX_WINID_COMPASS,

	GRPX_WINID_MAX			// please don't modify this id name

} osdCR_graph_winId;

#define WINID_MAX (GRPX_WINID_MAX)

#define BYTE2BIT(Byte, BitNum, At) (((BitNum+At)<=8) ? ((Uint8)(Byte<<(8-At-BitNum)) >> (8-BitNum)) : -1)

int APP_init_graphic_obj(void *pPrm);
int APP_init_graphic_parms(int chId, int winId, void *pPrm);
int APP_set_graphic_parms(int chId, int winId, void *pPrm);
int APP_set_flicker_parms(int chId, int winId, void *pPrm);

#endif
/*************************************END***************************************/

