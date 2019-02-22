/**************************************************************************
* Copyright (C), 2011-2016, ChamRun Tech. Co., Ltd.
* WorkContext:
* FileName:    ialg_def.h
* Author:      xavier       Version :          Date:2016/01/29
* Description: //  
* Version:     //  
* History:     //  
* <author> 		<time> 				<version > 		<description>
* xavier	29:1:2016  14:06       V1.00    	build this module
**************************************************************************/
#ifndef _IALG_DEF_H_
#define _IALG_DEF_H_

typedef struct POINT_2D_16S{
	short x;
	short y;
}POINT_2D_16S,Point_16S;

typedef struct _point_2D_64f_{
	double x;
	double y;
}POINT_2D_64F,Point_2D64F;

typedef struct _point_64f_
{
	double x;
	double y;
	double z;
}POINT_3D_64F,Point_3D64F;


typedef struct _cyl_mapMatrix_
{
	float fx;
	float fovAngle;
	int	  cylWindth;
	Point_16S * matrix;
}CYLMAT,*CYLMAT_HNDL;

#endif
/*************************************************************************/