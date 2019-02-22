
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "cv.h"
#include "cxcore.h"
#include "gyro_calibrate.h"

void Calib_parms_set(CALIB_PRMS* calib_prm_in, CALIB_PRMS* calib_prm_out)
{
	assert(calib_prm_in != NULL && calib_prm_out!=NULL);

	memcpy(calib_prm_out,calib_prm_in,sizeof(CALIB_PRMS));
}

void Calib_rotRCG_transfer(CALIB_PRMS* calibParms_in, double RotCGVec[3],double RotCGMat[9])
{
	CvMat CG_matrix;
	CvMat CG_vector;

	RotCGVec[0] = calibParms_in->rot_x;
	RotCGVec[1] = calibParms_in->rot_y;
	RotCGVec[2] = calibParms_in->rot_z;

	CG_matrix = cvMat( 3, 3, CV_64FC1, RotCGMat);
	CG_vector = cvMat( 3, 1, CV_64FC1, RotCGVec);
	cvRodrigues2(&CG_vector, &CG_matrix,0);
}