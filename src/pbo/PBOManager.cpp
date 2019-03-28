/*
 * PBOManager.cpp
 *
 *  Created on: Nov 10, 2016
 *      Author: hoover
 */
#include "GL/glew.h"
#include "GL/glext.h"

#include "glInfo.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "pboProcessSrcThread.h"
#include <osa_sem.h>
#include "PBOManager.h"
#include"assert.h"
#include"stdio.h"
using namespace std;

PBOBase::PBOBase(unsigned int PBOchcnt, unsigned int w, unsigned int h, unsigned int cc,GLenum format,unsigned int pbo_mode,bool buse_pbo):
		width(w),height(h),chncnt(cc),pboMode(pbo_mode),pixel_format(format),bUsePBO(buse_pbo),PBOChannelCount(PBOchcnt)
{
	dataSize=w*h*cc;
	PBOBufferCount = pboMode*PBOChannelCount;
	pboIds = new GLuint[ PBOBufferCount];
	pIndex = new unsigned int[PBOChannelCount];
	for(int i = 0; i <PBOChannelCount; i++ ){
		pIndex[i] = i;
	}
	nowPboId=-1;
}

PBOBase::~PBOBase()
{

}

/***********************PBOSender******************************/

PBOSender::PBOSender(unsigned int PBOchcnt, unsigned int w , unsigned int h, unsigned int cc,GLenum format,float ratio_1,float ratio_2):
		PBOBase(PBOchcnt,w,h,cc,format,1,true)
{
	m_ratio_1=ratio_1;
	m_ratio_2=ratio_2;
	dataSize=w*h*cc;
	PBOBufferCount = pboMode*PBOChannelCount;
	pboIds = new GLuint[ PBOBufferCount];
	pIndex = new unsigned int[PBOChannelCount];
	for(int i = 0; i <PBOChannelCount; i++ ){
		pIndex[i] = i;
	}
}

PBOSender::~PBOSender()
{
	glDeleteBuffersARB(PBOBufferCount,pboIds);
	delete [] pboIds;
	delete [] pIndex;
}

bool PBOSender::Init()
{
    // get OpenGL info

	 if(1)
	    {
	        // create 2 pixel buffer objects, you need to delete them when program exits.
	        // glBufferDataARB with NULL pointer reserves only memory space.
	        glGenBuffersARB(PBOBufferCount, pboIds);
		for(int i = 0; i < PBOBufferCount; i++){
	        glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, pboIds[i]);
	        glBufferDataARB(GL_PIXEL_UNPACK_BUFFER_ARB, dataSize, 0, GL_STREAM_DRAW_ARB);
		}
	        glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, 0);
	    }
	 return true;
}

void PBOSender::sendDataNoPBO(GLEnv &env,GLuint textureId, PFN_PBOFILLBUFFER fxn, GLuint idx)
{
       static GLubyte buffer[DEFAULT_IMAGE_WIDTH*DEFAULT_IMAGE_HEIGHT*DEFAULT_IMAGE_DEPTH];
	   if(fxn){
		(*fxn)(buffer,idx,env);
	   	}
	glBindTexture(GL_TEXTURE_2D, textureId);
       glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, pixel_format, GL_UNSIGNED_BYTE, buffer);
}

void PBOSender::sendDataPBO(GLEnv &env,GLuint textureId, PFN_PBOFILLBUFFER fxn, GLuint idx)
{
	int index = 0, nextIndex = 0;                  // pbo index used for next frame
	idx = idx %PBOChannelCount;
	GLenum error= 0;
	// "index" is used to copy pixels from a PBO to a texture object
	// "nextIndex" is used to update pixels in a PBO
	if(pboMode == 1)
	{
		// In single PBO mode, the index and nextIndex are set to 0
		index = nextIndex = pIndex[idx];
	}
	else if(pboMode == 2)
	{
		// In dual PBO mode, increment current index first then get the next index
		index = pIndex[idx] = (pIndex[idx]+PBOChannelCount) % PBOBufferCount;
		nextIndex = (index + PBOChannelCount) % PBOBufferCount;
	}
	// bind the texture and PBO
	glBindTexture(GL_TEXTURE_2D, textureId);
	error =glGetError();
				if(GL_NO_ERROR != error){
				//	printf("%d\n",idx);
					cout<<"0textureId GLError = "<<  gluErrorString(error)<<endl;
				}
	glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, pboIds[index]);
	error =glGetError();
					if(GL_NO_ERROR != error){
					//	printf("%d\n",idx);
						cout<<"0pboIds GLError = "<<  gluErrorString(error)<<endl;
					}
	// copy pixels from PBO to texture object
	// Use offset instead of pointer.
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 1920, 1080, pixel_format, GL_UNSIGNED_BYTE, 0);

	// bind PBO to update pixel values
	glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, pboIds[nextIndex]);
	error =glGetError();
			if(GL_NO_ERROR != error){
				cout<<"1 GLError = "<<  gluErrorString(error)<<endl;
			}
	// map the buffer object into client's memory
	// Note that glMapBufferARB() causes sync issue.
	// If GPU is working with this buffer, glMapBufferARB() will wait(stall)
	// for GPU to finish its job. To avoid waiting (stall), you can call
	// first glBufferDataARB() with NULL pointer before glMapBufferARB().
	// If you do that, the previous data in PBO will be discarded and
	// glMapBufferARB() returns a new allocated pointer immediately
	// even if GPU is still working with the previous data.
	GLubyte* ptr = (GLubyte*)glMapBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, GL_WRITE_ONLY_ARB);
	if(ptr && fxn)
	{
		// update data directly on the mapped buffer
		(*fxn)(ptr,idx,env);
// let OpenGL release it
		glUnmapBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB); // release pointer to mapping buffer
		error =glGetError();
		if(GL_NO_ERROR != error){
			cout<<"3 GLError = "<<  gluErrorString(error)<<endl;
		}
	}
	else
	{
		cout<<"PBOSender Err! ptr="<<(ptr!=NULL)<<", fxn="<<(fxn!=NULL)<<endl;
	}

	glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, pboIds[nextIndex]);
	error =glGetError();
			if(GL_NO_ERROR != error){
				cout<<"4 GLError = "<< gluErrorString(error)<<endl;
			}
	// copy pixels from PBO to texture object
	// Use offset instead of pointer.
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, pixel_format, GL_UNSIGNED_BYTE, 0);
    // it is good idea to release PBOs with ID 0 after use.
    // Once bound with 0, all pixel operations behave normal ways.
    glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, 0);
    error =glGetError();
    if(GL_NO_ERROR != error){
    	cout<<" 5 GLError = "<< gluErrorString(error)<<endl;
    }
    glFlush();
}

void PBOSender::sendData(GLEnv &env,GLuint textureId, PFN_PBOFILLBUFFER fxn, GLuint idx,bool bPBO)
{
	if(bPBO&&bUsePBO)
             sendDataPBO(env,textureId, fxn, idx);
	else
		sendDataNoPBO(env,textureId, fxn, idx);
}

/***********************PBOReceiver******************************/

PBOReceiver::PBOReceiver(int idx,unsigned int PBOchcnt, unsigned int w, unsigned int h, unsigned int cc,GLenum format):
		PBOBase(PBOchcnt,w,h,cc,format,1,true),midx(idx),trheadStartOnce(true)
{
	dataSize=w*h*cc;
	PBOBufferCount = pboMode*PBOChannelCount;
	pboIds = new GLuint[ PBOBufferCount];
	pIndex = new unsigned int[PBOChannelCount];
	for(int i = 0; i <PBOChannelCount; i++ ){
		pIndex[i] = i;
	}
	for(int i=0;i<2;i++)
		pPixelBuffer[i]=(unsigned char *)malloc(dataSize);
	pSemPBO=(OSA_SemHndl *)malloc(sizeof(OSA_SemHndl)) ;
}
PBOReceiver::~PBOReceiver()
{
	glDeleteBuffersARB(PBOBufferCount,pboIds);
	delete [] pboIds;
	delete [] pIndex;
	free (pSemPBO);
}

bool PBOReceiver::Init()
{
	int ret=OSA_semCreate(pSemPBO,1,0);
	if(ret<0)
	{
		printf("OSA_semCreate failed\n");
	}
	 if(1)
	    {
	        // create 2 pixel buffer objects, you need to delete them when program exits.
	        // glBufferDataARB with NULL pointer reserves only memory space.
	        glGenBuffersARB(PBOBufferCount, pboIds);
	       	 {
	       			for(int i = 0; i < PBOBufferCount; i++){
	       			        glBindBufferARB(GL_PIXEL_PACK_BUFFER_ARB, pboIds[i]);
	       			        glBufferDataARB(GL_PIXEL_PACK_BUFFER_ARB, dataSize, 0, GL_STREAM_READ_ARB);
	       				}
	       	 }
	       			        glBindBufferARB(GL_PIXEL_PACK_BUFFER_ARB, 0);
	    }
	 return true;
}

void PBOReceiver::getDataPBO(int startX,int startY,int w,int h, GLuint idx)
{
	static timeval startT[20]={0};
	int t[10]={0};
	gettimeofday(&startT[0],0);

	int index = 0, nextIndex = 0;                  // pbo index used for next frame
	idx = idx %PBOChannelCount;

	GLenum error= 0;
	// "index" is used to copy pixels from a PBO to a texture object
	// "nextIndex" is used to update pixels in a PBO
	if(pboMode == 1)
	{
		// In single PBO mode, the index and nextIndex are set to 0
		index = nextIndex = pIndex[idx];
	}
	else if(pboMode == 2)
	{
		// In dual PBO mode, increment current index first then get the next index
		index = pIndex[idx] = (pIndex[idx]+PBOChannelCount) % PBOBufferCount;
		nextIndex = (index + PBOChannelCount) % PBOBufferCount;
	}
//read data from FBO to PBO
//glReadPixels() will return immediately
	glBindBufferARB(GL_PIXEL_PACK_BUFFER_ARB, pboIds[index]);
	gettimeofday(&startT[4],0);
	glReadPixels(startX,startY,w,h,pixel_format,GL_UNSIGNED_BYTE,0);
	gettimeofday(&startT[5],0);
	error =glGetError();
	if(GL_NO_ERROR != error){
	//	cout<<"0 GLError = "<<gluErrorString(error)<<endl;
	}
	//t[2]=(startT[5].tv_sec-startT[4].tv_sec)*1000000+(startT[5].tv_usec-startT[4].tv_usec);
	//printf("deltatimet[5]-t[4] =%d us\n",t[2]);


	// copy pixels from PBO to texture object
	// Use offset instead of pointer.
	// bind PBO to update pixel values
	gettimeofday(&startT[6],0);
	glBindBufferARB(GL_PIXEL_PACK_BUFFER_ARB, pboIds[nextIndex]);
	error =glGetError();
	if(GL_NO_ERROR != error){
	//	cout<<"1 GLError = "<<gluErrorString(error)<<endl;
	}
	// map the buffer object into client's memory
	// Note that glMapBufferARB() causes sync issue.
	// If GPU is working with this buffer, glMapBufferARB() will wait(stall)
	// for GPU to finish its job. To avoid waiting (stall), you can call
	// first glBufferDataARB() with NULL pointer before glMapBufferARB().
	// If you do that, the previous data in PBO will be discarded and
	// glMapBufferARB() returns a new allocated pointer immediately
	// even if GPU is still working with the previous data.
	glBufferDataARB(GL_PIXEL_PACK_BUFFER_ARB, dataSize, NULL, /*GL_DYNAMIC_DRAW_ARB*/GL_STREAM_READ_ARB);
	error =glGetError();
	if(GL_NO_ERROR != error){
	//		cout<<"2 GLError = "<< gluErrorString(error)<<endl;
		}
	pPixelBuffer[nextIndex] = (GLubyte*)glMapBufferARB(GL_PIXEL_PACK_BUFFER_ARB, GL_READ_ONLY_ARB);
	nowPboId=nextIndex;
			if(pPixelBuffer[nextIndex])
	{
				OSA_semSignal(pSemPBO);
		if(trheadStartOnce)
		{
			start_pbo_process_thread(midx);
			trheadStartOnce=false;
		}
// let OpenGL release it
		glUnmapBufferARB(GL_PIXEL_PACK_BUFFER_ARB); // release pointer to mapping buffer
		error =glGetError();
		if(GL_NO_ERROR != error){
	//		cout<<"3 GLError = "<< gluErrorString(error)<<endl;
		}
	}
	else
	{
		cout<<"PBOSender Err!"<<endl;
	}
	// copy pixels from PBO to texture object
	// Use offset instead of pointer.
    // it is good idea to release PBOs with ID 0 after use.
    // Once bound with 0, all pixel operations behave normal ways.
    glBindBufferARB(GL_PIXEL_PACK_BUFFER_ARB, 0);
    error =glGetError();
    if(GL_NO_ERROR != error){
 //   	cout<<" 5 GLError = "<< gluErrorString(error)<<endl;
    }
}

void PBOReceiver::getData(int startX,int startY,int w,int h,GLuint idx, bool bPBO)
{
	if(bPBO&&bUsePBO)
		getDataPBO(startX,startY, w,h,idx);
	else
	{

	}
}

void PBOReceiver::callbackPBODraw()
{
	int x=0,y=0,w=0,h=0;
	if(midx==	ROI_C
		||midx==	ROI_A
		||midx==ROI_B
			)
	{
		x=0;
		y=0;
		w=ROIFBOW;
		h=ROIFBOH;
	}
	else if(midx==PANO_PIC)
	{
		x=0;
		y=0;
		w=PANO360FBOW;
		h=PANO360FBOH;
	}
	else
	{
		printf("Is this IDX right?\n");
		assert(false);
	}

      getData(x,y,w,h);//TODO
}
