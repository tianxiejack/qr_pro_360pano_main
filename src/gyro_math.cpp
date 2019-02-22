/*******************************************************************************
* Copyright (C), 2011-2012, ChamRun Tech. Co., Ltd.
* WorkContext:
* FileName:    gyro_math.c
* Author:      xavier       Version :          Date:2016��01��20��
* Description: //  
* Version:     //  
* History:     //  
* <author> 		<time> 				<version > 		<description>
*  xavier		2015��11��25��          V1.00    	build this module
********************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>

#include "ialg_def.h"
#include "gyro.h"
#include "gyro_math.h"

void quat_to_rotation_matrix(double q[], double rotation[])
{
	double qq[4];
	int i;
	for (i=0; i<4; i++)
   		qq[i] = q[i]*q[i];
	rotation[0] = qq[0] + qq[1] - qq[2] - qq[3];
	rotation[1] = 2*q[1]*q[2] - 2*q[0]*q[3];
	rotation[2] = 2*q[1]*q[3] + 2*q[0]*q[2];
	rotation[3] = 2*q[1]*q[2] + 2*q[0]*q[3];
	rotation[4] = qq[0] - qq[1] + qq[2] - qq[3];
	rotation[5] = 2*q[2]*q[3] - 2*q[0]*q[1];
	rotation[6] = 2*q[1]*q[3] - 2*q[0]*q[2];
	rotation[7] = 2*q[2]*q[3] + 2*q[0]*q[1];
	rotation[8] = qq[0] - qq[1] - qq[2] + qq[3];
}

void slerp(double q1[], double q2[], double u, double q_alph[])
{
	double q1q2=0.0, costheta, theta, f1, f2, q[4], sumq=0.0;
	int i;
	for (i=0; i<4; i++)
		q1q2 +=q1[i]*q2[i];
	costheta = sqrt(q1q2);
	theta = acos(costheta);
	f1 = sin((1.0-u)*theta)/sin(theta);
	f2 = sin(u*theta)/sin(theta);
	if(costheta < 0)
		f1 = -f1;   
	for(i=0; i<4; i++){
		q[i] = f1*q1[i] + f2*q2[i];
		sumq += q[i]*q[i];
	}
	sumq = sqrt(sumq);
	for(i=0; i<4; i++){
		q_alph[i] = q[i]/sumq;
	}
}

void matrixMultiply(double* A, double* B, int m, int p, int n, double* C)
{
	// A = input matrix (m x p)
	// B = input matrix (p x n)
	// m = number of rows in A
	// p = number of columns in A = number of rows in B
	// n = number of columns in B
	// C = output matrix = A*B (m x n)
	int i, j, k;
	for (i=0;i<m;i++)
	{
		for(j=0;j<n;j++)
		{
			C[n*i+j]=0;
			for (k=0;k<p;k++)
			{
				C[n*i+j]= C[n*i+j]+A[p*i+k]*B[n*k+j];
			}
		}
	}
}

void getRotMatrix(double alpha, double beta, double gamma, CvMat *rotMatrix)
{
	double angleX[9] = {1, 0, 0, 0, cos(alpha), sin(alpha), 0, -sin(alpha), cos(alpha)};
	double angleY[9] = {cos(beta), 0, -sin(beta), 0, 1, 0, sin(beta), 0, cos(beta)};
	double angleZ[9] = {cos(gamma), sin(gamma), 0, -sin(gamma), cos(gamma), 0, 0, 0, 1};

	CvMat rotX = cvMat(3, 3, CV_64FC1, angleX);
	CvMat rotY = cvMat(3, 3, CV_64FC1, angleY);
	CvMat rotZ = cvMat(3, 3, CV_64FC1, angleZ);

	double angleYZ[9];
	CvMat rotYZ = cvMat(3, 3, CV_64FC1, angleYZ);

	cvGEMM(&rotZ, &rotY, 1, 0, 0, &rotYZ, 0);

	if (rotMatrix != NULL)
		cvGEMM(&rotYZ, &rotX, 1, 0, 0, rotMatrix, 0);
}

void getRotAngle(double refRot[9],double curRot[9], double rotAngle[3])
{
	double r1Tr2[9];

	CvMat R1Mat		= cvMat(3, 3, CV_64FC1, refRot);
	CvMat R2Mat		= cvMat(3, 3, CV_64FC1, curRot);
	CvMat R1TR2Mat	= cvMat(3, 3, CV_64FC1, r1Tr2);

	CvMat AngleRot	= cvMat(3, 1, CV_64FC1, rotAngle);

	cvGEMM(&R1Mat, &R2Mat, 1, 0, 0, &R1TR2Mat, CV_GEMM_B_T);

	//cvRotMat_2_Angle(&R1TR2Mat, &AngleRot);
}

void calMapPoint( double K[], double invK[], double _RotCGMat[], double _refRot[], double _curRot[], double dbx, double dby, double retPoint[3])
{
	POINT_3D_64F point3D, resltpoint;
	double _r1Tr2[9], _RcgRg[9], _Rcam[9], _KRcam[9], Horn[9];

	CvMat R1Mat    = cvMat(3, 3, CV_64FC1, _refRot);
	CvMat R2Mat    = cvMat(3, 3, CV_64FC1, _curRot);
	CvMat R1TR2Mat = cvMat(3, 3, CV_64FC1, _r1Tr2);
	CvMat RcgRgMat = cvMat(3, 3, CV_64FC1, _RcgRg);
	CvMat RcamMat  = cvMat(3, 3, CV_64FC1, _Rcam);

	CvMat CGMatrix = cvMat( 3, 3, CV_64FC1, _RotCGMat);

	cvGEMM( &R1Mat, &R2Mat, 1, 0, 0, &R1TR2Mat, CV_GEMM_B_T);
	/***************************************************************************/
	// 	cvSVD( &R1TR2Mat, &_W, &_U, &_V, CV_SVD_MODIFY_A);
	// 	cvGEMM( &_U, &_V, 1, 0, 0, &R1TR2Mat, CV_GEMM_B_T );
	/***************************************************************************/
	cvGEMM(&CGMatrix, &R1TR2Mat, 1, 0, 0, &RcgRgMat,0);
	cvGEMM(&RcgRgMat, &CGMatrix, 1, 0, 0, &RcamMat, CV_GEMM_B_T);
	matrixMultiply(K, _Rcam, 3, 3, 3, _KRcam);
	matrixMultiply(_KRcam, invK, 3, 3, 3, Horn);

	point3D.x = dbx, point3D.y = dby, point3D.z = 1.;

	resltpoint.x = Horn[0]*point3D.x + Horn[1]*point3D.y + Horn[2]*point3D.z;
	resltpoint.y = Horn[3]*point3D.x + Horn[4]*point3D.y + Horn[5]*point3D.z;
	resltpoint.z = Horn[6]*point3D.x + Horn[7]*point3D.y + Horn[8]*point3D.z;
	if (fabs(resltpoint.z) < 1e-6)
	{
		resltpoint.x = resltpoint.y = 0;
	}
	else
	{
		resltpoint.x /= resltpoint.z;
		resltpoint.y /= resltpoint.z;
		resltpoint.z /= resltpoint.z;
	}

	retPoint[0] = resltpoint.x;
	retPoint[1] = resltpoint.y;
	retPoint[2] = resltpoint.z;
}

void getHornMatrix(double K[9], double invK[9], double RotCGMat[9], double refRot[9], double curRot[9], double Horn[9])
{
	CvMat R1Mat;
	CvMat R2Mat;
	CvMat R1TR2Mat;
	CvMat RcgRgMat;
	CvMat RcamMat;

	CvMat CG_matrix;

	double r1Tr2[9];	
	double RcgRg[9], Rcam[9], KRcam[9];

	CG_matrix = cvMat( 3, 3, CV_64FC1, RotCGMat);

	R1Mat    = cvMat(3, 3, CV_64FC1, refRot);
	R2Mat    = cvMat(3, 3, CV_64FC1, curRot);		
	R1TR2Mat = cvMat(3, 3, CV_64FC1, r1Tr2);
	RcgRgMat = cvMat(3, 3, CV_64FC1, RcgRg);
	RcamMat  = cvMat(3, 3, CV_64FC1, Rcam);

	cvGEMM(&R1Mat,     &R2Mat,     1, 0, 0, &R1TR2Mat, CV_GEMM_B_T);
	cvGEMM(&CG_matrix, &R1TR2Mat,  1, 0, 0, &RcgRgMat, 0);
	cvGEMM(&RcgRgMat,  &CG_matrix, 1, 0, 0, &RcamMat,  CV_GEMM_B_T);

	matrixMultiply(K, Rcam, 3, 3, 3, KRcam);
	matrixMultiply(KRcam, invK, 3, 3, 3, Horn);
}
/********************************************************************************/
