/*
 * RecordManager.hpp
 *
 *  Created on: Dec 27, 2018
 *      Author: wj
 */

#ifndef RECORDMANAGER_HPP_
#define RECORDMANAGER_HPP_
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <sys/statfs.h>
#include <sys/vfs.h>
#include"Status.hpp"
using namespace std;

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

}Recordmantime;

enum videotype{
	timer_video = 1,
	live_video,
	mtd_video
};
class RecordManager
{
public:
	vector<string> recordvideonames;
	vector<string> recordfilenames;
	vector<string> recordlivevideonames;

	vector<Recordmantime> recordtime;
	vector<Recordmantime> liverecordtime;
	string  recordpath;
	struct{
		int id;
		int type;
	} nextvideo;
	int createplayertimeid;
	int playertimer;
	int enableplay;
	int videoclip = 0;
public:
	void create();
	typedef  struct statfs DISK,*pDISK;
	void enableplayer(int enable){enableplay=enable;};
	int getableplayer(){return enableplay;};
	void enableclip(int enable){videoclip=enable;};
	int getableclip(){return videoclip;};
	
	void createplayertimer();
	void findrecordnames();

	int getpalyerclass(){return playerclass;};
	void setpalyerclass(int value){playerclass = value;};
	void getnexvideo();
	void setpalyervide(int num, int type);
	void setselecttime(playerdate_t startparam, playerdate_t selectparam);
	void removelocalfile(string  path);
	void getJustCurrentFile(string  path, vector<string> & video,vector<string> & files, vector<string> & livevideo);
	 bool startsWith(const std::string& str, const std::string& substr);
	 bool endsWith(const std::string& str, const std::string& substr);
	static void recordplaycallback(void *arg);
	void setplayertimer(int playerclass);
	int getDiskInfo(pDISK diskInfo,const char *path);
	int calDiskInfo(int *diskTotal,int *diskAvail,int *diskFree,pDISK diskInfo);
	static void ringrecord();

	void setdataheldrecord(int a[2][7][24]);

	string recordnameerase;
	string recordavierase;
	string recorddirerase;
	int playerclass = 0;
	int recordtimer = 60;
	
private:
	 DISK diskInfo;
	 Recordmantime earlyestdata_;
	 int earlyestnum;

private:
	RecordManager();
	~RecordManager();
	static RecordManager*instance;
public:
	static RecordManager*getinstance();
	void playerquerry();
};



#endif /* RECORDMANAGER_HPP_ */
