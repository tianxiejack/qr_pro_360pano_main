#include"RecordManager.hpp"
#include <stdio.h>
#include <iostream>
#include "fstream"  
#include <dirent.h>
#include <stdio.h>
#include <string>
#include"videoload.hpp"
#include"videorecord.hpp"
#include "DxTimer.hpp"
//#include "globalDate.h"
RecordManager*RecordManager::instance=NULL;
#define AVHEAD "record"
#define AVTAIL ".avi"
#define MP4TAIL ".mp4"
#define FILETAIL ".xml"
#define AVCONTENC "/home/nvidia/calib/video/"


RecordManager::RecordManager():recordpath(AVCONTENC),nextvideoid(0),createplayertimeid(0),playertimer(60),enableplay(0),recordavierase("1.avi"),recorddirerase(AVCONTENC),recordnameerase("1.xml")
{

}
RecordManager::~RecordManager()
{


}

void RecordManager::create()
{
	findrecordnames();
	createplayertimer();
	VideoRecord::getinstance()->registermanagercall(ringrecord);
	

}
void RecordManager::recordplaycallback(void *arg)
{
	
	//printf("%s\n",__func__);
	if(instance==NULL)
		return ;
	if(instance->getableplayer())
	VideoLoad::getinstance()->playvideo();
	
}

void RecordManager::setplayertimer(int playerclass)
{
	switch(playerclass)
	{
		case -16:
			recordtimer = 960;
			break;
		case -14:
			recordtimer = 840;
			break;
		case -12:
			recordtimer = 720;
			break;
		case -10:
			recordtimer = 600;
			break;
		case -8:
			recordtimer = 480;
			break;
		case -6:
			recordtimer = 360;
			break;
		case -4:
			recordtimer = 240;
			break;
		case -2:
			recordtimer = 120;
			break;
		case 0:
			recordtimer = 60;
			break;
		case 2:
			recordtimer = 30;
			break;
		case 4:
			recordtimer = 15;
			break;
		case 6:
			recordtimer = 11;
			break;
		case 8:
			recordtimer = 7;
			break;
		case 10:
			recordtimer = 6;
			break;
		case 12:
			recordtimer = 5;
			break;
		case 14:
			recordtimer = 4;
			break;
		case 16:
			recordtimer = 3;
			break;
		default:
			break;
	}
	
	printf("%s,%d,player timer=%d\n",__FILE__,__LINE__,recordtimer);
	DxTimer::getinstance()->resetTimer(createplayertimeid,recordtimer);

}
void RecordManager::createplayertimer()
{
	createplayertimeid=DxTimer::getinstance()->createTimer();
	DxTimer::getinstance()->registerTimer(createplayertimeid,recordplaycallback,0);
	DxTimer::getinstance()->startTimer(createplayertimeid,playertimer);

}

 bool RecordManager::startsWith(const std::string& str, const std::string& substr)
    {
        return str.find(substr) == 0;
    }

bool RecordManager::endsWith(const std::string& str, const std::string& substr)
    {
        return str.rfind(substr) == (str.length() - substr.length());
    }
void RecordManager::getJustCurrentFile(string  path, vector<string> & video,vector<string> & files)
{

    DIR * dir, *dir2;
    struct dirent *ptr, *ptr2;
    int i=0;   
    string x,names;
    video.clear();
    files.clear();
    dir = opendir((char *)path.c_str());
    while((ptr = readdir(dir)) != NULL)
    	{
    		if(ptr->d_type & DT_DIR)
    		{
			if(strcmp(ptr->d_name,".")==0 || strcmp(ptr->d_name,"..")==0)
				continue;
			else
			{
				string  path2 = path + ptr->d_name;
				printf("%s, %d,path=%s\n", __FILE__,__LINE__, path2.c_str());
				dir2 = opendir((char *)path2.c_str());
				while((ptr2 = readdir(dir2)) != NULL)
				{
				   	if(ptr2->d_type & DT_DIR)
    					{
						if(strcmp(ptr2->d_name,".")==0 || strcmp(ptr2->d_name,"..")==0)
							continue;
				   	}
					else
					{
						x=ptr2->d_name;
					      names = x;
					      if(startsWith(names,AVHEAD))
					      	{
					      		if(endsWith(names,MP4TAIL))
					      		{
					      			printf("%s, %d,names=%s\n", __FILE__,__LINE__, names.c_str());
								video.push_back(names);
					      		}
							else if(endsWith(names,FILETAIL))
								files.push_back(names);

					      	}
					}
				}
				closedir(dir2);
			}
		}
		else
		{
			x=ptr->d_name;
		      names = x;
		      if(startsWith(names,AVHEAD))
		      	{
		      		if(endsWith(names,MP4TAIL))
		      		{
		      			printf("%s, %d,names=%s\n", __FILE__,__LINE__, names.c_str());
					video.push_back(names);
		      		}
				else if(endsWith(names,FILETAIL))
					files.push_back(names);

		      	}
		} 
    	}
	closedir(dir);
 }

#define DEFAULT_DISK_PATH "/home/nvidia/calib/"




int RecordManager::getDiskInfo(pDISK diskInfo,const char *path)
{
    char dpath[100]=DEFAULT_DISK_PATH;
    int flag=0;

    if(NULL!=path)
    {
        strcpy(dpath,path);
    }

    if(-1==(flag=statfs(dpath,diskInfo)))
    {
        perror("getDiskInfo statfs fail");
        return 0;
    }

    return 1;
}


int RecordManager::calDiskInfo(int *diskTotal,int *diskAvail,int *diskFree,pDISK diskInfo)
{
    unsigned long long total=0,avail=0,free=0,blockSize=0;
    int flag=0;

    if(!diskTotal&&diskAvail&&diskFree&&diskInfo)
    {
        printf("\ncalDiskInfo param null!\n");
        return 0;
    }
    blockSize=diskInfo->f_bsize;
    *diskTotal=(diskInfo->f_blocks*blockSize)>>20;
    *diskAvail=(diskInfo->f_bavail*blockSize)>>20;
    *diskFree=(diskInfo->f_bfree*blockSize)>>20;

    
    
    return flag;

}
void RecordManager::ringrecord()
{
	int diskTotal=0;
	int diskAvail=0;
	int diskFree=0;
	string aviname;
	string xmlname;
	if(instance==NULL)
		return;
	instance->getDiskInfo(&instance->diskInfo,NULL);
	instance->calDiskInfo(&diskTotal,&diskAvail,&diskFree,&instance->diskInfo);
	char cmdBuf[128];
	
	

	if(diskFree<2000)
		{
			instance->findrecordnames();
			aviname=instance->recordvideonames[instance->earlyestnum];
			//printf("erase %s\n",);
			cout<<"erase"<<aviname<<endl;
			xmlname=aviname;
			
			string::iterator begin=xmlname.end()-4;
			string::iterator end=xmlname.end();
			xmlname.erase(begin,end);//+FILETAIL;
			xmlname=xmlname+FILETAIL;
			aviname=AVCONTENC+aviname;
			xmlname=AVCONTENC+xmlname;
			sprintf(cmdBuf, "rm %s", aviname.c_str());
			system(cmdBuf);

			sprintf(cmdBuf, "rm %s", xmlname.c_str());
			system(cmdBuf);
		}
	
	//printf("the diskFree=%d\n",diskFree);

	
	

}
void RecordManager::findrecordnames()
{
	//string temp;
	getJustCurrentFile(recordpath, recordvideonames,recordfilenames);
	Recordmantime data;
	recordtime.clear();
	int onece=0;
	for(int i=0;i<recordvideonames.size();i++)
		{
			string videname=recordvideonames[i];
			sscanf(videname.c_str(),"record_%04d%02d%02d-%02d%02d%02d_%04d%02d%02d-%02d%02d%02d.avi",&data.startyear,&data.startmon,&data.startday,&data.starthour\
				,&data.startmin,&data.startsec,&data.endyear,&data.endmon,&data.endday,&data.endhour,&data.endtmin,&data.endsec);
			if(onece)
				{
					unsigned int cday=(data.startyear*24+data.startmon)*30+data.startday;
					unsigned int csec=(data.starthour*60+data.startmin)*60+data.startsec;
					unsigned int pday=(earlyestdata_.startyear*24+earlyestdata_.startmon)*30+earlyestdata_.startday;
					unsigned int psec=(earlyestdata_.starthour*60+earlyestdata_.startmin)*60+earlyestdata_.startsec;
					if(cday<pday)
						{
							earlyestdata_=data;
							earlyestnum=i;
						}
					else if((csec<psec)&&(cday=pday))
						{
							earlyestdata_=data;
							earlyestnum=i;
						}
					//if(data.startyear*24data.startmon)

				}
			else
				earlyestdata_=data;
			onece=1;
			recordtime.push_back(data);
		}

}


void RecordManager::setselecttime(playerdate_t startparam, playerdate_t selectparam)
{
	VideoLoad::getinstance()->setselecttime(startparam, selectparam);
}

void RecordManager::setpalyervide(int num)
{
	int count=recordvideonames.size();
	if(num>=count)
		return ;

	nextvideoid = num;
	
	string videname=recordvideonames[num];
	string directory = videname.substr(7, 8);
	videname = directory + "/" + videname;
		
	cout<<"zzq will play:"<<videname<<endl;
	VideoLoad::getinstance()->setreadavi(videname);
	cout<<"[setpalyervide]"<<videname<<endl;
	string::iterator begin=videname.end()-4;
	string::iterator end=videname.end();
	videname.erase(begin,end);//+FILETAIL;
	videname=videname+FILETAIL;
	//string filename=videname.erase( 4,4);//+FILETAIL;
	VideoLoad::getinstance()->setreadname(videname);
	VideoLoad::getinstance()->setreadnewfile(1);


	cout<<"[setpalyervide]"<<videname<<endl;
	


}
void RecordManager::getnexvideo()
{	
	int num=recordvideonames.size();
	if(num==0)
		return ;
	nextvideoid++;
	nextvideoid=nextvideoid%num;

	string videname=recordvideonames[nextvideoid];
	string directory = videname.substr(7, 8);
	videname = directory + "/" + videname;


	VideoLoad::getinstance()->setreadavi(videname);
	cout<<"[getnexvideo]"<<videname<<endl;
	string::iterator begin=videname.end()-4;
	string::iterator end=videname.end();
	videname.erase(begin,end);//+FILETAIL;
	videname=videname+FILETAIL;
	//string filename=videname.erase( 4,4);//+FILETAIL;
	VideoLoad::getinstance()->setreadname(videname);
	VideoLoad::getinstance()->setreadnewfile(1);

	cout<<"[getnexvideo]"<<videname<<endl;

}

void RecordManager::setdataheldrecord(int a[2][7][24])
{
	if(a==NULL)
		return ;
	
	VideoRecord::getinstance()->setdataheldrecord(a);

}
RecordManager*RecordManager::getinstance()
{
	if(instance==NULL)
	{
		instance=new RecordManager();
	}
	return instance;
}
