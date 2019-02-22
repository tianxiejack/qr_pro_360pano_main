/*
 * PBOManager.cpp
 *
 *  Created on: Nov 10, 2016
 *      Author: hoover
 */
#include "GL/glew.h"
#include "PBOManager.h"
#include "GL/glext.h"

#include "glInfo.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>


using namespace std;
#define gluErrorString(a) a

PBOManager::PBOManager(unsigned int PBOchcnt, unsigned int w, unsigned int h, unsigned int cc,GLenum format):
		width(w),height(h),chncnt(cc),pboMode(2),pixel_format(format),bUsePBO(true),PBOChannelCount(PBOchcnt)
{
	dataSize=w*h*cc;
	PBOBufferCount = pboMode*PBOChannelCount;
	pboIds = new GLuint[ PBOBufferCount];
	pIndex = new unsigned int[PBOChannelCount];
	for(int i = 0; i <PBOChannelCount; i++ ){
		pIndex[i] = i;
	}
}

PBOManager::~PBOManager()
{

	glDeleteBuffersARB(PBOBufferCount,pboIds);
	delete [] pboIds;
	delete [] pIndex;

}

bool PBOManager::IsInitOK()
{
	bool pboSupported = false;
    // get OpenGL info
    glInfo glInfo;
    glInfo.getInfo();

	 if(glInfo.isExtensionSupported("GL_ARB_pixel_buffer_object"))
	    {
	        pboSupported = true;
	        std::cout << "Video card supports GL_ARB_pixel_buffer_object." << std::endl;
	    }
	    else
	    {
	        pboSupported = false;
	        std::cout << "Video card does NOT support GL_ARB_pixel_buffer_object." << std::endl;
	    }
	 if(pboSupported)
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

	 return pboSupported;
}

void PBOManager::sendData(GLuint textureId, PFN_PBOFILLBUFFER fxn, GLuint idx, bool bPBO)
{
	if(bPBO&&bUsePBO)
	{

             sendDataPBO(textureId, fxn, idx);

	}
	else
	{

			sendDataNoPBO(textureId, fxn, idx);
		
	}
}

void PBOManager::sendDataPBO(GLuint textureId, PFN_PBOFILLBUFFER fxn, GLuint idx)
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
	glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, pboIds[index]);
	error =glGetError();
			if(GL_NO_ERROR != error){
//				cout<<"0 GLError = "<<gluErrorString(error)<<endl;
			}
	// copy pixels from PBO to texture object
	// Use offset instead of pointer.
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, pixel_format, GL_UNSIGNED_BYTE, 0);

	// bind PBO to update pixel values
	glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, pboIds[nextIndex]);
	error =glGetError();
			if(GL_NO_ERROR != error){
				cout<<"1 GLError = "<<gluErrorString(error)<<endl;
			}
	// map the buffer object into client's memory
	// Note that glMapBufferARB() causes sync issue.
	// If GPU is working with this buffer, glMapBufferARB() will wait(stall)
	// for GPU to finish its job. To avoid waiting (stall), you can call
	// first glBufferDataARB() with NULL pointer before glMapBufferARB().
	// If you do that, the previous data in PBO will be discarded and
	// glMapBufferARB() returns a new allocated pointer immediately
	// even if GPU is still working with the previous data.
	glBufferDataARB(GL_PIXEL_UNPACK_BUFFER_ARB, dataSize, 0, /*GL_DYNAMIC_DRAW_ARB*/GL_STREAM_DRAW_ARB);
	error =glGetError();
	if(GL_NO_ERROR != error){
			cout<<"2 GLError = "<<gluErrorString(error)<<endl;
		}
	GLubyte* ptr = (GLubyte*)glMapBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, GL_WRITE_ONLY_ARB);

	if(ptr && fxn)
	{
		// update data directly on the mapped buffer
		(*fxn)(ptr,idx);
// let OpenGL release it
		glUnmapBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB); // release pointer to mapping buffer
		error =glGetError();
		if(GL_NO_ERROR != error){
			cout<<"3 GLError = "<<gluErrorString(error)<<endl;
		}
	}
	else
	{
		cout<<"PBOManager Err! ptr="<<(ptr!=NULL)<<", fxn="<<(fxn!=NULL)<<endl;
	}

	glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, pboIds[nextIndex]);
	error =glGetError();
			if(GL_NO_ERROR != error){
				cout<<"4 GLError = "<<gluErrorString(error)<<endl;
			}
	// copy pixels from PBO to texture object
	// Use offset instead of pointer.
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, pixel_format, GL_UNSIGNED_BYTE, 0);
	
    // it is good idea to release PBOs with ID 0 after use.
    // Once bound with 0, all pixel operations behave normal ways.
    glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, 0);
    error =glGetError();
    if(GL_NO_ERROR != error){
    	cout<<" 5 GLError = "<<gluErrorString(error)<<endl;
    }
    glFlush();
}
void PBOManager::sendDataNoPBO(GLuint textureId, PFN_PBOFILLBUFFER fxn, GLuint idx)
{
       static GLubyte buffer[DEFAULT_IMAGE_WIDTH*DEFAULT_IMAGE_HEIGHT*DEFAULT_IMAGE_DEPTH];
	   if(fxn){
		(*fxn)(buffer,idx);
	   	}
	glBindTexture(GL_TEXTURE_2D, textureId);
       glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, pixel_format, GL_UNSIGNED_BYTE, buffer);
}

void PBOManager::sendSubData(GLuint textureId, PFN_PBOFILLBUFFER fxn,GLuint idx, bool bPBO,int start_x,int start_y,int sub_width,int sub_height,GLenum sub_pixel_format,GLubyte * pic)
{
	if(bPBO&&bUsePBO)
	{

			sendSubDataPBO(textureId,fxn,idx,start_x,start_y,sub_width,sub_height,sub_pixel_format,pic);

	}
	else
	{

			sendSubDataNoPBO(textureId,fxn, idx,start_x,start_y,sub_width,sub_height,sub_pixel_format,pic);

	
	}
}


void PBOManager::sendSubDataPBO(GLuint textureId, PFN_PBOFILLBUFFER fxn, GLuint idx,int start_x,int start_y,int sub_width,int sub_height,GLenum sub_pixel_format,GLubyte * pic)
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
	glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, pboIds[index]);
	error =glGetError();
			if(GL_NO_ERROR != error){
//				cout<<"0 GLError = "<<gluErrorString(error)<<endl;
			}
	// copy pixels from PBO to texture object
	// Use offset instead of pointer.
	glTexSubImage2D(GL_TEXTURE_2D, 0, start_x, start_y, sub_width, sub_height, sub_pixel_format, GL_UNSIGNED_BYTE, 0);

	// bind PBO to update pixel values
	glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, pboIds[nextIndex]);
	error =glGetError();
			if(GL_NO_ERROR != error){
				cout<<"1 GLError = "<<gluErrorString(error)<<endl;
			}
	// map the buffer object into client's memory
	// Note that glMapBufferARB() causes sync issue.
	// If GPU is working with this buffer, glMapBufferARB() will wait(stall)
	// for GPU to finish its job. To avoid waiting (stall), you can call
	// first glBufferDataARB() with NULL pointer before glMapBufferARB().
	// If you do that, the previous data in PBO will be discarded and
	// glMapBufferARB() returns a new allocated pointer immediately
	// even if GPU is still working with the previous data.
	glBufferDataARB(GL_PIXEL_UNPACK_BUFFER_ARB, dataSize, 0, /*GL_DYNAMIC_DRAW_ARB*/GL_STREAM_DRAW_ARB);
	error =glGetError();
	if(GL_NO_ERROR != error){
			cout<<"2 GLError = "<<gluErrorString(error)<<endl;
		}
	GLubyte* ptr = (GLubyte*)glMapBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, GL_WRITE_ONLY_ARB);

	if(ptr)
	{
		// update data directly on the mapped buffer
		(*fxn)(ptr,idx);
// let OpenGL release it
		glUnmapBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB); // release pointer to mapping buffer
		error =glGetError();
		if(GL_NO_ERROR != error){
			cout<<"3 GLError = "<<gluErrorString(error)<<endl;
		}
	}
	else
	{
		//cout<<"PBOManager Err! ptr="<<(ptr!=NULL)<<", fxn="<<(fxn!=NULL)<<endl;
	}

	glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, pboIds[nextIndex]);
	error =glGetError();
			if(GL_NO_ERROR != error){
				cout<<"4 GLError = "<<gluErrorString(error)<<endl;
			}
	// copy pixels from PBO to texture object
	// Use offset instead of pointer.
	glTexSubImage2D(GL_TEXTURE_2D, 0, start_x, start_y, sub_width, sub_height, sub_pixel_format, GL_UNSIGNED_BYTE, 0);
	
    // it is good idea to release PBOs with ID 0 after use.
    // Once bound with 0, all pixel operations behave normal ways.
    glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, 0);
    error =glGetError();
    if(GL_NO_ERROR != error){
    	cout<<" 5 GLError = "<<gluErrorString(error)<<endl;
    }
    glFlush();
}
void PBOManager::sendSubDataNoPBO(GLuint textureId, PFN_PBOFILLBUFFER fxn, GLuint idx,int start_x,int start_y,int sub_width,int sub_height,GLenum sub_pixel_format,GLubyte * pic)
{
       static GLubyte buffer[DEFAULT_IMAGE_WIDTH*DEFAULT_IMAGE_HEIGHT*DEFAULT_IMAGE_DEPTH];
	   if(fxn){
		(*fxn)(buffer,idx);
	   	}
       glBindTexture(GL_TEXTURE_2D, textureId);
       glTexSubImage2D(GL_TEXTURE_2D, 0, start_x, start_y, sub_width, sub_height, sub_pixel_format, GL_UNSIGNED_BYTE, buffer);
}
