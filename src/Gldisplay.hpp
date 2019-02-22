/*
 * Gldisplay.hpp
 *
 *  Created on: 2018年9月14日
 *      Author: fsmdn113
 */

#ifndef GLDISPLAY_HPP_
#define GLDISPLAY_HPP_

#include "GL/glew.h"
#include "GL/glut.h"
#include <math.h>
#include <iostream>
#include "opencv2/opencv.hpp"
#include "opencv2/highgui.hpp"
#include "opencv/highgui.h"

#include "config.h"

#include "osa.h"
#include "osa_thr.h"
#include "osa_buf.h"
#include "osa_sem.h"

using namespace cv;
using namespace std;
#define DS_CHAN_MAX         (4)
typedef struct {
	int w;
	int h;
	int c;
}GLMain_Size;

typedef struct {
	int x;
	int y;
	int w;
	int h;
}GLMain_Rect;

typedef struct {
	float x;
	float y;
	float w;
	float h;
}GLMain_fRect;

typedef struct 
{
	int video_chId;
	GLMain_Rect displayrect;
	bool bCrop;
	GLMain_Rect croprect;
	GLMain_fRect bindrect;
	bool bBind;
	float transform[4][4];
}GLMain_Render;
typedef struct _glmain_init_param{
	bool bFullScreen;
	int winPosX;
	int winPosY;
	int winWidth;
	int winHeight;
	int disFPS;
	bool bScript;
	char szScriptFile[256];
	GLMain_Size channelsSize[DS_CHAN_MAX];
	int nChannels;
	int nQueueSize;
	int memType;
	//void (*displayfunc)(void);
	void (*mousefunc)(int button, int state, int x, int y);
	//void (*reshapefunc)(int width, int height);
	void (*keyboardfunc)(unsigned char key, int x, int y);
	void (*keySpecialfunc)( int, int, int );
	void (*visibilityfunc)(int state);
	void (*timerfunc)(int value);
	void (*idlefunc)(void);
	void (*closefunc)(void);
	int timerfunc_value;//context
	int timerInterval;//ms
}GLMain_InitPrm;


class GLMain{
public:
	GLMain();
	~GLMain();
	void parseArgs(int argc, char** argv);
	void initGlut(int argc, char **argv);
	void captureUSB(GLubyte *ptr);
	void captureCSI(GLubyte *ptr);
	static void DrawGLScene();
	static void DrawIdle();
	static void ReSizeGLScene(int Width, int Height);
	static void keyPressed(unsigned char key, int x, int y);
	static void specialkeyPressed (int key, int x, int y);
	static void mouseButtonPress(int button, int state, int x, int y);
	static void mouseMotionPress(int x, int y);
	int start(int argc, char** argv,void *parma);
	void mainloop();

public:
	//static GLMain*gThis;
	OSA_BufHndl m_bufQue[DS_CHAN_MAX];
	OSA_BufInfo *queueinfo[DS_CHAN_MAX];
	OSA_BufInfo *processqueueinfo[IMAGEQUEUE];
	OSA_BufHndl *IPocess_bufQue[IMAGEQUEUE];
	int queuenum;
private:
};


#endif /* GLDISPLAY_HPP_ */
