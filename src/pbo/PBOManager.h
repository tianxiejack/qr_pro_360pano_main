/*
 * PBOManager.h
 *
 *  Created on: Nov 10, 2016
 *      Author: hoover
 */

#ifndef PBOMANAGER_H_
#define PBOMANAGER_H_

#include "GL/gl.h"
#include "StlGlDefines.h"
#include "stdio.h"
#include "FBOManager.h"
#include "osa_sem.h"
class GLEnv;
typedef void(*PFN_PBOFILLBUFFER)(GLubyte *,int,GLEnv &);

#define PBO_ALTERNATE_NUM	2
class GLEnv;
class PBOBase
{
public:
		PBOBase(unsigned int PBOchcnt=4, unsigned int w = DEFAULT_IMAGE_WIDTH, unsigned int h=DEFAULT_IMAGE_HEIGHT, unsigned int cc=3,GLenum format = GL_BGR
			,unsigned int pbo_mode=2,bool buse_pbo=true);
	virtual ~PBOBase()=0;
protected:
	unsigned int PBOChannelCount;// stitching cam + one individual video
	unsigned int PBOBufferCount;
	unsigned int width, height, chncnt;
	GLenum pixel_format;
	unsigned int    dataSize;
	unsigned int* pIndex;						//latest pbo index
	int pboMode;							//single/double buffer mode
	GLuint *pboIds;                   // IDs of PBO
	bool bUsePBO;
	 int nowPboId;
};


class PBOSender:public PBOBase
{
public:
	PBOSender(unsigned int PBOchcnt=4, unsigned int w = DEFAULT_IMAGE_WIDTH, unsigned int h=DEFAULT_IMAGE_HEIGHT, unsigned int cc=3,GLenum format = GL_BGR,float ratio_1=1,float ratio_2=1);
	~PBOSender();
	bool Init();
	void sendData(GLEnv &env,GLuint textureId, PFN_PBOFILLBUFFER fxn, GLuint idx,bool bPBO=true);
private:
    float m_ratio_1;
    float m_ratio_2;
	void sendDataPBO(GLEnv &env,GLuint textureId, PFN_PBOFILLBUFFER fxn, GLuint idx);
	void sendDataNoPBO(GLEnv &env,GLuint textureId, PFN_PBOFILLBUFFER fxn, GLuint idx);
};

class PBOReceiver :public PBOBase,
										 public InterfacepboDrawCB
{
public:
	PBOReceiver(int idx,unsigned int PBOchcnt=2, unsigned int w = 1920, unsigned int h=1080, unsigned int cc=3,GLenum format = GL_BGR);
	~PBOReceiver();
	bool Init();
	inline int getCurrentPBOIdx(){return nowPboId;};
	GLubyte** getPixelBuffer(int idx){return &pPixelBuffer[idx];};
    virtual void callbackPBODraw();//TODO
    pOSA_SemHndl getSemPBO(){return pSemPBO;};
private:
    bool trheadStartOnce;
    int midx;
    pOSA_SemHndl pSemPBO;
	void getDataPBO(int startX,int startY,int w,int h,GLuint idx=0);
	void delete_POBReceiver();
	void getData(int startX=0,int startY=0,int w=1920,int h=1080,  GLuint idx=0, bool bPBO=true);

	 GLubyte* pPixelBuffer[PBO_ALTERNATE_NUM];
};

#endif /* PBOMANAGER_H_ */
