#ifndef _GYRO_QUATER_H_
#define _GYRO_QUATER_H_

#include "gyro.h"

void slerp(double q1[], double q2[], double u, double q_alph[]);
void quat_to_rotation_matrix(double q[], double rotation[]);
void Gyro_set_ref_quateration(GYRO_HNDL gyroHndl, double read_out_time, double Fg, double frame_ts, double td_time);
void Gyro_gen_quateration(GYRO_HNDL gyroHndl, GYRO_DATA_T* pGyroElem);
void Gyro_quat_to_rotation_matrix( GYRO_DATA_T *lpGyroData, double Fg, double basetime ,double curptimes,double *curRot);
void Gyro_quat_to_rotation_matrix_new( GYRO_DATA_T *lpGyroData, double Fg, double basetime ,double curptimes, double curstarttime, double *curRot);

#endif