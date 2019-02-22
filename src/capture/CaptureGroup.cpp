/*
 * CaptureGroup.cpp
 *
 *  Created on: Dec 16, 2016
 *      Author: wang
 */

#include "CaptureGroup.h"
#include <stdbool.h>
#include <iostream>
#include <assert.h>
#include <string.h>


#if USE_12
#include"HDV4lcap.h"
#endif
using namespace std;

PseudoCaptureGroup PseudoCaptureGroup::pseudoCaptureGroup(0,0,0,0);

CaptureGroup::CaptureGroup(unsigned int capCount):m_TotalCamCount(capCount),m_currentIdx(0),
		width(DEFAULT_IMAGE_WIDTH),height(DEFAULT_IMAGE_HEIGHT),depth(DEFAULT_IMAGE_DEPTH),Defaultimage(NULL)
{
	capCamera = new pInterface_VCap[m_TotalCamCount];
	for(int i = 0; i < capCount; i ++){
		capCamera[i] = new PseudoVCap();
	}
	//memset((void*)m_V4LImag,0,sizeof(m_V4LImag));
	Defaultimage = new unsigned char [capCount*width* height * depth];
//	fillColorBar();
}
CaptureGroup::CaptureGroup(unsigned int wide,unsigned int height,int NCHAN,unsigned int capCount):
		m_TotalCamCount(capCount),m_currentIdx(0),width(wide),height(height),
		depth(NCHAN),Defaultimage(NULL)
{
	capCamera = new pInterface_VCap[m_TotalCamCount];
	for(int i = 0; i < capCount; i ++){
		capCamera[i] = new PseudoVCap();
	}
	//memset((void*)m_V4LImag,0,sizeof(m_V4LImag));
	Defaultimage = new unsigned char [capCount*width* height * depth];
//	fillColorBar();
}

void CaptureGroup::init(int *queueid,int count)
{
	CreateProducers();
	SetConsumers(GetConsumers(queueid,count));
	OpenProducers();
	Open();
}

CaptureGroup::~CaptureGroup()
{
	Close();
	for(int i=0; i < m_TotalCamCount; i ++){
		BaseVCap *pCamera = dynamic_cast<BaseVCap*>(capCamera[i]);
		if(pCamera){
			delete pCamera;
		}
		//delete update[i];
	}

	delete [] capCamera;
	delete [] Defaultimage;
}

void CaptureGroup::SetConsumers(vector<Consumer>  h)
{
     int size = h.size();
     for(int i = 0; i < size; i++){
    //re-oder the cameras according to idx
    	 for(int j = 0; j<size;j++){
    			 if(h[j].idx == i){
    				 Append(h[j].pcap);
    			 }
    	 }
     }
}


void CaptureGroup::fillColorBar()
{
	float factor = 18.45f;
	for(int i=0; i<m_TotalCamCount; i++)
	{
		char *pBuff = (char*)(& Defaultimage[i* width * height* depth]);
		unsigned int offset = (i+1)*width*height;
		for(int j = 0; j < width*height; j++)
		{
			int real = (int)factor;
			*pBuff++ = (int)(((j +offset%255)*(real*real)))&0xFF;
			*(pBuff++) = (int)(((j+offset%255)*real))&0xFF;
			*(pBuff++) = (int)(((m_TotalCamCount-1-i)*(j+offset%255)))&0xFF;//init a bluer stripe texture in the buffers.
			factor *= 3.30f;
			offset = (int)(offset*real)&0xff;
		}
	}
}

bool CaptureGroup:: Append(pInterface_VCap cap)
{
	if(m_currentIdx < m_TotalCamCount ){
		char * pImg = (char*)&Defaultimage[m_currentIdx * width * height* depth];
		// delete the pseudoCamera before assigning new one.
		delete capCamera[m_currentIdx];
		capCamera[m_currentIdx] = cap;
		cap->SetDefaultImg(pImg);
		m_currentIdx++;
		return true;
	}
	return false;
}
void CaptureGroup::Open()
{
	for(int i = 0; i < m_currentIdx; i ++){
		if(!capCamera[i]->Open()){
			cout<<"Error: fail to open camera["<<i<<"]"<<endl;
		}
	}
}

void CaptureGroup::Close()
{
	for(int i=0; i<m_currentIdx; i++)
	{
		capCamera[i]->Close();
	}
}

void CaptureGroup::captureCam(GLubyte *ptr, int index)
{
	capCamera[index]->Capture((char*)ptr);
}
void CaptureGroup::captureCamFish(GLubyte *ptr, int index)
{
	capCamera[index]->CaptureFish((char*)ptr);
}
bool CaptureGroup::getScannerPic(IplImage *pImage)
{
	if(capCamera[0])
	{
		capCamera[0]->Capture(pImage->imageData);
		return true;
	}
	return false;
}

bool CaptureGroup::saveCapImg()
{
	return true;
}



void CaptureGroup::saveOverLap()
{
}

bool CaptureGroup::saveSingleCapImg(int cam_num)
{
	char buf[32];
	int i=0;
	i=cam_num;
		sprintf(buf,"./data/%02d.bmp",i);
		if(capCamera[i])
			capCamera[i]->SavePic(buf);
		else
			printf("cap:%d error\n",i);

	return true;
}



vector<Consumer>  HDCaptureGroup::GetConsumers(int *queueid,int count)
{
	Consumer cons;
#if 0
	 for(int i=0;i<count;i++)
	 {
		 int qid=queueid[i];
		   cons.pcap = new HDVCap(qid,DEFAULT_IMAGE_WIDTH,DEFAULT_IMAGE_HEIGHT);
		   cons.idx = i;
		   v_cons.push_back(cons);
	 }
#endif
	   return v_cons;
}


PseudoCaptureGroup *PseudoCaptureGroup::GetInstance()
{
	static bool once =true;
	if(once){
		pseudoCaptureGroup.init(NULL,0);
		once =false;
	}
	return &pseudoCaptureGroup;
}

