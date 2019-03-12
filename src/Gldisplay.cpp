/*
 * Gldisplay.cpp
 *
 *  Created on: 2018年9月14日
 *      Author: fsmdn113
 */
#include"Gldisplay.hpp"
#include <stdio.h>
#include"Render.hpp"
#include <GL/glew.h>
#include "osa_image_queue.h"
#include"Queuebuffer.hpp"
#include"Comcontrl.hpp"
Render render;
static GLMain *gThis = NULL;
GLMain::GLMain()
{
	gThis=this;
	queuenum=0;
}
GLMain::~GLMain()
{
}
void GLMain::DrawGLScene()
{
	int fullcount=0;
	Queue*queuebuf=Queue::getinstance();
	
	for(int chId=0; chId<gThis->queuenum; chId++)
		gThis->queueinfo[chId]=image_queue_getFull(&gThis->m_bufQue[chId]);

/*
if(gThis->queueinfo[RTSP_QUE_ID]!=NULL)
{
	 static int a=0;
	static bool once=true;
	static unsigned char * ptr=NULL;

		if(once)
		{
			once =false;
			ptr=(unsigned char *)malloc(1920*1080*4);
		}
		memcpy(ptr,gThis->queueinfo[RTSP_QUE_ID]->virtAddr,1920*1080*3);
		 Mat localbgrImg(1080,1920,CV_8UC3,ptr);
			if (a < 300 &&(a++%10 == 1)) {
				char fname[50];
				sprintf(fname, "van_%d.bmp", a);
				imwrite(fname, localbgrImg);
			}
}
*/
	for(int chId=0; chId<gThis->queuenum; chId++)
		{
			if(gThis->queueinfo[chId]!=NULL)
				{
				
				//printf("render angle=%d\n",gThis->queueinfo[chId]->framegyroyaw);
				render.CaptureFrame(chId, gThis->queueinfo[chId]->width, gThis->queueinfo[chId]->height, gThis->queueinfo[chId]->channels, (unsigned char *)gThis->queueinfo[chId]->virtAddr);
				}
			else
				render.CaptureFrame(chId, 0, 0, 0, NULL);

			
		
		}
	//fullcount=queuebuf->getfullcount(Queue::FROMEPANOSTICH,0);
	
	for(int chId=0; chId<1; chId++)
		{
		//gThis->processqueueinfo[chId]=image_queue_peekFull(gThis->IPocess_bufQue[chId]);
		//if(gThis->processqueueinfo[chId].)
		fullcount = queuebuf->getfullcount(Queue::FROMEPANOSTICH,chId);
		if(fullcount>1)
		gThis->processqueueinfo[chId]=(OSA_BufInfo*)queuebuf->getfull(Queue::FROMEPANOSTICH,chId,OSA_TIMEOUT_NONE);
		else
		gThis->processqueueinfo[chId]=NULL;	

		}
	for(int chId=0; chId<1; chId++)
		{
			if(gThis->processqueueinfo[chId]!=NULL)
				{
				//OSA_printf("OK  PROCESS INFO");
				render.setPanoAngle( gThis->processqueueinfo[chId]->framegyroyaw);
				render.settailcut(gThis->processqueueinfo[chId]->tailcut);
				render.CaptureProcessFrame(chId, gThis->processqueueinfo[chId]->width, gThis->processqueueinfo[chId]->height, gThis->processqueueinfo[chId]->channels, (unsigned char *)gThis->processqueueinfo[chId]->virtAddr);
				}
			else
				render.CaptureProcessFrame(chId, 0, 0, 0, NULL);

		}
	
  	 render.RenderScene();
	 render.PostCaptureFrame();

	  for(int chId=0; chId<1; chId++)
		if(gThis->processqueueinfo[chId]!=NULL)
			queuebuf->putempty(Queue::FROMEPANOSTICH,chId,gThis->processqueueinfo[chId]);
			//image_queue_putEmpty(gThis->IPocess_bufQue[chId],gThis->processqueueinfo[chId]);

	 for(int chId=0; chId<gThis->queuenum; chId++)
		if(gThis->queueinfo[chId]!=NULL)
			image_queue_putEmpty(&gThis->m_bufQue[chId],gThis->queueinfo[chId]);

	glutPostRedisplay();
}
void GLMain::ReSizeGLScene(int Width, int Height)
{


	render.ReSizeGLScene(Width,Height);
	
	
}
void GLMain::keyPressed(unsigned char key, int x, int y)
{
	render.keyPressed(key,x,y);
}
void GLMain::specialkeyPressed(int key, int x, int y)
{
	render.specialkeyPressed(key,x,y);
}
void GLMain::mouseButtonPress(int button, int state, int x, int y)
{
	render.mouseButtonPress(button,state,x,y);
}
void GLMain::mouseMotionPress(int x, int y)
{
	render.mouseMotionPress(x,y);
}
void GLMain::DrawIdle()
{
		//glutPostRedisplay();
}

void GLMain::initGlut(int argc, char **argv)
{
	char arg1[256], arg2[256];
	// GLUT Window Initialization:
	glutInit (&argc, argv);
	glutInitWindowSize (1920, 1080);
	glutInitWindowPosition(0, 0);
	glutInitDisplayMode ( GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	{
	sprintf(arg1," %s (%s, %s):",VERSION_STRING, __DATE__,__TIME__);
	

	#if 0
		/* getting a warning here about passing arg1 of sprinf incompatable pointer type ?? */
		/* WTF ?!? */
		if (sprintf(arg2, " %i+%i Polygons using ", render.BowlGetpoly_count(), render.VehicleGetpoly_count()))
		{
			strcat (arg1, arg2);
		}
		if (sprintf(arg2, "%i+%i Kb", render.BowlGetMemSize()/1024, render.VehicleGetMemSize()/1024))
		{
			strcat (arg1, arg2);
		}

		/* save most of the name for use later */
		common.setWindowName(arg1);

		if (sprintf(arg2, "%.2f FPS", common.getFrameRate()))
		{
			strcat (arg1, arg2);
		}
	#endif
		glutCreateWindow (arg1);
	}
	//glutSetCursor(GLUT_CURSOR_NONE);
	/* Register the event callback functions since we are using GLUT */
	glutDisplayFunc(DrawGLScene); /* Register the function to do all our OpenGL drawing. */
	glutIdleFunc(DrawIdle); /* Even if there are no events, redraw our gl scene. */
	glutReshapeFunc(ReSizeGLScene); /* Register the function called when our window is resized. */
	glutKeyboardFunc(keyPressed); /* Register the function called when the keyboard is pressed. */
	glutSpecialFunc(specialkeyPressed); /* Register the special key function */
	glutMouseFunc(mouseButtonPress); /* Register the function called when the mouse buttons are pressed */
	glutMotionFunc(mouseMotionPress); /*Register the mouse motion function */

	//    glutReshapeFunc(ChangeSize);


	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
	  /* Problem: glewInit failed, something is seriously wrong. */
	  cerr<<"glewInit failed"<<endl;
	  exit(1);
	}
}



void GLMain::mainloop()
{
		glutMainLoop();

}
//--------main entry------------------
int GLMain::start(int argc, char** argv,void *parm)
{
	GLMain_InitPrm *m_initPrm=(GLMain_InitPrm *)parm;

	/***************************/
	COM_Contrl::getinstance()->registkey(mouseButtonPress);
	//parseArgs(argc, argv);
	initGlut(argc, argv);
		if(m_initPrm->nQueueSize < 4)
		m_initPrm->nQueueSize =4;
	queuenum=m_initPrm->nChannels;
	for(int chId=0; chId<m_initPrm->nChannels; chId++)
		image_queue_create(&m_bufQue[chId], m_initPrm->nQueueSize,
				m_initPrm->channelsSize[chId].w*m_initPrm->channelsSize[chId].h*m_initPrm->channelsSize[chId].c,
				m_initPrm->memType);
	//initGlew();
	//render.initPixle();

	render.SetupRC(720, 576);//1920,1080);//

	return 0;
}



