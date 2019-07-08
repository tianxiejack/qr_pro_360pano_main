/*
 * StlGlDefines.h
 *
 *  Created on: Nov 3, 2016
 *      Author: hoover
 */

#ifndef STLGLDEFINES_H_
#define STLGLDEFINES_H_

#define DEFAULT_IMAGE_WIDTH 1920
#define DEFAULT_IMAGE_HEIGHT 1080
#define DEFAULT_IMAGE_DEPTH 2

#define HOT_WIDTH 720//1280
#define HOT_HEIGHT 288//1024
#define HOT_CC 1
#define TV_WIDTH 1920
#define TV_HEIGHT 1080
#define TV_CC 2

#define	 	PANO360FBOW	1920//1560
#define		PANO360FBOH	1080//360
#define		ROIFBOW	1920
#define		ROIFBOH		720
typedef enum{
	PANO_PIC,
	ROI_A,
	ROI_B,
	ROI_C,
	PIC_COUNT
}SaveIDX;

enum
{
	START_STATE,
	RECORDING_STATE,
	STOP_STATE
};

enum{
	TV_QUE_ID=0,
	HOT_QUE_ID,
	RTSP_QUE_ID,
	QUE_CHID_COUNT
};

enum{
	TV_DEV_ID=1,
	HOT_DEV_ID,
	DEV_ID_COUNT
};
enum CCT_COUNT {
		T_180_T,
		T_360_T,
		RADAR_T,
		CCT_COUNT
		};
#define T180_TGA "180.tga"
#define T360_TGA			"360.tga"
#define RADAR_TGA "radar.tga"

////////////////////////////////////////////////////
// sys config part
typedef struct{
	int address;
	int protocol;
	int baudrate;
	int ptzspeed;
	int start_signal;
	int pt_check;
}platformcfg_t;

typedef struct{
	int outputresol;
	int brightness;
	int contract;
	int autobright;
	// tv part
	int backlight;
	int whitebalance;
	int gain;
	int daynight;
	int stronglightsup;
	int exposuremode;
	int elecshutter_time;
	// fr part
	int backandwrite;
	int correct;
	int digitfilter;
	int digitenhance;
	int mirror;
}sensorcfg_t;

typedef struct{
	int movedetectalgenable;
	int sensitivity;
	int speedpriority;
	int movmaxwidth;
	int movmaxheight;
	int movminwidth;
	int movminheight;
	int moverecordtime;
}mtdcfg_t;

typedef struct{
	int hideline;
	int offset50m[2];// 0-tv 1-fr
	int offset100m[2];// 0-tv 1-fr
	int offset300m[2];// 0-tv 1-fr
}radarcfg_t;

typedef struct{
	int trkpriodis;
	int trkpriobright;
	int trkpriosize;
	int trktime;
}trackcfg_t;

typedef struct{
	int ptzspeed[2];// 0-tv 1-fr
	int piexfocus[2];// 0-tv 1-fr
	int circlefps[2];// 0-tv 1-fr
}panocfg_t;

typedef struct{
	int devid;
	char ip[16];
}adddevcfg_t;

typedef struct{
	int year;
	int mon;
	int day;
	int hour;
	int min;
	int sec;
}playertime_t, playerdate_t, correcttime_t;

#endif /* STLGLDEFINES_H_ */
