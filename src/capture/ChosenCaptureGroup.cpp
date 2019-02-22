#include "ChosenCaptureGroup.h"
#include"HDV4lcap.h"
#include "linux/videodev2.h"
ChosenCaptureGroup ChosenCaptureGroup::TVChosenGroup(TV_WIDTH,TV_HEIGHT,2,1);
ChosenCaptureGroup ChosenCaptureGroup::HOTChosenGroup(HOT_WIDTH,HOT_HEIGHT,1,1);
static HDAsyncVCap4* pHDAsyncVCap[2]={0};

ChosenCaptureGroup::ChosenCaptureGroup(unsigned int w,unsigned int h,int NCHAN,unsigned int capCount):
		HDCaptureGroup(w,h,NCHAN,capCount)
{ }

void  ChosenCaptureGroup::CreateProducers()
{
		int dev_id=TV_DEV_ID;
		if(pHDAsyncVCap[dev_id]==NULL)
			pHDAsyncVCap[dev_id] = new HDAsyncVCap4(auto_ptr<BaseVCap>( new HDv4l_cam(dev_id,TV_WIDTH,TV_HEIGHT,V4L2_PIX_FMT_YUYV,CV_8UC2,MEMORY_LOCKED)),dev_id);
		dev_id=HOT_DEV_ID;
		if(pHDAsyncVCap[dev_id]==NULL)
			pHDAsyncVCap[dev_id] = new HDAsyncVCap4(auto_ptr<BaseVCap>( new HDv4l_cam(dev_id,HOT_WIDTH,HOT_HEIGHT,V4L2_PIX_FMT_YUYV,CV_8UC2,MEMORY_LOCKED)),dev_id);
		
};

void  ChosenCaptureGroup::OpenProducers()
{
	int dev_id=TV_DEV_ID;
	 pHDAsyncVCap[dev_id]->Open();
	 dev_id=HOT_DEV_ID;
	 //pHDAsyncVCap[dev_id]->Open();
}

ChosenCaptureGroup::~ChosenCaptureGroup()
{
	for(int i=TV_DEV_ID ;i<=HOT_DEV_ID;i++)
	{
		if(pHDAsyncVCap[i]){
					delete pHDAsyncVCap[i];
					pHDAsyncVCap[i]= NULL;
		}
	}
}

ChosenCaptureGroup * ChosenCaptureGroup::GetTVInstance()
{
	int queueid[1]={TV_DEV_ID};
	int count=1;
	static bool once =true;
	if(once){
		TVChosenGroup.init(queueid,count);
		once =false;
	}
	return &TVChosenGroup;
}


ChosenCaptureGroup * ChosenCaptureGroup::GetHOTInstance()
{
	int queueid[1]={HOT_DEV_ID};
	int count=1;
	static bool once =true;
	if(once){
		HOTChosenGroup.init(queueid,count);
		once =false;
	}
	return &HOTChosenGroup;
}


