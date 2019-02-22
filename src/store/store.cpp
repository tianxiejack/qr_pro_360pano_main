#include"store.hpp"
#include"stdio.h"
#include"osa_sem.h"
#include "CMessage.hpp"
#include"Status.hpp"
#include "plantformcontrl.hpp"
Store*Store::instance=NULL;
//#define DIRRECTDIR  "/home/ubuntu/calib/store.xml"
#define DIRRECTDIR "/home/ubuntu/config/store.xml"
Store::Store()
{
	OSA_mutexCreate(&filelock);
}
Store::~Store()
{
	OSA_mutexDelete(&filelock);
}
void Store::create()
{
	registorfun();
	load();

}

void Store::addstore()
{
	storepam parm;
	Plantformpzt::getinstance()->getpanopanpos();
	OSA_waitMsecs(200);
	Plantformpzt::getinstance()->getpanotitlepos();
	OSA_waitMsecs(200);
	parm.ptzpan=Plantformpzt::getinstance()->getpanangle();
	parm.ptztitle=Plantformpzt::getinstance()->gettitleangle();
	parm.value=1;
	printf("the pan=%f tile=%f\n",parm.ptzpan,parm.ptztitle);
	store.push_back(parm);
}
		
void Store::erasestore(int id)
{
	list<storepam>::iterator iter;
	//iter=store.begin()+4;//+id;
	
	
	int count=0;
	for(iter=store.begin();iter!=store.end();iter++)
		{
			//parm=*iter;
			if(count==id)
				break;
			count++;
		}
	if(count>=store.size())
		return;
	store.erase(iter);

}

void Store::update()
{
	

}
void Store::load()
{
	storepam parm;
	store.clear();
	//OSA_mutexLock(&filelock);
	filefp=fopen(DIRRECTDIR,"r");
	if (filefp==NULL)			
		{
			printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!Open Failed.\n");
			return;
		}

	if(filefp!=NULL)
		{
		
			while(!feof(filefp))
				{
					fgets(buffer,MAXSTORE_LINE,filefp);
					sscanf(buffer,"valid:%d_pan:%lf_title:%lf",&parm.value,&parm.ptzpan,&parm.ptztitle);
					//puts(buffer);
					//printf("valid:%d_pan:%f_title:%f",parm.value,parm.ptzpan,parm.ptztitle);
					//printf("***********************\n");
					store.push_back(parm);
				}
		
		}


	if(filefp!=NULL)
		fclose(filefp);
}

void Store::save()
{
	list<storepam>::iterator iter;
	storepam parm;
	if ((filefp=fopen(DIRRECTDIR,"w"))==NULL)			//\u6253\u5f00\u6307\u5b9a\u6587\u4ef6\uff0c\u5982\u679c\u6587\u4ef6\u4e0d\u5b58\u5728\u5219\u65b0\u5efa\u8be5\u6587\u4ef6
		{
			printf("Open Failed.\n");
			return;
		} 
	for(iter=store.begin();iter!=store.end();iter++)
		{
			parm=*iter;
			if(parm.value!=1)
				continue;
			sprintf(buffer,"valid:%d_pan:%f_title:%f",parm.value,parm.ptzpan,parm.ptztitle);
			if(filefp!=NULL)
	  			fprintf(filefp,"%s\n",buffer);
			else 
				break;
			fflush(filefp);
		}
	if(filefp!=NULL)
		fclose(filefp);
	

}
void Store::erase()
{
	
	if ((filefp=fopen(DIRRECTDIR,"w"))==NULL)			//\u6253\u5f00\u6307\u5b9a\u6587\u4ef6\uff0c\u5982\u679c\u6587\u4ef6\u4e0d\u5b58\u5728\u5219\u65b0\u5efa\u8be5\u6587\u4ef6
		{
			printf("Open Failed.\n");
			return;
		} 
	if(filefp!=NULL)
		fclose(filefp);

	char cmdBuf[128];
	sprintf(cmdBuf, "rm %s", DIRRECTDIR);
	system(cmdBuf);
	store.clear();
	

}
void Store::gostore(int num)
{
	list<storepam>::iterator iter;
	storepam parm;
	//iter=store.begin()+4;//+id;
	int count=0;
	for(iter=store.begin();iter!=store.end();iter++)
		{
			//parm=*iter;
			if(count==num)
				break;
			count++;
		}
	if(count>=store.size())
		return;
	parm=*iter;


	Plantformpzt::getinstance()->setpanopanpos(parm.ptzpan);

	Plantformpzt::getinstance()->setpanotitlepos(parm.ptztitle);
	Plantformpzt::getinstance()->getpanopanpos();
	Plantformpzt::getinstance()->getpanotitlepos();
	
	Plantformpzt::getinstance()->Enbalecallback(Plantformpzt::PRESETGO,parm.ptzpan,parm.ptztitle);
	
	
	//store.erase(iter);

}
void Store::reload()
{
	save();
	load();

}

void Store::registorfun()
{

	CMessage::getInstance()->MSGDRIV_register(MSGID_EXT_INPUT_StoreMod,Storemodevent,0);
/*
	CMessage::getInstance()->MSGDRIV_register(MSGID_EXT_INPUT_WorkModeCTRL,workmod,0);
	CMessage::getInstance()->MSGDRIV_register(MSGID_EXT_INPUT_SIGLEinterrupt,singlecircle,0);
	CMessage::getInstance()->MSGDRIV_register(MSGID_EXT_INPUT_Updatapano,updatepano,0);
	CMessage::getInstance()->MSGDRIV_register(MSGID_EXT_INPUT_MouseEvent,mouseevent,0);
*/
	//MSGID_EXT_INPUT_WorkModeCTRL
}


void Store::Storemodevent(long lParam)
{
	if(lParam==Status::STOREGO)
		{
			int num=Status::getinstance()->storegonum;
			instance->gostore(num);
			

		}
	else if(lParam==Status::STORESAVE)
		{
			int num=Status::getinstance()->storesavenum;
			int mod=Status::getinstance()->storesavemod;
			if(mod==Status::STORESAVENUM)
				{
					instance->addstore();
					instance->save();
					printf("++++++++++++++++++%s+++++++++++++++++++\n",__func__);

				}
			else if(mod==Status::STOREERASENUM)
				{
				
					instance->erasestore(num);
					instance->save();
						

				}
			else if(mod==Status::STOREDEFAULT)
				{
					instance->erase();

				}
			
			//instance->gostore(num);

			
			

		}

}

Store*Store::getinstance()
{
	if(instance==NULL)
		instance=new Store();
	return instance;
}

