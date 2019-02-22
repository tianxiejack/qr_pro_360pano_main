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
class RecordManager
{
public:
	vector<string> recordvideonames;
	vector<string> recordfilenames;

	vector<Recordmantime> recordtime;
	string  recordpath;
	int nextvideoid;
	int createplayertimeid;
	int playertimer;
	int enableplay;
public:
	void create();
	typedef  struct statfs DISK,*pDISK;
	void enableplayer(int enable){enableplay=enable;};
	int getableplayer(){return enableplay;};
	
	void createplayertimer();
	void findrecordnames();
	void getnexvideo();
	void setpalyervide(int num);
	void getJustCurrentFile(string path, vector<string> & video,vector<string> & files);
	 bool startsWith(const std::string& str, const std::string& substr);
	 bool endsWith(const std::string& str, const std::string& substr);
	static void recordplaycallback(void *arg);
	void setplayertimer(unsigned int timer);
	int getDiskInfo(pDISK diskInfo,const char *path);
	int calDiskInfo(int *diskTotal,int *diskAvail,int *diskFree,pDISK diskInfo);
	static void ringrecord();

	void setdataheldrecord(int a[2][7][24]);

	string recordnameerase;
	string recordavierase;
	string recorddirerase;
	
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
};



#endif /* RECORDMANAGER_HPP_ */
