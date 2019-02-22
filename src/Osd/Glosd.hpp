#ifndef GLOSD_HPP_
#define GLOSD_HPP_
#include <GLShaderManager.h>
#include <GLFrustum.h>
#include <GLBatch.h>
#include <GLFrame.h>
#include <GLMatrixStack.h>
#include <GLGeometryTransform.h>
#include "FreetypeFont.hpp"



#define POINTNUMMAX 1000

enum
{
	GLRED,
	GLGREEN,
	GLBLUE,
	BLACK,
	WIRTE,
	GREY,
	
	


};
typedef struct 
{
	double x;
	double y;

}OSDPoint;

class GLOSD{

public:
	GLOSD();
	~GLOSD();
	void create();
	void drawbegin();
	void drawend();
	void setwindow(int w,int h);
	void drawrect(int x,int y,int w,int h);
	void drawline(int xs,int ys,int xe,int ye);
	void drawcross(int x,int y,int w,int h);
	void windowtoglcenter(float point[],int num);
	void drawstring(char *string);
	void drawstrings(int x,int y,char *string);
	void ChangeSize(int w,int h );
	void drawrectfill(int x,int y,int w,int h);
	bool setcolorlinealpha(double alpha);
	void drawloops(std::vector<OSDPoint> points);
	void drawrectfov(int x,int y,int w,int h);
	GLfloat Linecolor[4];
	void glcenter(float point[],int num);
	bool setcolorline(int color);
	void setlinewidth(double w);
	GLShaderManager	drawshaderManager;
	int windowW;
	int windowH;

	GLfloat Vpoints[POINTNUMMAX][3];

	void drawunicode(int x,int y,wchar_t* text);
	bool setcolorunicode(int color);
	bool createunicode(const char* faceName,int fontSize,int w ,int h);
	bool desteryunicode();
	bool drawunicodebegin();
	bool drawunicodeend();
	void drawloopsscreen(std::vector<OSDPoint> points);
	
	GLfloat Unicodecolor[4];
	FreeTypeFont *freetype;


	GLMatrixStack		modelViewMatrix;
       GLMatrixStack		projectionMatrix;
       GLFrame				cameraFrame;
       GLFrustum			viewFrustum;
	GLGeometryTransform	transformPipeline;		// Geometry Transform Pipeline
	
	void drawunicode(int x,int y,Rgba colour,wchar_t* text);
	void windowtoglcenterunicode(int x,int y,int *cx,int *cy);

};

#endif
