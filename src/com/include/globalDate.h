/*
 * globalDate.h
 *
 *  Created on: 2018年9月26日
 *      Author: jet
 */

#ifndef GLOBALDATE_H_
#define GLOBALDATE_H_

#include <iostream>
#include <vector>
#include <time.h>
#include "CMessage.hpp"
#include "osa_sem.h"
#include <errno.h>

using namespace std;

typedef struct{
	int fd;
	int type;// 1. /dev/ttyTHS*  2.network
}comtype_t;

typedef struct
{
	bool validCh[5];
	char idx;
}selectCh_t;

typedef  struct  {
	u_int8_t   cmdBlock;
	u_int8_t   cmdFiled;
	float    confitData;
}systemSetting;

typedef struct {
		int  m_TrkStat;
		int  m_SenSwStat;
		int m_ZoomLongStat;
		int m_ZoomShortStat;
		int m_zoomSpeed;
		int m_zoomSpeedBak;
		int m_TrkBoxSizeStat;
		int m_SecTrkStat;
		int m_AcqBoxStat;
		int m_AcqStat_ipc;
		int m_IrisUpStat;
		int m_IrisDownStat;
		int m_FoucusFarStat;
		int m_FoucusNearStat;
		int m_PresetStat;
		int m_ImgEnhanceStat;
		int m_MmtStat;
		int m_MmtSelectNum;
		volatile unsigned char m_AimPosXStat;
		volatile unsigned char m_AimPosYStat;
		int m_AxisXStat;
		int m_AxisYStat;
}CurrParaStat,*PCurrParaStat;

typedef struct{
	int block;
	int filed;
}Read_config_buffer;

typedef enum {
	Exit = 0,
	iris,
	Focus
}IrisAndFocusAndExit;

typedef struct{
	uint channel0:1;
	uint channel1:1;
	uint channel2:1;
	uint channel3:1;
	uint channel4:1;
	uint channel5:1;
	uint reserve5:2;
}channelTable;

typedef enum{
	DefaultOff = 0,
	PosLoop ,
	SpeedLoop
}MtdMode;

typedef enum{
	Default = 0,
	Next,
	Prev,
	Selext
}MtdSelect;

typedef enum{
	Auto = 0,
	Manual
}MtdExitMode;

enum{recv_AutoMtdDate = 0, shield_AutoMtdDate};

typedef struct{
	volatile int mptzx;
	volatile int mptzy;
}MOUSEPTZ;

typedef struct{
	volatile unsigned int preset;
	volatile unsigned int warning;
	volatile unsigned int high_low_level;
	volatile unsigned int trktime;  /*  ms  */
}MtdConfig;

typedef struct{
	unsigned int panPos;
	unsigned int tilPos;
	unsigned int zoom;
}LinkageParam;

typedef enum{
	platFormCapture = 0,
	boxCapture,
	ManualMtdCapture
}Capture_Mode;


typedef struct {
	int startyear;
	int startmon;
	int startday;
	int starthour;
	int startmin;
	int startsec;

	int endyear;
	int endmon;
	int endday;
	int endhour;
	int endtmin;
	int endsec;

}Recordtime;


typedef struct{
	int major;
	int secmajor;
	int testversion;
	int year;
	int mon;
	int day;
	int hour;
	int min;
	int sec;
	int softstage;


}SoftVersion;
#define MAJORVERSION 2
#define SECVERSION 0
#define TESTVERSION 2
class CGlobalDate
{
public:
	~CGlobalDate(){};

static CGlobalDate* Instance();

float errorOutPut[2];
char  ACK_read_content[128];
vector <float> ACK_read;
vector <float> Host_Ctrl;
vector<unsigned char>  rcvBufQue;
static vector<Read_config_buffer>  readConfigBuffer;
static vector<int>  defConfigBuffer;
vector<Recordtime> querrytime;
vector<Recordtime> querrylivetime;
MtdConfig mtdconfig;

SoftVersion softversion;

OSA_SemHndl  m_semHndl;
OSA_SemHndl m_semHndl_socket_client;
int sendfile_status;
void set_sendfile_status(int value){sendfile_status = value;};
int get_sendfile_status(void){return sendfile_status;};
void milliseconds_sleep(unsigned long mSec);

int feedback;
comtype_t comtype;
int mainProStat[ACK_value_max];
int choose;
int respupgradefw_stat;
int respupgradefw_perc;
int respexpconfig_stat;
int respexpconfig_len;
unsigned char respexpconfig_buf[1024+256];
int time_start;
long int time_stop;
 int frame;
 int target;
char ImgMtdStat;
char mtdMode;
char Mtd_Moitor; /*Recording detection area; 1: record; 0: don't record*/
char Mtd_ExitMode;
char Mtd_ipc_target; /*receive ipc detect message; 1: find target; 0: no target*/
char Mtd_Limit;  /* Block IPC messages if you enter the trace */
char MtdAutoredetect_sync;
unsigned short Mtd_Moitor_X;   	/*Detect regional center coordinates*/
unsigned short Mtd_Moitor_Y;
bool MtdAutoLoop;
int ThreeMode_bak;
int Sync_Query;
int rcv_zoomValue;
char send_filepath[128] = {0};

private:
CGlobalDate();
static CGlobalDate* _Instance;

};


#endif /* GLOBALDATE_H_ */
