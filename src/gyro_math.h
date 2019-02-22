/*******************************************************************************
* Copyright (C), 2011-2012, ChamRun Tech. Co., Ltd.
* WorkContext:
* FileName:    gyro_math.h
* Author:      xavier       Version :          Date:2016年01月20日
* Description: //  
* Version:     //  
* History:     //  
* <author> 		<time> 				<version > 		<description>
*  xavier		2015年11月25日          V1.00    	build this module
********************************************************************************/

#ifndef _GYRO_MATH_H_
#define _GYRO_MATH_H_
#include "cv.h"
#include "cxcore.h"
#include "gyro.h"

void quat_to_rotation_matrix(double q[], double rotation[]);
void slerp(double q1[], double q2[], double u, double q_alph[]);
void matrixMultiply(double* A, double* B, int m, int p, int n, double* C);

void getRotMatrix(double alpha, double beta, double gamma, CvMat *rotMatrix);
void getRotAngle(double refRot[9],double curRot[9], double rotAngle[3]);

void calMapPoint(double K[],double invK[],  double _RotCGMat[], double _refRot[], double _curRot[], double dbx, double dby, double retPoint[3]);
void getHornMatrix(double K[9], double invK[9], double RotCGMat[9], double refRot[9], double curRot[9], double Horn[9]);

#endif

/********************************************************************************/