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
class Status{
public:
	typedef enum {
		LIVEMOD=0,
		PLAYCALLBACK,
	}Dismod;

	typedef enum {
		PTZPANOMOV,
		PTZTITLEMOV,
		PTZTWOMOV,
	}Plantctl;

	typedef enum {
		PTZFOCUSSTOP,
		PTZFOCUSSNEAR,
		PTZFOCUSSFAR,
	}Focusctl;
	typedef enum {
		PTZFOCUSONSTOP,
		PTZFOCUSSONDOWN,
		PTZFOCUSSONUP,
	}Focusonctl;

	typedef enum {
		PTZIRISSTOP,
		PTZIRISDOWN,
		PTZIRISUP,
	}Irisctl;

	typedef enum {
		PANOAUTO,
		PANOPTZ,
		PANOSELECT,
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
	int rollerstatus;


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

	/******plantform config*******/

	int ptzaddress;
	int ptzprotocal;
	int ptzbaudrate;
	int ptzspeed;

	/******sensor config*******/
	unsigned char brightness;
	unsigned char contract;
	unsigned char autobright;
	unsigned char backandwrite;
	
	unsigned char correct;
	unsigned char digitfilter;
	unsigned char digitdenoise;
	unsigned char mirror;
	
	unsigned char crossdisplay;
	unsigned int crossx;
	unsigned int crossy;
	unsigned int save;

	
	/******zero config*******/

	unsigned char zeromod;


	/******record config*******/
	int recordpositionheld[HELDNUM][HELDWEEK][HELDHOUR];

	/******movedetect config*******/

	int movedetectalgenable;

	//int movedetectmaxnum;
	int sensitivity;
	
	int speedpriority;

	
	int movmaxwidth;
	int movmaxheight;
	int movminwidth;
	int movminheight;
	//int movedetectrecord;

	int moverecordtime;

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


	/******detect config*******/
	int movdetectenable;
	int panodetectenable;

	/***********************/

	int querryconfig;

	/***********************/
	int mvdetectresearch;
	double researchangle;
	int mvdetectnum;
	int nextmvdetectnum;

	int currentid;
	int nextid;

	int usestepdetect;

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

	int getworkmod(){return workmode;};
	void setworkmod(int dis){workmode=dis;};


	int getstoremod(){return storemod;};
	void setstoremod(int mod){storemod=mod;};
	
private:
	int displaymod;
	int workmode;
private:
	Status();
	~Status();
	static Status*instance;

public:
	static Status* getinstance();

};




#endif /* STATUS_HPP_ */
