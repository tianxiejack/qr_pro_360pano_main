#ifndef _GYRO_CALBIATE_H_
#define _GYRO_CALBIATE_H_

typedef struct _rotAngle_
{
	double	rot_x;
	double	rot_y;
	double	rot_z;
}RotAngle;

typedef struct _GYROSTB_CALIBRATE
{
	double	td_time;	// time diff between video and gyro (video_time - gyro_time ),ms

	double	rot_x;	    // RCG_x
	double  rot_y;	    // RCG_y
	double	rot_z;	    // RCG_z

	double	gbias_x;
	double	gbias_y;
	double	gbias_z;

	double  cam_readout; // 31 us
	double  cam_freq;	 // 30 fps

	int     cam_video_line;
}CALIB_PRMS;

void Calib_parms_set(CALIB_PRMS* calib_prm_in, CALIB_PRMS* calib_prm_out);
void Calib_rotRCG_transfer(CALIB_PRMS* calibParms_in, double RotCGVec[3],double RotCGMat[9]);

#endif