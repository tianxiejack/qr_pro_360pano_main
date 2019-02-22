/*************************************************************************
  Copyright (C), 2015-2016, ChamRun Tech. Co., Ltd.
  File name:    osd_graphic.h
  Author:       aloysa       Version:  1.0.0      Date: 2017-8-4
  Description:
  Version:      1.0.0
  Function List:
  History:
      <author>  <time>   <version >   <desc>
**************************************************************************/

#ifndef _MULTICH_GRAPHIC_H
#define _MULTICH_GRAPHIC_H

#include <opencv2/opencv.hpp>

typedef enum
{
	grpx_ObjId_Line = 0x00,
	grpx_ObjId_Rect,
	grpx_ObjId_Rect_gap,
	grpx_ObjId_Compass,
	grpx_ObjId_Text,

	// for project
	// for project end

	grpx_ObjId_Max,
	
}GRPX_OBJ_ID;

typedef struct _text_param_fb
{
	UInt32 enableWin;
	UInt32 objType;        //GRPX_OBJ_ID
	UInt32 frcolor;
	UInt32 bgcolor;

	Int16 text_x[3];
	Int16 text_y[3];
	Int16 textLen[3];

	Int16 text_valid;  // bit0-> text0; bit1-> text1; bit2-> text2
	UInt8 textAddr[512];
} Text_Param_fb;

typedef struct  _line_param_fb{
	UInt32 enableWin;
	UInt32 objType;        //GRPX_OBJ_ID
	UInt32 frcolor;		// eOSDColor
	UInt32 bgcolor;

	Int16 x;		        //center position
	Int16 y;		        //center position
	Int16 res0;

	Int16 width;            //graphic width
	Int16 height;           //graphic height
	Int16 res1;

	Int16 linePixels;		//line width
	Int16 lineGapWidth;     //must be <= width
	Int16 lineGapHeight;    //must be <= height
}Line_Param_fb;

typedef struct  _rect_param_fb{
	UInt32 enableWin;
	UInt32 objType;        	//GRPX_OBJ_ID
	UInt32 frcolor;
	UInt32 bgcolor;

	Int16 x;		        //center position
	Int16 y;		        //center position
	Int16 res0;

	Int16 width;            //graphic width
	Int16 height;           //graphic height
	Int16 res1;

	Int16 linePixels;		//line width
	Int16 lineGapWidth;     //must be <= width
	Int16 lineGapHeight;    //must be <= height
}Rect_Param_fb;

#define BIT_SET(val,bit) (val|(1<<bit))
#define BIT_CLR(val,bit) (val|(~(1<<bit)))


#define OSD_MODE_CV
#define OSD_MODE_RGB

#ifdef OSD_MODE_CV
CvScalar GetcvColour(unsigned int colorId);
#endif

void osd_graph_init(void *pPrm);
void osd_graph_update_sem_post(void);
void osd_graph_uninit(void);
void osd_graph_draw(int chId, cv::Mat &dc);

#endif
/*******************************END***************************************/
