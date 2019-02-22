
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv/cv.hpp>
#include <opencv2/opencv.hpp>

//#include <gl.h>
#include <glew.h>
#include <glut.h>
#include <freeglut_ext.h>
#include <cuda.h>
#include <cuda_gl_interop.h>
#include "cuda_runtime_api.h"
#include "osa.h"
#include "osa_mutex.h"
#include "osa_tsk.h"
#include "osa_image_queue.h"
#include "Displayer.hpp"
#include "cuda_mem.cpp"

#ifdef _WIN32
#pragma comment (lib, "glew32.lib")
#endif

static CRender *gThis = NULL;

GLint Uniform_tex_in = -1;
GLint Uniform_tex_osd = -1;
GLint Uniform_tex_pbo = -1;
GLint Uniform_osd_enable = -1;
GLint Uniform_mattrans = -1;
static GLfloat m_glmat44fTransDefault[16] ={
	1.0f, 0.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 0.0f, 1.0f
};
static GLfloat m_glmat44fTransDefault2[16] ={
	1.1f, 0.0f, 0.0f, 0.0f,
	0.0f, 1.1f, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 0.0f, 1.0f
};
static GLfloat m_glvVertsDefault[8] = {-1.0f, 1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f};
static GLfloat m_glvTexCoordsDefault[8] = {0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f};
double capTime = 0;

CRender::CRender()
:m_mainWinWidth(720),m_mainWinHeight(576),m_renderCount(0),
m_bRun(false),m_bFullScreen(false),m_bOsd(false),
 m_glProgram(0), m_bUpdateVertex(false)
{
	gThis = this;
	memset(&m_initPrm, 0, sizeof(m_initPrm));
	memset(m_bufQue, 0, sizeof(m_bufQue));
	memset(m_totimestamp, 0xff, sizeof(m_totimestamp));
}

CRender::~CRender()
{
	//destroy();
	gThis = NULL;
}

int CRender::create()
{
	int i;
	cudaError_t et;
	memset(m_renders, 0, sizeof(m_renders));
	for(i=0; i<4; i++){
		memcpy(m_renders[i].transform, m_glmat44fTransDefault, sizeof(float)*16);
	}
	for(; i<DS_RENDER_MAX; i++){
		memcpy(m_renders[i].transform, m_glmat44fTransDefault2, sizeof(float)*16);
	}
	memset(&m_videoSize[0], 0, sizeof(DS_Size)*DS_CHAN_MAX);

	for(i=0; i<DS_DC_CNT; i++){
		m_imgOsd[i] = cv::Mat(1080, 1920, CV_8UC4, cv::Scalar(0,0,0,0));
	}

	gl_create();

	OSA_mutexCreate(&m_mutex);

	return 0;
}

int CRender::destroy()
{
	int i;
	cudaError_t et;

	stop();

	gl_destroy();

	uninitRender();

	for(int chId=0; chId<m_initPrm.nChannels; chId++)
		image_queue_delete(&m_bufQue[chId]);

	OSA_mutexDelete(&m_mutex);

	return 0;
}

int CRender::initRender(bool bInitBind)
{
	int i=0;

	if(bInitBind)
		m_renders[i].video_chId    = 0;
	m_renders[i].displayrect.x = 0;
	m_renders[i].displayrect.y = 0;
	m_renders[i].displayrect.w = m_mainWinWidth;
	m_renders[i].displayrect.h = m_mainWinHeight;
	i++;

	if(bInitBind)
		m_renders[i].video_chId    = -1;
	m_renders[i].displayrect.x = m_mainWinWidth*2/3;
	m_renders[i].displayrect.y = m_mainWinHeight*2/3;
	m_renders[i].displayrect.w = m_mainWinWidth/3;
	m_renders[i].displayrect.h = m_mainWinHeight/3;
	i++;

#if 0
	m_renders[i].video_chId    = -1;
	m_renders[i].displayrect.x = m_mainWinWidth*2/3;//m_mainWinWidth - (int)(m_mainWinWidth /  3 * sqrt(2));
	m_renders[i].displayrect.y = m_mainWinHeight*2/3;;//m_mainWinHeight - (int)(m_mainWinHeight /  3 * sqrt(2));
	m_renders[i].displayrect.w = m_mainWinWidth/3;;//(int)(m_mainWinWidth /  3 * sqrt(2));
	m_renders[i].displayrect.h = m_mainWinHeight/3;;//(int)(m_mainWinHeight /  3 * sqrt(2));
	i++;

#endif
	m_renderCount = i;

	return 0;
}

void CRender::uninitRender()
{
	m_renderCount = 0;
}

void CRender::_display(void)
{
	gThis->gl_display();
}

void CRender::_timeFunc(int value)
{
	if(!gThis->m_bRun){
		return ;
	}

	glutTimerFunc(gThis->m_initPrm.timerInterval, _timeFunc, value);

	gThis->_display();
	if(gThis->m_initPrm.timerfunc != NULL)
		gThis->m_initPrm.timerfunc(value);
}

void CRender::disp_fps(){
    static GLint frames = 0;
    static GLint t0 = 0;
    static char  fps_str[20] = {'\0'};
    GLint t = glutGet(GLUT_ELAPSED_TIME);
    sprintf(fps_str, "%6.1f FPS\n", 0.0f);
    if (t - t0 >= 200) {
        GLfloat seconds = (t - t0) / 1000.0;
        GLfloat fps = frames / seconds;
        sprintf(fps_str, "%6.1f FPS\n", fps);
        printf("%6.1f FPS\n", fps);
        t0 = t;
        frames = 0;
    }
    glColor3f(0.0, 0.0, 1.0);
    glRasterPos2f(0.5, 0.5);
    glutBitmapString(GLUT_BITMAP_HELVETICA_18, (unsigned char *)fps_str);
    frames++;
}

void CRender::_reshape(int width, int height)
{
	assert(gThis != NULL);
	glViewport(0, 0, width, height);
	gThis->m_mainWinWidth = width;
	gThis->m_mainWinHeight = height;
	gThis->initRender(false);
	gThis->gl_updateVertex();
	gThis->gl_resize();
}
void CRender::gl_resize()
{
	//glGenBuffers(1, pixBuffObjs);
	//glBindBuffer(GL_PIXEL_PACK_BUFFER, pixBuffObjs[0]);
	//glBufferData(GL_PIXEL_PACK_BUFFER,
	//	m_mainWinWidth*m_mainWinHeight*3,
	//	NULL, GL_DYNAMIC_COPY);
	//glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
}

void CRender::_close(void)
{
	if(gThis->m_initPrm.closefunc != NULL)
		gThis->m_initPrm.closefunc();
}

int CRender::init(DS_InitPrm *pPrm)
{
	if(pPrm != NULL)
		memcpy(&m_initPrm, pPrm, sizeof(DS_InitPrm));

	if(m_initPrm.winWidth > 0)
		m_mainWinWidth = m_initPrm.winWidth;
	if(m_initPrm.winHeight > 0)
		m_mainWinHeight = m_initPrm.winHeight;

	if(m_initPrm.timerInterval<=0)
		m_initPrm.timerInterval = 40;

	if(m_initPrm.disFPS<=0)
		m_initPrm.disFPS = 25;

    //glutInitWindowPosition(m_initPrm.winPosX, m_initPrm.winPosY);
    glutInitWindowSize(m_mainWinWidth, m_mainWinHeight);
    glutCreateWindow("DSS");
	glutDisplayFunc(&_display);
	if(m_initPrm.idlefunc != NULL)
		glutIdleFunc(m_initPrm.idlefunc);
	glutReshapeFunc(_reshape);

	if(m_initPrm.keyboardfunc != NULL)
		glutKeyboardFunc(m_initPrm.keyboardfunc);
	if(m_initPrm.keySpecialfunc != NULL)
		glutSpecialFunc(m_initPrm.keySpecialfunc);

	//mouse event:
	if(m_initPrm.mousefunc != NULL)
		glutMouseFunc(m_initPrm.mousefunc);//GLUT_LEFT_BUTTON GLUT_MIDDLE_BUTTON GLUT_RIGHT_BUTTON; GLUT_DOWN GLUT_UP
	//glutMotionFunc();//button down
	//glutPassiveMotionFunc();//button up
	//glutEntryFunc();//state GLUT_LEFT, GLUT_ENTERED

	if(m_initPrm.visibilityfunc != NULL)
		glutVisibilityFunc(m_initPrm.visibilityfunc);

	//setFullScreen(true);
	if(m_initPrm.bFullScreen){
		glutFullScreen();
		m_bFullScreen = true;
		setFullScreen(m_bFullScreen);
	}
	glutCloseFunc(_close);

	initRender();

	gl_updateVertex();

	gl_init();

	if(m_initPrm.nQueueSize < 2)
		m_initPrm.nQueueSize = 2;
	for(int chId=0; chId<m_initPrm.nChannels; chId++)
		image_queue_create(&m_bufQue[chId], m_initPrm.nQueueSize,
				m_initPrm.channelsSize[chId].w*m_initPrm.channelsSize[chId].h*m_initPrm.channelsSize[chId].c,
				m_initPrm.memType);

	m_interval = (1000000000ul)/(uint64)m_initPrm.disFPS;

	return 0;
}

int CRender::get_videoSize(int chId, DS_Size &size)
{
	if(chId < 0 || chId >= DS_CHAN_MAX)
		return -1;
	size = m_videoSize[chId];

	return 0;
}
int CRender::dynamic_config(DS_CFG type, int iPrm, void* pPrm)
{
	int iRet = 0;
	int chId;
	bool bEnable;
	DS_Rect *rc;
	DS_fRect *frc;
	
	if(type == DS_CFG_ChId){
		if(iPrm >= m_renderCount || iPrm < 0)
			return -1;
		if(pPrm == NULL)
			return -2;
		chId = *(int*)pPrm;

		m_renders[iPrm].video_chId = chId;
		m_totimestamp[chId] = ~0;
	}

	if(type == DS_CFG_EnhEnable){
		if(iPrm >= m_renderCount || iPrm < 0)
			return -1;
		if(pPrm == NULL)
			return -2;
		bEnable = *(bool*)pPrm;
		//m_bEnh[iPrm] = bEnable;
	}

	if(type == DS_CFG_CropEnable){
		if(iPrm >= m_renderCount || iPrm < 0)
			return -1;
		if(pPrm == NULL)
			return -2;
		bEnable = *(bool*)pPrm;
		m_renders[iPrm].bCrop = bEnable;
	}

	if(type == DS_CFG_CropRect){
		if(iPrm >= m_renderCount || iPrm < 0)
			return -1;
		if(pPrm == NULL)
			return -2;
		rc = (DS_Rect*)pPrm;
		m_renders[iPrm].croprect = *rc;
		gl_updateVertex();
	}

	if(type == DS_CFG_VideoTransMat){
		if(iPrm >= DS_CHAN_MAX || iPrm < 0)
			return -1;
		if(pPrm == NULL)
			return -2;
		memcpy(m_glmat44fTrans[iPrm], pPrm, sizeof(float)*16);
	}

	if(type == DS_CFG_ViewTransMat){
		if(iPrm >= m_renderCount || iPrm < 0)
			return -1;
		if(pPrm == NULL)
			return -2;
		memcpy(m_renders[iPrm].transform , pPrm, sizeof(float)*16);
		gl_updateVertex();
	}

	if(type == DS_CFG_BindRect){
		if(iPrm >= m_renderCount || iPrm < 0)
			return -1;
		if(pPrm == NULL)
			return -2;
		frc = (DS_fRect*)pPrm;
		m_renders[iPrm].bindrect = *frc;
		//initRender(false);
		gl_updateVertex();
	}

	return iRet;
}

GLuint CRender::async_display(int chId, int width, int height, int channels)
{
	assert(chId>=0 && chId<DS_CHAN_MAX);

	if(m_videoSize[chId].w  == width  && m_videoSize[chId].h == height && m_videoSize[chId].c == channels )
		return buffId_input[chId];

	OSA_printf("%s: w = %d h = %d (%dx%d) cur %d\n", __FUNCTION__, width, height, m_videoSize[chId].w, m_videoSize[chId].h, buffId_input[chId]);

	if(m_videoSize[chId].w != 0){
		glDeleteBuffers(1, &buffId_input[chId]);
		glGenBuffers(1, &buffId_input[chId]);
	}

	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, buffId_input[chId]);
	glBufferData(GL_PIXEL_UNPACK_BUFFER, width*height*channels, NULL, GL_DYNAMIC_COPY);//GL_STATIC_DRAW);//GL_DYNAMIC_DRAW);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

	m_videoSize[chId].w = width;
	m_videoSize[chId].h = height;
	m_videoSize[chId].c = channels;

	OSA_printf("%s: w = %d h = %d (%dx%d) out %d\n", __FUNCTION__, width, height, m_videoSize[chId].w, m_videoSize[chId].h, buffId_input[chId]);
	return buffId_input[chId];
}

void CRender::run()
{
	m_bRun = true;
	//glutSetOption();
	//glutMainLoopEvent();
	//glutMainLoop();
	//glutTimerFunc(0, _timeFunc, m_initPrm.timerfunc_value);
	glutPostRedisplay();
}

void CRender::stop()
{
	m_bRun = false;
}

int CRender::setFullScreen(bool bFull)
{
	if(bFull)
		glutFullScreen();
	else{

	}
	m_bFullScreen = bFull;

	return 0;
}

/***********************************************************************/

static char glName[] = {"DS_RENDER"}; 

int CRender::gl_create()
{
	char *argv[1] = {glName};
	int argc = 1;
   // GLUT init
    glutInit(&argc, argv);  
	//Double, Use glutSwapBuffers() to show
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_MULTISAMPLE);
	//Single, Use glFlush() to show
	//glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB );

	// Blue background
	glClearColor(0.0f, 0.0f, 0.01f, 0.0f );

	return 0;
}
void CRender::gl_destroy()
{
	gl_uninit();
	glutLeaveMainLoop();
	for(int chId=0; chId<DS_CHAN_MAX; chId++)
		cudaResource_UnregisterBuffer(chId);
}

#define TEXTURE_ROTATE (0)
#define ATTRIB_VERTEX 3
#define ATTRIB_TEXTURE 4
void CRender::gl_init()
{
	int i;

	GLenum err = glewInit();
	if (GLEW_OK != err) {
		fprintf(stderr, "GLEW Error: %s\n", glewGetErrorString(err));
		return;
	}

	m_glProgram = gltLoadShaderPairWithAttributes("Shader.vsh", "Shader.fsh", 2, 
		ATTRIB_VERTEX, "vVertex", ATTRIB_TEXTURE, "vTexCoords");
	//m_glProgram = gltLoadShaderPairWithAttributes("Shader.vsh", "Shader.fsh", 0);

	glGenBuffers(DS_CHAN_MAX, buffId_input);
	glGenTextures(DS_CHAN_MAX, textureId_input);
	for(i=0; i<DS_CHAN_MAX; i++)
	{
		glBindTexture(GL_TEXTURE_2D, textureId_input[i]);
		assert(glIsTexture(textureId_input[i]));
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);//GL_NEAREST);//GL_NEAREST_MIPMAP_NEAREST);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);//GL_NEAREST);//GL_NEAREST_MIPMAP_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);//GL_CLAMP);//GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);//GL_CLAMP);//GL_CLAMP_TO_EDGE);
		glTexImage2D(GL_TEXTURE_2D, 0, 3, 1920, 1080, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, NULL);
	}

	glGenTextures(1, &textureId_osd); 
	glBindTexture(GL_TEXTURE_2D, textureId_osd);
	assert(glIsTexture(textureId_osd));

	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	//glGenTextures(1, &textureId_pbo);
	//glBindTexture(GL_TEXTURE_2D, textureId_pbo);
	//assert(glIsTexture(textureId_pbo));
	//glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	for(i=0; i<DS_CHAN_MAX; i++){
		memcpy(m_glmat44fTrans[i], m_glmat44fTransDefault, sizeof(m_glmat44fTransDefault));
	}

	//glGenBuffers(1, pixBuffObjs);
	//glBindBuffer(GL_PIXEL_PACK_BUFFER, pixBuffObjs[0]);
	//glBufferData(GL_PIXEL_PACK_BUFFER,
	//	m_mainWinWidth*m_mainWinHeight*4,
	//	NULL, GL_DYNAMIC_COPY);
	//glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);

	//glEnable(GL_LINE_SMOOTH);
	//glHint(GL_LINE_SMOOTH_HINT, GL_DONT_CARE);
	glClear(GL_COLOR_BUFFER_BIT);
}

void CRender::gl_uninit()
{
	int i;
	if(m_glProgram != 0)
		glDeleteProgram(m_glProgram);
	m_glProgram = 0;

	glDeleteTextures(DS_CHAN_MAX, textureId_input);
	glDeleteBuffers(DS_CHAN_MAX, buffId_input);
	glDeleteTextures(1, &textureId_osd); 

}

int CRender::gl_updateVertex(void)
{
	int iRet = 0;
	int winId, chId, i;
	DS_Rect rc;
	//GLfloat ftmp;

	for(winId=0; winId<m_renderCount; winId++)
	{
		m_glvVerts[winId][0] = -1.0f; m_glvVerts[winId][1] 	= 1.0f;
		m_glvVerts[winId][2] = 1.0f;  m_glvVerts[winId][3] 	= 1.0f;
		m_glvVerts[winId][4] = -1.0f; m_glvVerts[winId][5] 	= -1.0f;
		m_glvVerts[winId][6] = 1.0f;  m_glvVerts[winId][7] 	= -1.0f;

		for(i=0; i<4; i++){
			float x = m_glvVerts[winId][i*2+0];
			float y = m_glvVerts[winId][i*2+1];
			m_glvVerts[winId][i*2+0] = m_renders[winId].transform[0][0] * x + m_renders[winId].transform[0][1] * y + m_renders[winId].transform[0][3];
			m_glvVerts[winId][i*2+1] = m_renders[winId].transform[1][0] * x + m_renders[winId].transform[1][1] * y + m_renders[winId].transform[1][3];
		}

		if(m_renders[winId].bBind){
			GLfloat subw, subh;
			subw = m_glvVerts[winId][2] - m_glvVerts[winId][0];
			subh = m_glvVerts[winId][5] - m_glvVerts[winId][1];
			m_glvVerts[winId][0] += m_renders[winId].bindrect.x*subw;
			m_glvVerts[winId][1] += m_renders[winId].bindrect.y*subh;
			m_glvVerts[winId][2] = m_glvVerts[winId][0] + m_renders[winId].bindrect.w*subw;
			m_glvVerts[winId][3] = m_glvVerts[winId][1];
			m_glvVerts[winId][4] = m_glvVerts[winId][0];
			m_glvVerts[winId][5] = m_glvVerts[winId][1] + m_renders[winId].bindrect.h*subh;
			m_glvVerts[winId][6] = m_glvVerts[winId][0] + m_renders[winId].bindrect.w*subw;
			m_glvVerts[winId][7] = m_glvVerts[winId][1] + m_renders[winId].bindrect.h*subh;
		}

		m_glvTexCoords[winId][0] = 0.0; m_glvTexCoords[winId][1] = 0.0;
		m_glvTexCoords[winId][2] = 1.0; m_glvTexCoords[winId][3] = 0.0;
		m_glvTexCoords[winId][4] = 0.0; m_glvTexCoords[winId][5] = 1.0;
		m_glvTexCoords[winId][6] = 1.0; m_glvTexCoords[winId][7] = 1.0;
	}

	for(winId=0; winId<m_renderCount; winId++)
	{
		chId = m_renders[winId].video_chId;
		if(chId < 0 || chId >= DS_CHAN_MAX)
			continue;
		rc = m_renders[winId].croprect;
		if(m_videoSize[chId].w<=0 || m_videoSize[chId].h<=0){
			iRet ++;
			continue;
		}
		if(rc.w == 0 && rc.h == 0){
			continue;
		}
		m_glvTexCoords[winId][0] = (GLfloat)rc.x/m_videoSize[chId].w; 
		m_glvTexCoords[winId][1] = (GLfloat)rc.y/m_videoSize[chId].h;

		m_glvTexCoords[winId][2] = (GLfloat)(rc.x+rc.w)/m_videoSize[chId].w;
		m_glvTexCoords[winId][3] = (GLfloat)rc.y/m_videoSize[chId].h;

		m_glvTexCoords[winId][4] = (GLfloat)rc.x/m_videoSize[chId].w;
		m_glvTexCoords[winId][5] = (GLfloat)(rc.y+rc.h)/m_videoSize[chId].h;

		m_glvTexCoords[winId][6] = (GLfloat)(rc.x+rc.w)/m_videoSize[chId].w;
		m_glvTexCoords[winId][7] = (GLfloat)(rc.y+rc.h)/m_videoSize[chId].h;
	}

	return iRet;
}

/*unsigned char* CRender::bpoMap(int chId, int width, int height, int channels)
{
	GLuint pbo = async_display(chId, width, height, channels);
	unsigned int byteCount = width*height*channels;
	unsigned char *dev_pbo = NULL;
	size_t tmpSize;
	OSA_assert(pbo == buffId_input[chId]);
	cudaResource_RegisterBuffer(chId, pbo, byteCount);
	cudaResource_mapBuffer(chId, (void **)&dev_pbo, &tmpSize);
	assert(tmpSize == byteCount);

	cudaResource_unmapBuffer(chId);
	cudaResource_UnregisterBuffer(chId);

	return dev_pbo;
}*/

void CRender::gl_display(void)
{
	int winId, chId;
	unsigned int mask = 0;
	int iret;

	//static int64 tstart = 0;
	//tstart = getTickCount();
	//OSA_printf("%d %s. 0", OSA_getCurTimeInMsec(), __func__);

	for(chId = 0; chId<DS_CHAN_MAX; chId++){
		if(m_totimestamp[chId]<~0)
			m_totimestamp[chId] += m_interval;//ns
		if(!m_bufQue[chId].bCreate){
			for(int i=0; i<m_bufQue[chId].numBuf; i++){
				cuMap(&m_bufQue[chId].bufInfo[i]);
			}
			m_bufQue[chId].bCreate = true;
		}
	}

	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(m_glProgram);
	Uniform_tex_in = glGetUniformLocation(m_glProgram, "tex_in");
	Uniform_tex_osd = glGetUniformLocation(m_glProgram, "tex_osd");
	Uniform_tex_pbo = glGetUniformLocation(m_glProgram, "tex_pbo");
	Uniform_osd_enable = glGetUniformLocation(m_glProgram, "bOsd");
	Uniform_mattrans = glGetUniformLocation(m_glProgram, "mTrans");

	for(winId=0; winId<m_renderCount; winId++)
	{
		chId = m_renders[winId].video_chId;
		if(chId < 0 || chId >= DS_CHAN_MAX)
			continue;

		glUniformMatrix4fv(Uniform_mattrans, 1, GL_FALSE, m_glmat44fTrans[chId]);
		//glUniform1i(Uniform_osd_enable, m_bOsd);

		glUniform1i(Uniform_tex_in, 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureId_input[chId]);

		if(!((mask >> chId)&1))
		{
			bool bDevMem = false;
			OSA_BufInfo* info = NULL;
			Mat img;
			OSA_mutexLock(&m_mutex);

			int count = OSA_bufGetFullCount(&m_bufQue[chId]);

			for(int i=0; i<count; i++)
			{
				int bufferId;
				iret = OSA_quePeek(&m_bufQue[chId].fullQue, &bufferId);
				OSA_assert(iret == OSA_SOK);
				info = OSA_bufGetBufInfo(&m_bufQue[chId], bufferId);
				OSA_assert(info != NULL);
				if(info->timestamp < m_totimestamp[chId]-1000000 && i<count-1){
					OSA_bufSwitchEmpty(&m_bufQue[chId], &bufferId);
				}else{
					bDevMem = (info->memtype == memtype_cudev || info->memtype == memtype_cumap);
					void *data = (bDevMem) ? info->physAddr : info->virtAddr;
					if(info->channels == 1){
						img = cv::Mat(info->height, info->width, CV_8UC1, data);
					}else{
						img = cv::Mat(info->height, info->width, CV_8UC3, data);
					}
					OSA_bufSwitchEmpty(&m_bufQue[chId], &bufferId);
					if(m_totimestamp[chId]==~0)
						m_totimestamp[chId] = info->timestamp;
					break;
				}
			}
			if(info != NULL)
			{
				uint64 deferent;
				if(m_totimestamp[chId] > info->timestamp)
					deferent = m_totimestamp[chId]-info->timestamp;
				else
					deferent = info->timestamp - m_totimestamp[chId];
				if(deferent>m_interval){
					OSA_printf("ch%d tm %lu-%lu=%lu (>%lu)ns  recover", chId,
							info->timestamp, m_totimestamp[chId], deferent, m_interval);
					m_totimestamp[chId] = info->timestamp;
				}
				//OSA_printf("ch%d bDevMem = %d img(%d x %d) %ld(%ld)ns", chId,
				//		bDevMem, img.cols, img.rows,
				//		info->timestamp, m_totimestamp[chId]);
			}
			count = OSA_bufGetFullCount(&m_bufQue[chId]);
			if(count>1){
				m_totimestamp[chId] = ~0;
				OSA_printf("%s: ch%d full queue count > 1, reset!!!\n",
									__func__, chId);
			}

			if(img.cols > 0 && img.rows > 0)
			{
				static int bCreate[DS_CHAN_MAX] = {0,};
				GLuint pbo = 0;
				OSA_assert(info != NULL);
				if(bDevMem)
				{
					unsigned int byteCount = img.cols*img.rows*img.channels();
					unsigned char *dev_pbo = NULL;
					size_t tmpSize;
					pbo = async_display(chId, img.cols, img.rows, img.channels());
					OSA_assert(pbo == buffId_input[chId]);
					cudaResource_RegisterBuffer(chId, pbo, byteCount);
					cudaResource_mapBuffer(chId, (void **)&dev_pbo, &tmpSize);
					assert(tmpSize == byteCount);
					cudaMemcpy(dev_pbo, img.data, byteCount, cudaMemcpyDeviceToDevice);
					img.data = NULL;
					cudaResource_unmapBuffer(chId);
					cudaResource_UnregisterBuffer(chId);
				}

				if(info->memtype == memtype_glpbo){
					cuUnmap(info);
					pbo = info->pbo;
				}
				glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbo);
				if(!bCreate[chId])
				{
					if(img.channels() == 1)
						glTexImage2D(GL_TEXTURE_2D, 0, img.channels(), img.cols, img.rows, 0, GL_RED, GL_UNSIGNED_BYTE, img.data);
					else if(img.channels() == 4)
						glTexImage2D(GL_TEXTURE_2D, 0, img.channels(), img.cols, img.rows, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, img.data);
					else
						glTexImage2D(GL_TEXTURE_2D, 0, img.channels(), img.cols, img.rows, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, img.data);
					m_videoSize[chId].w = img.cols;
					m_videoSize[chId].h = img.rows;
					m_videoSize[chId].c = img.channels();
					bCreate[chId] = true;
				}
				else
				{
					if(img.channels() == 1)
						glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, img.cols, img.rows, GL_RED, GL_UNSIGNED_BYTE, img.data);
					else if(img.channels() == 4)
						glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, img.cols, img.rows, GL_BGRA_EXT, GL_UNSIGNED_BYTE, img.data);
					else
						glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, img.cols, img.rows, GL_BGR_EXT, GL_UNSIGNED_BYTE, img.data);
				}
				glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
				if(info->memtype == memtype_glpbo){
					cuMap(info);
				}
			}
			OSA_mutexUnlock(&m_mutex);

			mask |= (1<<chId);
		}

		glVertexAttribPointer(ATTRIB_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, m_glvVerts[winId]);
		glVertexAttribPointer(ATTRIB_TEXTURE, 2, GL_FLOAT, GL_FALSE, 0, m_glvTexCoords[winId]);
		glEnableVertexAttribArray(ATTRIB_VERTEX);
		glEnableVertexAttribArray(ATTRIB_TEXTURE);

		glEnable(GL_MULTISAMPLE);
		//glEnable(GL_BLEND);
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glViewport(m_renders[winId].displayrect.x,
				m_renders[winId].displayrect.y,
				m_renders[winId].displayrect.w, m_renders[winId].displayrect.h);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glDisable(GL_MULTISAMPLE);
		//glDisable(GL_BLEND);
	}

	if(m_bOsd)
	{
		glUniformMatrix4fv(Uniform_mattrans, 1, GL_FALSE, m_glmat44fTransDefault);

		glUniform1i(Uniform_tex_in, 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureId_osd);

		glVertexAttribPointer(ATTRIB_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, m_glvVertsDefault);
		glVertexAttribPointer(ATTRIB_TEXTURE, 2, GL_FLOAT, GL_FALSE, 0, m_glvTexCoordsDefault);
		glEnableVertexAttribArray(ATTRIB_VERTEX);
		glEnableVertexAttribArray(ATTRIB_TEXTURE);

		glEnable(GL_MULTISAMPLE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		for(int i=0; i<DS_DC_CNT; i++)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_imgOsd[i].cols, m_imgOsd[i].rows,
				0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, m_imgOsd[i].data);
			glViewport(0, 0, m_mainWinWidth, m_mainWinHeight);
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		}

		glDisable(GL_MULTISAMPLE);
		glDisable(GL_BLEND);
	}

	//glValidateProgram(m_glProgram);
	//OSA_printf("%d %s. 1", OSA_getCurTimeInMsec(), __func__);
	glutSwapBuffers();
	glutPostRedisplay();
	//OSA_printf("%d %s. 2", OSA_getCurTimeInMsec(), __func__);
	
	//float telapse = ( (getTickCount() - tstart)/getTickFrequency());
	//if(telapse > 0.5)
	//	OSA_printf("%s: time = %f sec \n", __func__,  telapse);

}


//////////////////////////////////////////////////////////////////////////
// Load the shader from the source text
void CRender::gltLoadShaderSrc(const char *szShaderSrc, GLuint shader)
{
	GLchar *fsStringPtr[1];

	fsStringPtr[0] = (GLchar *)szShaderSrc;
	glShaderSource(shader, 1, (const GLchar **)fsStringPtr, NULL);
}

#define MAX_SHADER_LENGTH   8192
static GLubyte shaderText[MAX_SHADER_LENGTH];
////////////////////////////////////////////////////////////////
// Load the shader from the specified file. Returns false if the
// shader could not be loaded
bool CRender::gltLoadShaderFile(const char *szFile, GLuint shader)
{
	GLint shaderLength = 0;
	FILE *fp;

	// Open the shader file
	fp = fopen(szFile, "r");
	if(fp != NULL)
	{
		// See how long the file is
		while (fgetc(fp) != EOF)
			shaderLength++;

		// Allocate a block of memory to send in the shader
		assert(shaderLength < MAX_SHADER_LENGTH);   // make me bigger!
		if(shaderLength > MAX_SHADER_LENGTH)
		{
			fclose(fp);
			return false;
		}

		// Go back to beginning of file
		rewind(fp);

		// Read the whole file in
		if (shaderText != NULL){
			size_t ret = fread(shaderText, 1, (size_t)shaderLength, fp);
			OSA_assert(ret == shaderLength);
		}

		// Make sure it is null terminated and close the file
		shaderText[shaderLength] = '\0';
		fclose(fp);
	}
	else
		return false;    

	// Load the string
	gltLoadShaderSrc((const char *)shaderText, shader);

	return true;
}   

/////////////////////////////////////////////////////////////////
// Load a pair of shaders, compile, and link together. Specify the complete
// source text for each shader. After the shader names, specify the number
// of attributes, followed by the index and attribute name of each attribute
GLuint CRender::gltLoadShaderPairWithAttributes(const char *szVertexProg, const char *szFragmentProg, ...)
{
	// Temporary Shader objects
	GLuint hVertexShader;
	GLuint hFragmentShader; 
	GLuint hReturn = 0;   
	GLint testVal;

	// Create shader objects
	hVertexShader = glCreateShader(GL_VERTEX_SHADER);
	hFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	// Load them. If fail clean up and return null
	// Vertex Program
	if(gltLoadShaderFile(szVertexProg, hVertexShader) == false)
	{
		glDeleteShader(hVertexShader);
		glDeleteShader(hFragmentShader);
		fprintf(stderr, "The shader at %s could ot be found.\n", szVertexProg);
		return (GLuint)NULL;
	}

	// Fragment Program
	if(gltLoadShaderFile(szFragmentProg, hFragmentShader) == false)
	{
		glDeleteShader(hVertexShader);
		glDeleteShader(hFragmentShader);
		fprintf(stderr,"The shader at %s  could not be found.\n", szFragmentProg);
		return (GLuint)NULL;
	}

	// Compile them both
	glCompileShader(hVertexShader);
	glCompileShader(hFragmentShader);

	// Check for errors in vertex shader
	glGetShaderiv(hVertexShader, GL_COMPILE_STATUS, &testVal);
	if(testVal == GL_FALSE)
	{
		char infoLog[1024];
		glGetShaderInfoLog(hVertexShader, 1024, NULL, infoLog);
		fprintf(stderr, "The shader at %s failed to compile with the following error:\n%s\n", szVertexProg, infoLog);
		glDeleteShader(hVertexShader);
		glDeleteShader(hFragmentShader);
		return (GLuint)NULL;
	}

	// Check for errors in fragment shader
	glGetShaderiv(hFragmentShader, GL_COMPILE_STATUS, &testVal);
	if(testVal == GL_FALSE)
	{
		char infoLog[1024];
		glGetShaderInfoLog(hFragmentShader, 1024, NULL, infoLog);
		fprintf(stderr, "The shader at %s failed to compile with the following error:\n%s\n", szFragmentProg, infoLog);
		glDeleteShader(hVertexShader);
		glDeleteShader(hFragmentShader);
		return (GLuint)NULL;
	}

	// Create the final program object, and attach the shaders
	hReturn = glCreateProgram();
	glAttachShader(hReturn, hVertexShader);
	glAttachShader(hReturn, hFragmentShader);


	// Now, we need to bind the attribute names to their specific locations
	// List of attributes
	va_list attributeList;
	va_start(attributeList, szFragmentProg);

	// Iterate over this argument list
	char *szNextArg;
	int iArgCount = va_arg(attributeList, int);	// Number of attributes
	for(int i = 0; i < iArgCount; i++)
	{
		int index = va_arg(attributeList, int);
		szNextArg = va_arg(attributeList, char*);
		glBindAttribLocation(hReturn, index, szNextArg);
	}
	va_end(attributeList);

	// Attempt to link    
	glLinkProgram(hReturn);

	// These are no longer needed
	glDeleteShader(hVertexShader);
	glDeleteShader(hFragmentShader);  

	// Make sure link worked too
	glGetProgramiv(hReturn, GL_LINK_STATUS, &testVal);
	if(testVal == GL_FALSE)
	{
		char infoLog[1024];
		glGetProgramInfoLog(hReturn, 1024, NULL, infoLog);
		fprintf(stderr,"The programs %s and %s failed to link with the following errors:\n%s\n",
			szVertexProg, szFragmentProg, infoLog);
		glDeleteProgram(hReturn);
		return (GLuint)NULL;
	}

	// All done, return our ready to use shader program
	return hReturn;  
}   
