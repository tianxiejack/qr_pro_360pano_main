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

typedef void(*PFN_PBOFILLBUFFER)(GLubyte *,int);

class PBOManager{
public:
    PBOManager(unsigned int PBOchcnt, unsigned int w = DEFAULT_IMAGE_WIDTH, unsigned int h=DEFAULT_IMAGE_HEIGHT, unsigned int cc=4,GLenum format = GL_BGRA);
	~PBOManager();
	bool IsInitOK();
	void sendData(GLuint textureId, PFN_PBOFILLBUFFER fxn, GLuint idx, bool bPBO=true);
	void sendSubData(GLuint textureId, PFN_PBOFILLBUFFER fxn,GLuint idx, bool bPBO,int start_x,int start_y,int sub_width,int sub_height,GLenum sub_pixel_format,GLubyte * pic);
	void flipPBOBit(){bUsePBO = !bUsePBO;};
private:
	void sendDataPBO(GLuint textureId, PFN_PBOFILLBUFFER fxn, GLuint idx);
	void sendSubDataPBO(GLuint textureId, PFN_PBOFILLBUFFER fxn,GLuint idx, int start_x,int start_y,int sub_width,int sub_height,GLenum sub_pixel_format,GLubyte * pic);
	void sendDataNoPBO(GLuint textureId, PFN_PBOFILLBUFFER fxn, GLuint idx);
	void sendSubDataNoPBO(GLuint textureId, PFN_PBOFILLBUFFER fxn,GLuint idx,int start_x,int start_y,int sub_width,int sub_height,GLenum sub_pixel_format,GLubyte * pic);
	unsigned int PBOChannelCount;// stitching cam + one individual video
	unsigned int PBOBufferCount;
	unsigned int width, height, chncnt;
	GLenum pixel_format;
	unsigned int    dataSize;
	unsigned int* pIndex;						//latest pbo index
	int pboMode;							//single/double buffer mode
	GLuint *pboIds;                   // IDs of PBO
	bool bUsePBO;
};


#endif /* PBOMANAGER_H_ */
