/*
 * Status.hpp
 *
 *  Created on: Dec 27, 2018
 *      Author: wj
 */

#ifndef STATUS_HPP_
#define STATUS_HPP_


#define HELDNUM  2
#define HELDWEEK  7
#define HELDHOUR  24

typedef struct{
	int address;
	int protocol;
	int baudrate;
	int start_signal;
	int pt_check;
}scan_platformcfg_t;

typedef struct{
	int outputresol;
	int brightness;
	int contract;
	int autobright;
	int backlight;
	int whitebalance;
	int gain;
	int daynight;
	int stronglightsup;
	int exposuremode;
	int elecshutter_time;
}sensortvcfg_t;

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
	int sensor;
	int hideline;
	int offset50m;
	int offset100m;
	int offset300m;
}radarcfg_t;
	
class Status{
public:
	typedef enum {
		LIVEMOD=0,
		PLAYCALLBACK,
	}Dismod;

	typedef enum {
		PTZFOCUSSTOP = 0,
		PTZFOCUSSNEAR,
		PTZFOCUSSFAR,
	}Focusctl;
	typedef enum {
		PTZFOCUSLENGTHSTOP = 0,
		PTZFOCUSLENGTHOWN,
		PTZFOCUSLENGTHUP,
	}Focusonctl;

	typedef enum {
		PTZIRISSTOP = 0,
		PTZIRISDOWN,
		PTZIRISUP,
	}Irisctl;

	typedef enum {
		PANOAUTO,
		PANOPTZ,
		PANOSELECT,
		TRACKPTZ,
		TRACKSELECT
	}Workmod;

	typedef enum {
		MOUSEBUTTON,
		MOUSEROLLER,
		
	}MouseEvent;


	typedef enum {
		STOREGO,
		STORESAVE,
		
		
	}STOREMOD;
	typedef enum {
		
		STOREERASENUM,
		STORESAVENUM,
		STOREDEFAULT,
		
	}STOREMODNUM;


	typedef enum {
		ZEROSTOP,
		ZEROSTART,
		ZEROSAVE,
		
	}ZeroMod;


	typedef enum {
		PLAYERSTOP,
		PLAYERRUN,
		PLAYERACC,
		PLAYERDEC,
		
	}PlayerCtl;


	typedef enum {
		MOVSENSERTITYHIGH,
		MOVSENSERTITYMID,
		MOVSENSERTITYLOW,
		
	}MOVSENSERTITY;


	typedef enum {
		SPEEDEXTRAHIGH,
		SPEEDHIGH,
		SPEEDMIDDLE,
		SPEEDLOW,
		SPEEDEXTRALOW
		
	}MOVSPEED;

	typedef enum {
		DEV1 = 1,
		DEV2,
		DEV3,
		DEVMAX
	}DevId;

	typedef enum {
		PTZ_SCAN = 1,
		PTZ_TRK,
	}PtzId;

	typedef enum {
		CFG_TRKPLARFORM = 0,
		CFG_SENSORFR = 1,
		CFG_ZERO = 2,
		CFG_PLAYBACK = 3,
		CFG_MTD = 4,
		CFG_OUTPUTRESOL = 5,
		CFG_CALIBTIME = 6,
		CFG_MONTAGE = 7,
		CFG_ROADSAVE = 9,
		CFG_VIDEO = 10,
		CFG_SCANPLATFORM = 11,
		CFG_SENSORTV = 12,
		CFG_SENSORTRK = 13,
		CFG_SENSORRADAR = 14,
		CFG_TRK = 15,
		CFG_DEV = 16,
	}Cfgid;


	const int mvconfignum=16;

	

	int ptzpanodirection;
	int ptztitledirection;
	int ptzpanspeed;
	int ptztitlespeed;

	int mouseevent;
	int mousemod;
	int mousebuttonstaus;
	int mouseleftright;
	int mousex;
	int mousey;
	int zoom_rigion;
	int zoom_state;


	int storemod;
	
	int storegonum;

	int storesavenum;

	int storesavemod;
	/******jiaozhun*******/
	int calibration;
	/******player*******/
	int playercontrl;
	/******player select*******/
	int playeryear;
	int playermonth;
	int playerday;
	int playerhour;
	int playermin;
	int playersec;

	/************************/
	int mvconfigenable;

	/******player query*******/
	int playerqueryyear;
	int playerquerymon;
	int playerqueryday;

	/******trk plantform config*******/
	int ptzaddress;
	int ptzprotocal;
	int ptzbaudrate;
	int ptzspeed;

	/******scan plantform config*******/
	scan_platformcfg_t scan_platformcfg;

	/******sensor TV config*******/
	sensortvcfg_t sensortvcfg;

	/******sensor TRK config*******/
	sensortvcfg_t sensortrkcfg;
	
	/******sensor FR config*******/
	unsigned char brightness;
	unsigned char contract;
	unsigned char autobright;
	unsigned char backandwrite;
	
	unsigned char correct;
	unsigned char digitfilter;
	unsigned char digitenhance;
	unsigned char mirror;
	unsigned char outputresol;

	
	/******zero config*******/

	unsigned char zeromod;


	/******record config*******/
	int recordpositionheld[HELDNUM][HELDWEEK][HELDHOUR];

	/******movedetect config*******/
	mtdcfg_t mtdcfg;


	/******movearea config*******/
	int detectareanum;
	int detectareaenable;


	/******display config*******/

	int displayresolution;

	/******time config*******/
	int correctyear;
	int correctmonth;
	int correctday;
	int correcthour;
	int correctmin;
	int correctsec;

	/******pano config*******/
	int panoptzspeed;
	int panopiexfocus;
	int panopicturerate;
	int panoresolution;


	/******detect config*******/
	int movdetectenable;
	int panodetectenable;

	/******radar config*******/
	radarcfg_t radarcfg;

	int querryconfig;

	/***********************/
	int mvdetectresearch;
	double researchangle;
	int mvdetectnum;
	int nextmvdetectnum;

	int currentid;
	int nextid;

	int usestepdetect;

	/***feedback***/
	int ack_updatepano;
	int ack_fullscreenmode;

	//int getcurrentid(int id){int cid=0;cid=id;cid=cid%;};
	
	double getmvreachangle(){return researchangle;};
	void setmvreachangle(double reach){researchangle=reach;};

	int getmvreach(){return mvdetectresearch;};
	void setmvreach(int reach){mvdetectresearch=reach;};

	

	int getmvdetectnum(){return mvdetectnum;};
	void setmvdetectnum(int reach){mvdetectnum=reach;};

	int getnextmvdetectnum(){return nextmvdetectnum;};
	void setnextmvdetectnum(int reach){nextmvdetectnum=reach;};

	int getusestepdetect(){return usestepdetect;};
	void setusestepdetect(int reach){usestepdetect=reach;};

	

	

public:
	void getmouseparam(int &button, int &state, int &x, int &y){button=mouseleftright;state=mousebuttonstaus;x=mousex;y=mousey;};
	int getdisplaymod(){return displaymod;};
	void setdisplaymod(int dis){displaymod=dis;};

	int getdisplaysensor(){return displaysensor;};
	void setdisplaysensor(int dis){displaysensor=dis;};

	int getworkmod(){return workmode;};
	void setworkmod(int dis){workmode=dis;};

	int getstoremod(){return storemod;};
	void setstoremod(int mod){storemod=mod;};

	int getdevid(){return devid;};
	void setdevid(int id){devid=id;};

	int getrigion(){return zoom_rigion;};
	void setrigion(int rigion){zoom_rigion=rigion;};

	int getzoomstat(){return zoom_state;};
	void setzoomstat(int stat){zoom_state=stat;};
	
private:
	int displaymod;
	int displaysensor;
	int workmode;
	int devid;
private:
	Status();
	~Status();
	static Status*instance;
	void init_scan_platformcfg();
	void init_mtdcfg();

public:
	static Status* getinstance();

};




#endif /* STATUS_HPP_ */
