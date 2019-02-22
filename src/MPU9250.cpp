#include"MPU9250.hpp"
#include"osa.h"
#include "math.h"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/video/tracking.hpp"
#include <stdio.h>
#include"config.h"
#include"Stich.hpp"
using namespace cv;
using namespace cv;
using namespace std;



//#ifdef BUTTERFLY
//#define gryo_scale   	(3.141592653/16384/180.0)	
//#else
#define gryo_scale   	(500.0/65536.0*3.141592653/180.0)	
//#endif

//#define gryo_scale   	(1>>14)	
//#define gryo_scale   	(500.0/65536.0)
#define accel_scale 	(4.0/65536.0)												
#define mag_scale 	  (9600.0/16384.0/100.0)							


/***********************�궨��***********************/
#define Kp 40.0f                  
	
#define Ki 0.02f          // integral gain governs rate of convergenceof gyroscope biases
	
#define halfT 0.004f      // half the sample period  
	
#define dt 0.008f		
/***************************************************/

static float q0=1.0f,q1=0.0f,q2=0.0f,q3=0.0f;

static double gyrollbase=0,gypitchbase=0,gyyawbase=0;

static double gyroll=0,gypitch=0,gyyaw=0;
static float exInt = 0, eyInt = 0, ezInt = 0; 
static short turns=0;
static float newdata=0.0f,olddata=0.0f;
static float pitchoffset,rolloffset,yawoffset;

static float k10=0.0f,k11=0.0f,k12=0.0f,k13=0.0f;
static float k20=0.0f,k21=0.0f,k22=0.0f,k23=0.0f;
static float k30=0.0f,k31=0.0f,k32=0.0f,k33=0.0f;
static float k40=0.0f,k41=0.0f,k42=0.0f,k43=0.0f;






#ifdef BUTTERFLY
int gyro_offsetx=0,gyro_offsety=0,gyro_offsetz=0;
#else
short gyro_offsetx=0,gyro_offsety=0,gyro_offsetz=0;
#endif
float tmp1,tmp2,tmp3;
float magoffsetx=1.31454428611172,magoffsety=-1.21753632395713,magoffsetz=1.6567777185719;
float B[6]={0.980358187761106,-0.0105514731414606,0.00754899338354401,0.950648704823113,-0.0354995317649016,1.07449478456729};
float accoffsetx=-0.0118443148713821,accoffsety=-0.00939616830387326,accoffsetz=-0.0507891438815726;
float accsensx=1.00851297697413,accsensy=0.991366909333871,accsensz=1.00019364448499;

#define filter_high 0.8
#define filter_low 	0.2
short accoldx,accoldy,accoldz;
short magoldx,magoldy,magoldz;
short gyrooldx,gyrooldy,gyrooldz;



KalmanFilter KF;
Mat Xstate;
Mat QprocessNoise;
Mat Zmeasurement;


float invSqrt(float number);
void AHRSupdate(float gx, float gy, float gz, float ax, float ay, float az, float mx, float my, float mz,float *roll,float *pitch,float *yaw);
void CountTurns(float *newdata,float *olddata,short *turns);
void CalYaw(float *yaw,short *turns);
void CalibrateToZero(void);
/*

int main(void)
{ 
	u8 t=0;

	float pitch,roll,yaw;
	short igx,igy,igz;
	short iax,iay,iaz;
	short imx,imy,imz;
	float gx,gy,gz;
	float ax,ay,az;
	float mx,my,mz;

	Stm32_Clock_Init(336,8,2,7);//����ʱ��,168Mhz
	delay_init(168);		//��ʼ����ʱ����
	uart_init(84,115200);	//���ڳ�ʼ��Ϊ115200
	t=MPU_Init();
	
	while (t)
	{
		printf("MPU_Init error ");
		delay_ms(200);
	}
	CalibrateToZero();
		while(1)
	{
			
			MPU_Get_Gyro(&igx,&igy,&igz,&gx,&gy,&gz);
			MPU_Get_Accel(&iax,&iay,&iaz,&ax,&ay,&az);
			MPU_Get_Mag(&imx,&imy,&imz,&mx,&my,&mz);
			AHRSupdate(gx,gy,gz,ax,ay,az,mx,my,mz,&roll,&pitch,&yaw);
			olddata=newdata;
			newdata=yaw;
			CountTurns(&newdata,&olddata,&turns);
			CalYaw(&yaw,&turns);
			pitch+=pitchoffset;
			roll+=rolloffset;
			yaw+=yawoffset;
			printf("%0.1f %0.1f %0.1f",roll,pitch,yaw);	
		  printf("\r\n");
			delay_us(5270);

		

	}

}

*/


float invSqrt(float number)
{
	long i;
	float x,y;
	const float f=1.5f;
	
	x=number*0.5f;
	y=number;
	i=*((long*)&y);
	i=0x5f375a86-(i>>1);
	y=*((float *)&i);
	y=y*(f-(x*y*y));
	return y;
}
typedef struct
{
	double w;
	double x;
	double y;
	double z;

}Quaterniond;





void quat_2_rotation_matrix(double q[], double rotation[])
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

Vec3f rotationMatrix2EulerAngles(Mat &R)
{



    float sy = sqrt(R.at<double>(0,0) * R.at<double>(0,0) +  R.at<double>(1,0) * R.at<double>(1,0) );

    bool singular = sy < 1e-6; // If

    float x, y, z;
    if (!singular)
    {
    	 //printf("***************usr right*****************\n");
        x = atan2(R.at<double>(2,1) , R.at<double>(2,2));
        y = atan2(-R.at<double>(2,0), sy);
        z = atan2(R.at<double>(1,0), R.at<double>(0,0));
    }
    else
    {
        x = atan2(-R.at<double>(1,2), R.at<double>(1,1));
        y = atan2(-R.at<double>(2,0), sy);
        z = 0;
    }
    return Vec3f(x, y, z);
}



void quat2euler(double q[], double& roll, double& pitch, double& yaw)
{
	double rotaion[9];
	Mat rotmat=Mat(3,3,CV_64FC1,rotaion);
	quat_2_rotation_matrix(q,rotaion);
	Vec3f euler=rotationMatrix2EulerAngles(rotmat);
	roll=euler[0];
	pitch=euler[1];
	yaw=euler[2];

}
static void toEulerAngle(const Quaterniond& q, double& roll, double& pitch, double& yaw)
{
	// roll (x-axis rotation)
	double sinr_cosp = +2.0 * (q.w * q.x + q.y * q.z);
	double cosr_cosp = +1.0 - 2.0 * (q.x * q.x + q.y* q.y);
	roll = atan2(sinr_cosp, cosr_cosp);

	// pitch (y-axis rotation)
	double sinp = +2.0 * (q.w * q.y - q.z * q.x);
	if (fabs(sinp) >= 1)
		pitch = copysign(M_PI / 2, sinp); // use 90 degrees if out of range
	else
		pitch = asin(sinp);

	// yaw (z-axis rotation)
	double siny_cosp = +2.0 * (q.w * q.z + q.x * q.y);
	double cosy_cosp = +1.0 - 2.0 * (q.y * q.y + q.z * q.z);  
	yaw = atan2(siny_cosp, cosy_cosp);
}


void mpu9250reset()
{
	gyroll=0;
	gypitch=0;
	gyyaw=0;
}
void getQunterandEuler(GYRO_MPU*mpu)
{
	     float norm;								
           float hx, hy, hz, bx, bz;		//
           float vx, vy, vz, wx, wy, wz; 
           float ex, ey, ez;
	     float tmp0,tmp1,tmp2,tmp3;
	     double gx=mpu->gyro_gx;
	     double gy=mpu->gyro_gy;
	     double gz=mpu->gyro_gz;
	     float ax=mpu->gyro_ax;
	     float ay=mpu->gyro_ay;
	     float az=mpu->gyro_az;
	     float mx=mpu->gyro_mx;
	     float my=mpu->gyro_my;
	     float mz=mpu->gyro_mz;

	     ax=0;ay=0;az=0;mx=0;my=0;mz=0;//for test

		 
 
           float q0q0 = q0*q0;
           float q0q1 = q0*q1;
           float q0q2 = q0*q2;
           float q0q3 = q0*q3;
           float q1q1 = q1*q1;
           float q1q2 = q1*q2;
           float q1q3 = q1*q3;
           float q2q2 = q2*q2;
           float q2q3 = q2*q3;
           float q3q3 = q3*q3;
		//u2s
	     float timeinterval=(mpu->gyro_timestamp-mpu->gyro_pretimestamp)/1000000.0;
	     float halftimeinterval=timeinterval/2;
/*
	    norm = sqrt(ax*ax + ay*ay + az*az);
           ax = ax / norm;
           ay = ay / norm;
           az = az / norm;
           norm = sqrt(mx*mx + my*my + mz*mz);
           mx = mx / norm;
           my = my / norm;
           mz = mz / norm;
          
   
           hx = 2*mx*(0.50 - q2q2 - q3q3) + 2*my*(q1q2 - q0q3) + 2*mz*(q1q3 + q0q2);
           hy = 2*mx*(q1q2 + q0q3) + 2*my*(0.50 - q1q1 - q3q3) + 2*mz*(q2q3 - q0q1);
           hz = 2*mx*(q1q3 - q0q2) + 2*my*(q2q3 + q0q1) + 2*mz*(0.50 - q1q1 -q2q2);    
           bx = sqrt((hx*hx) + (hy*hy));
           bz = hz;
          
           vx = 2*(q1q3 - q0q2);
           vy = 2*(q0q1 + q2q3);
           vz = q0q0 - q1q1 - q2q2 + q3q3;
	
           wx = 2*bx*(0.5 - q2q2 - q3q3) + 2*bz*(q1q3 - q0q2);
           wy = 2*bx*(q1q2 - q0q3) + 2*bz*(q0q1 + q2q3);
           wz = 2*bx*(q0q2 + q1q3) + 2*bz*(0.5 - q1q1 - q2q2); 
          

           ex = (ay*vz - az*vy) + (my*wz - mz*wy);
           ey = (az*vx - ax*vz) + (mz*wx - mx*wz);
           ez = (ax*vy - ay*vx) + (mx*wy - my*wx);


           exInt = exInt + ex*Ki*dt;
           eyInt = eyInt + ey*Ki*dt;
           ezInt = ezInt + ez*Ki*dt;

					
           gx = gx + Kp*ex + exInt;
           gy = gy + Kp*ey + eyInt;
           gz = gz + Kp*ez + ezInt;


	   */
//printf("gx=%f gy=%f gz=%f  timeinterval=%f",gx,gy,gz,timeinterval);


#if 1
gyroll+=gx*timeinterval;
gypitch+=gy*timeinterval;
gyyaw+=gz*timeinterval;


//gyroll+=gyrollbase;
//gypitch+=gypitchbase;
//gyyaw+=gyyawbase;

#else
	
         #if 1 
           // integrate quaernion rate aafnd normalaizle	
           tmp0 = q0 + (-q1*gx - q2*gy - q3*gz)*halftimeinterval;
           tmp1 = q1 + ( q0*gx + q2*gz - q3*gy)*halftimeinterval;
           tmp2 = q2 + ( q0*gy - q1*gz + q3*gx)*halftimeinterval;
           tmp3 = q3 + ( q0*gz + q1*gy - q2*gx)*halftimeinterval; 
           q0=tmp0;
           q1=tmp1;
           q2=tmp2;
           q3=tmp3;
					 //printf("q0=%0.1f q1=%0.1f q2=%0.1f q3=%0.1f",q0,q1,q2,q3);
	 #else

		  k10=0.5 * (-gx*q1 - gy*q2 - gz*q3);
			k11=0.5 * ( gx*q0 + gz*q2 - gy*q3);
			k12=0.5 * ( gy*q0 - gz*q1 + gx*q3);
			k13=0.5 * ( gz*q0 + gy*q1 - gx*q2);
			
			k20=0.5 * (halfT*(q0+halfT*k10) + (halfT-gx)*(q1+halfT*k11) + (halfT-gy)*(q2+halfT*k12) + (halfT-gz)*(q3+halfT*k13));
			k21=0.5 * ((halfT+gx)*(q0+halfT*k10) + halfT*(q1+halfT*k11) + (halfT+gz)*(q2+halfT*k12) + (halfT-gy)*(q3+halfT*k13));
			k22=0.5 * ((halfT+gy)*(q0+halfT*k10) + (halfT-gz)*(q1+halfT*k11) + halfT*(q2+halfT*k12) + (halfT+gx)*(q3+halfT*k13));
			k23=0.5 * ((halfT+gz)*(q0+halfT*k10) + (halfT+gy)*(q1+halfT*k11) + (halfT-gx)*(q2+halfT*k12) + halfT*(q3+halfT*k13));
			
			k30=0.5 * (halfT*(q0+halfT*k20) + (halfT-gx)*(q1+halfT*k21) + (halfT-gy)*(q2+halfT*k22) + (halfT-gz)*(q3+halfT*k23));
			k31=0.5 * ((halfT+gx)*(q0+halfT*k20) + halfT*(q1+halfT*k21) + (halfT+gz)*(q2+halfT*k22) + (halfT-gy)*(q3+halfT*k23));
			k32=0.5 * ((halfT+gy)*(q0+halfT*k20) + (halfT-gz)*(q1+halfT*k21) + halfT*(q2+halfT*k22) + (halfT+gx)*(q3+halfT*k23));
			k33=0.5 * ((halfT+gz)*(q0+halfT*k20) + (halfT+gy)*(q1+halfT*k21) + (halfT-gx)*(q2+halfT*k22) + halfT*(q3+halfT*k23));
			
			k40=0.5 * (dt*(q0+dt*k30) + (dt-gx)*(q1+dt*k31) + (dt-gy)*(q2+dt*k32) + (dt-gz)*(q3+dt*k33));
			k41=0.5 * ((dt+gx)*(q0+dt*k30) + dt*(q1+dt*k31) + (dt+gz)*(q2+dt*k32) + (dt-gy)*(q3+dt*k33));
			k42=0.5 * ((dt+gy)*(q0+dt*k30) + (dt-gz)*(q1+dt*k31) + dt*(q2+dt*k32) + (dt+gx)*(q3+dt*k33));
			k43=0.5 * ((dt+gz)*(q0+dt*k30) + (dt+gy)*(q1+dt*k31) + (dt-gx)*(q2+dt*k32) + dt*(q3+dt*k33));	
			
			q0=q0 + dt/6.0 * (k10+2*k20+2*k30+k40);
			q1=q1 + dt/6.0 * (k11+2*k21+2*k31+k41);
			q2=q2 + dt/6.0 * (k12+2*k22+2*k32+k42);
			q3=q3 + dt/6.0 * (k13+2*k23+2*k33+k43);
			#endif			  
           // normalise quaternion
        #if 1
           norm = sqrt(q0*q0 + q1*q1 + q2*q2 + q3*q3);
           q0 = q0 / norm;
           q1 = q1 / norm;
           q2 = q2 / norm;
           q3 = q3/ norm;
	#else
		
	norm = invSqrt(q0*q0 + q1*q1 + q2*q2 + q3*q3);
  	q0 = q0 * norm;
  	q1 = q1 * norm;
  	q2 = q2 * norm;
  	q3 = q3 * norm;
  	
	#endif
	#if 1			 
	 gypitch	= asin(-2 * q1 * q3 + 2 * q0 * q2);	// pitch
	 gyroll	= atan2(2 * q2 * q3 + 2 * q0 * q1, -2 * q1 * q1 - 2 * q2 * q2 + 1);	// roll
	 gyyaw  = atan2(2*(q1*q2 + q0*q3),-2 * q3 * q3 - 2 * q2 * q2 + 1);	//yaw
	 #elif 0
	double q[4];
	 q[0]=q0;
	 q[1]=q1;
	 q[2]=q2;
	 q[3]=q3;

	 quat2euler(q,mpu->roll,mpu->pitch,mpu->yaw);
	
	
	 
	 #else
	Quaterniond q;
	 q.w=q0;
	 q.x=q1;
	 q.y=q2;
	 q.z=q3;
	 toEulerAngle(q,mpu->roll,mpu->pitch,mpu->yaw);

	 #endif

	// printf("euler roll=%f pitch=%f yaw=%f\n",mpu->roll*180/3.141592653,mpu->pitch*180/3.141592653,mpu->yaw*180/3.141592653);


#endif


bool norma=true;


while(norma)
{
	if(gyroll>2*3.141592653)
		gyroll=gyroll-2*3.141592653;
	else if(gyroll<-2*3.141592653)
		gyroll=gyroll+2*3.141592653;
	else
		norma=false;
}

norma=true;
while(norma)
{
	if(gypitch>2*3.141592653)
		gypitch=gypitch-2*3.141592653;
	else if(gypitch<-2*3.141592653)
		gypitch=gypitch+2*3.141592653;
	else
		norma=false;
}
norma=true;

while(norma)
{
	if(gyyaw>2*3.141592653)
		gyyaw=gyyaw-2*3.141592653;
	else if(gyyaw<-2*3.141592653)
		gyyaw=gyyaw+2*3.141592653;
	else
		norma=false;
}

mpu->roll=gyroll;
mpu->pitch=gypitch;
mpu->yaw=gyyaw;

}

void AHRSupdate(GYRO_MPU*mpu)
{
		//;
	  
           float norm;								
           float hx, hy, hz, bx, bz;		//
           float vx, vy, vz, wx, wy, wz; 
           float ex, ey, ez;
					 float tmp0,tmp1,tmp2,tmp3;
	        float gx=mpu->gyro_gx;
	     float gy=mpu->gyro_gy;
	     float gz=mpu->gyro_gz;
	     float ax=mpu->gyro_ax;
	     float ay=mpu->gyro_ay;
	     float az=mpu->gyro_az;
	     float mx=mpu->gyro_mx;
	     float my=mpu->gyro_my;
	     float mz=mpu->gyro_mz;
 
           float q0q0 = q0*q0;
           float q0q1 = q0*q1;
           float q0q2 = q0*q2;
           float q0q3 = q0*q3;
           float q1q1 = q1*q1;
           float q1q2 = q1*q2;
           float q1q3 = q1*q3;
           float q2q2 = q2*q2;
           float q2q3 = q2*q3;
           float q3q3 = q3*q3;
          
      
           norm = invSqrt(ax*ax + ay*ay + az*az);
           ax = ax * norm;
           ay = ay * norm;
           az = az * norm;
           norm = invSqrt(mx*mx + my*my + mz*mz);
           mx = mx * norm;
           my = my * norm;
           mz = mz * norm;
          
   
           hx = 2*mx*(0.50 - q2q2 - q3q3) + 2*my*(q1q2 - q0q3) + 2*mz*(q1q3 + q0q2);
           hy = 2*mx*(q1q2 + q0q3) + 2*my*(0.50 - q1q1 - q3q3) + 2*mz*(q2q3 - q0q1);
           hz = 2*mx*(q1q3 - q0q2) + 2*my*(q2q3 + q0q1) + 2*mz*(0.50 - q1q1 -q2q2);    
           bx = sqrt((hx*hx) + (hy*hy));
           bz = hz;
          
           vx = 2*(q1q3 - q0q2);
           vy = 2*(q0q1 + q2q3);
           vz = q0q0 - q1q1 - q2q2 + q3q3;
	
           wx = 2*bx*(0.5 - q2q2 - q3q3) + 2*bz*(q1q3 - q0q2);
           wy = 2*bx*(q1q2 - q0q3) + 2*bz*(q0q1 + q2q3);
           wz = 2*bx*(q0q2 + q1q3) + 2*bz*(0.5 - q1q1 - q2q2); 
          

           ex = (ay*vz - az*vy) + (my*wz - mz*wy);
           ey = (az*vx - ax*vz) + (mz*wx - mx*wz);
           ez = (ax*vy - ay*vx) + (mx*wy - my*wx);


           exInt = exInt + ex*Ki*dt;
           eyInt = eyInt + ey*Ki*dt;
           ezInt = ezInt + ez*Ki*dt;

					
           gx = gx + Kp*ex + exInt;
           gy = gy + Kp*ey + eyInt;
           gz = gz + Kp*ez + ezInt;
	//printf("gx=%0.1f gy=%0.1f gz=%0.1f",gx,gy,gz);
         #if 1 
           // integrate quaernion rate aafnd normalaizle
				
           tmp0 = q0 + (-q1*gx - q2*gy - q3*gz)*halfT;
           tmp1 = q1 + ( q0*gx + q2*gz - q3*gy)*halfT;
           tmp2 = q2 + ( q0*gy - q1*gz + q3*gx)*halfT;
           tmp3 = q3 + ( q0*gz + q1*gy - q2*gx)*halfT; 
					 q0=tmp0;
					 q1=tmp1;
					 q2=tmp2;
					 q3=tmp3;
					 //printf("q0=%0.1f q1=%0.1f q2=%0.1f q3=%0.1f",q0,q1,q2,q3);
		#else

		  k10=0.5 * (-gx*q1 - gy*q2 - gz*q3);
			k11=0.5 * ( gx*q0 + gz*q2 - gy*q3);
			k12=0.5 * ( gy*q0 - gz*q1 + gx*q3);
			k13=0.5 * ( gz*q0 + gy*q1 - gx*q2);
			
			k20=0.5 * (halfT*(q0+halfT*k10) + (halfT-gx)*(q1+halfT*k11) + (halfT-gy)*(q2+halfT*k12) + (halfT-gz)*(q3+halfT*k13));
			k21=0.5 * ((halfT+gx)*(q0+halfT*k10) + halfT*(q1+halfT*k11) + (halfT+gz)*(q2+halfT*k12) + (halfT-gy)*(q3+halfT*k13));
			k22=0.5 * ((halfT+gy)*(q0+halfT*k10) + (halfT-gz)*(q1+halfT*k11) + halfT*(q2+halfT*k12) + (halfT+gx)*(q3+halfT*k13));
			k23=0.5 * ((halfT+gz)*(q0+halfT*k10) + (halfT+gy)*(q1+halfT*k11) + (halfT-gx)*(q2+halfT*k12) + halfT*(q3+halfT*k13));
			
			k30=0.5 * (halfT*(q0+halfT*k20) + (halfT-gx)*(q1+halfT*k21) + (halfT-gy)*(q2+halfT*k22) + (halfT-gz)*(q3+halfT*k23));
			k31=0.5 * ((halfT+gx)*(q0+halfT*k20) + halfT*(q1+halfT*k21) + (halfT+gz)*(q2+halfT*k22) + (halfT-gy)*(q3+halfT*k23));
			k32=0.5 * ((halfT+gy)*(q0+halfT*k20) + (halfT-gz)*(q1+halfT*k21) + halfT*(q2+halfT*k22) + (halfT+gx)*(q3+halfT*k23));
			k33=0.5 * ((halfT+gz)*(q0+halfT*k20) + (halfT+gy)*(q1+halfT*k21) + (halfT-gx)*(q2+halfT*k22) + halfT*(q3+halfT*k23));
			
			k40=0.5 * (dt*(q0+dt*k30) + (dt-gx)*(q1+dt*k31) + (dt-gy)*(q2+dt*k32) + (dt-gz)*(q3+dt*k33));
			k41=0.5 * ((dt+gx)*(q0+dt*k30) + dt*(q1+dt*k31) + (dt+gz)*(q2+dt*k32) + (dt-gy)*(q3+dt*k33));
			k42=0.5 * ((dt+gy)*(q0+dt*k30) + (dt-gz)*(q1+dt*k31) + dt*(q2+dt*k32) + (dt+gx)*(q3+dt*k33));
			k43=0.5 * ((dt+gz)*(q0+dt*k30) + (dt+gy)*(q1+dt*k31) + (dt-gx)*(q2+dt*k32) + dt*(q3+dt*k33));	
			
			q0=q0 + dt/6.0 * (k10+2*k20+2*k30+k40);
			q1=q1 + dt/6.0 * (k11+2*k21+2*k31+k41);
			q2=q2 + dt/6.0 * (k12+2*k22+2*k32+k42);
			q3=q3 + dt/6.0 * (k13+2*k23+2*k33+k43);
			#endif			
           // normalise quaternion
           norm = invSqrt(q0*q0 + q1*q1 + q2*q2 + q3*q3);
           q0 = q0 * norm;
           q1 = q1 * norm;
           q2 = q2 * norm;
           q3 = q3 * norm;
					 
	 mpu->pitch	= asin(-2 * q1 * q3 + 2 * q0 * q2)* 57.3;	// pitch
	 mpu->roll 	      = atan2(2 * q2 * q3 + 2 * q0 * q1, -2 * q1 * q1 - 2 * q2 * q2 + 1)* 57.3;	// roll
	 mpu->yaw       = atan2(2*(q1*q2 + q0*q3),q0*q0+q1*q1-q2*q2-q3*q3) * 57.3;	//yaw
}


void CountTurns(float *newdata,float *olddata,short *turns)
{
	if (*newdata<-170.0f && *olddata>170.0f)
		(*turns)++;
	if (*newdata>170.0f && *olddata<-170.0f)
		(*turns)--;

}


void CalYaw(float *yaw,short *turns)
{
	*yaw=360.0**turns+*yaw;
}

///////////////////////////////////////////////////
/////////////////////////////////
//�õ�������ֵ(ԭʼֵ)����Դ����ƽ��ֵ�˲��������������Ƿ�λ
//gx,gy,gz:������x,y,z���ԭʼ����(������)
//����ֵ:0,�ɹ�
//    ����,�������
unsigned char MPU_Get_Gyroscope( short *gx, short *gy, short *gz)
{
    unsigned char buf[6],res=0; 
  
	//res=MPU_Read_Len(MPU9250_ADDR,MPU_GYRO_XOUTH_REG,6,buf);
	if(res==0)
	{
		*gy=*gy+gyro_offsety;  
		*gx=*gx+gyro_offsetx;  
		*gz=*gz+gyro_offsetz;
		  if(GYRO_NOFILTER)
		return 0;
		*gx=-*gx;
		*gx=(short)(gyrooldx*0.5+*gx*0.5);
		*gy=(short)(gyrooldy*0.5+*gy*0.5);
		*gz=(short)(gyrooldz*0.5+*gz*0.5);
		gyrooldx=*gx;
		gyrooldy=*gy;
		gyrooldz=*gz;
		
	} 	
    return res;
}

unsigned char MPU_Get_Gyroscopeint( int *gx, int *gy, int *gz)
{
    unsigned char buf[6],res=0; 
  
	//res=MPU_Read_Len(MPU9250_ADDR,MPU_GYRO_XOUTH_REG,6,buf);
	if(res==0)
	{
		*gy=*gy+gyro_offsety;  
		*gx=*gx+gyro_offsetx;  
		*gz=*gz+gyro_offsetz;
		  if(GYRO_NOFILTER)
		return 0;
		*gx=-*gx;
		*gx=(short)(gyrooldx*0.5+*gx*0.5);
		*gy=(short)(gyrooldy*0.5+*gy*0.5);
		*gz=(short)(gyrooldz*0.5+*gz*0.5);
		gyrooldx=*gx;
		gyrooldy=*gy;
		gyrooldz=*gz;
		
	} 	
    return res;
}
unsigned char MPU_Get_Gyro( void *igx1, void  *igy1, void *igz1,double *gx,double *gy,double *gz)
{
	unsigned char res;
	short *igx=(short *)igx1;
	short  *igy=(short *)igy1;
	short *igz=(short *)igz1;

	res=MPU_Get_Gyroscope(igx,igy,igz);
	if (res==0)
	{


			*gx=(float)(*igx)*gryo_scale;
			*gy=(float)(*igy)*gryo_scale;
			*gz=(float)(*igz)*gryo_scale;


		
	}
	return res;
}

unsigned char MPU_Get_Gyroint( void *igx1, void   *igy1, void  *igz1,double *gx,double *gy,double *gz)
{
	unsigned char res;
	int *igx=(int *)igx1;
	int  *igy=(int *)igy1;
	int *igz=(int *)igz1;
	res=MPU_Get_Gyroscopeint(igx,igy,igz);
	if (res==0)
	{

	if(1)
		{
			*gx=(double)(*igx)*gryo_scale;
			*gy=(double)(*igy)*gryo_scale;
			*gz=(double)(*igz)*gryo_scale;
		}
	else
		{
			*gx=(float)(*igx)*gryo_scale;
			*gy=(float)(*igy)*gryo_scale;
			*gz=(float)(*igz)*gryo_scale;


		}
	}
	return res;
}
//�õ����ٶ�ֵ(ԭʼֵ)����ͨ�˲����������ٶȼƷ�λ
//gx,gy,gz:������x,y,z���ԭʼ����(������)
//����ֵ:0,�ɹ�
//    ����,�������
unsigned char MPU_Get_Accelerometer(short *ax,short *ay,short *az)
{
    unsigned char buf[6],res=0;
	if(GYRO_NOFILTER)
		return 0;
	//res=MPU_Read_Len(MPU9250_ADDR,MPU_ACCEL_XOUTH_REG,6,buf);
	if(res==0)
	{
		
		*ax=-*ax;
		*ax=(short)(accoldx*filter_high+*ax*filter_low);
		*ay=(short)(accoldy*filter_high+*ay*filter_low);
		*az=(short)(accoldz*filter_high+*az*filter_low);
		accoldx=*ax;
		accoldy=*ay;
		accoldz=*az;
		
	} 	
    return res;
}
/*//////////////////////////////////////////////////
*@���ܣ���ü��ٶȼ����ݣ���λg�����Լ��ٶȼƽ��в���
*
*
///////////////////////////////////////////////////*/
unsigned char MPU_Get_Accel( short  *iax, short  *iay, short  *iaz,double *ax,double *ay,double *az)
{
	unsigned char res;
	res=MPU_Get_Accelerometer(iax,iay,iaz);
	if (res==0)
	{
	tmp1=(float)(*iax)*accel_scale-accoffsetx;
	tmp2=(float)(*iay)*accel_scale-accoffsety;
	tmp3=(float)(*iaz)*accel_scale-accoffsetz;
	*ax=tmp1*accsensx;
	*ay=tmp2*accsensy;
	*az=tmp3*accsensz;
	}
	return res;
}

unsigned char MPU_Get_Magnetometer(short *mx,short *my,short *mz)
{
    unsigned charbuf[6],res=0;  
    if(GYRO_NOFILTER)
		return 0;
		
 	//res=MPU_Read_Len(AK8963_ADDR,MAG_XOUT_L,6,buf);
	if(res==0)
	{

		*my=-*my;
		*mz=-*mz;
		*mx=(short)(magoldx*0.5+*mx*0.5);
		*my=(short)(magoldy*0.5+*my*0.5);
		*mz=(short)(magoldz*0.5+*mz*0.5);
		magoldx=*mx;
		magoldy=*my;
		magoldz=*mz;
	} 	 
	
    return res;
}
/*//////////////////////////////////////////////////
*@���ܣ���ô��������ݣ���λ��˹�����Դ����ƽ��в���
*
*
///////////////////////////////////////////////////*/
unsigned char MPU_Get_Mag(short *imx,short *imy,short *imz,double *mx,double *my,double *mz)
{
	unsigned char res;
	res=MPU_Get_Magnetometer(imx,imy,imz);
	if (res==0)
	{
	tmp1=(float)(*imx)*mag_scale-magoffsetx;
	tmp2=(float)(*imy)*mag_scale-magoffsety;
	tmp3=(float)(*imz)*mag_scale-magoffsetz;
	*mx=B[0]*tmp1+B[1]*tmp2+B[2]*tmp3;
	*my=B[1]*tmp1+B[3]*tmp2+B[4]*tmp3;
	*mz=B[2]*tmp1+B[4]*tmp2+B[5]*tmp3;
	}
	return res;
}

#define CALIHEAD 100
#define CALILEN 50


unsigned int  calibreatetimes=0;
#define CALIBREATIONTIME 2000
unsigned char  calibrate(void *gx1, void *gy1, void *gz1)
{
	unsigned char t;
	static int sumx=0,sumy=0,sumz=0;
	short *gx=(short *)gx1;
	short *gy=(short *)gy1;
	short *gz=(short *)gz1;
	
	if(calibreatetimes>=CALIBREATIONTIME)
		return 1;
	//for (t=0;t<100;t++)
	{
		MPU_Get_Gyroscope(gx,gy,gz);
		if(calibreatetimes>CALIBREATIONTIME/2)
			{
				sumx=sumx+*gx;
				sumy=sumy+*gy;
				sumz=sumz+*gz;
			}
	}
	if(calibreatetimes==CALIBREATIONTIME-1)
		{
			gyro_offsetx=-sumx*2/CALIBREATIONTIME;
			gyro_offsety=-sumy*2/CALIBREATIONTIME;
			gyro_offsetz=-sumz*2/CALIBREATIONTIME;
		}
	calibreatetimes++;
	
	return 0;

}

unsigned char  calibrateint(void *gx1, void *gy1, void *gz1)
{
	unsigned char t;
	static long int sumx=0,sumy=0,sumz=0;
	int *gx=(int *)gx1;
	int *gy=(int *)gy1;
	int *gz=(int *)gz1;
	if(calibreatetimes>=CALIBREATIONTIME)
		return 1;
	//for (t=0;t<100;t++)
	{
		MPU_Get_Gyroscopeint(gx,gy,gz);
		sumx=sumx+*gx;
		sumy=sumy+*gy;
		sumz=sumz+*gz;
	}
	if(calibreatetimes==CALIBREATIONTIME-1)
		{
			gyro_offsetx=-sumx/CALIBREATIONTIME;
			gyro_offsety=-sumy/CALIBREATIONTIME;
			gyro_offsetz=-sumz/CALIBREATIONTIME;
		}
	calibreatetimes++;
	
	return 0;

}

void CalibrateToZero(GYRO_MPU*mpu)
{
	static unsigned char  t=0;
	static float sumpitch=0,sumroll=0,sumyaw=0;
	float pitch,roll,yaw;
	short igx,igy,igz;
	short iax,iay,iaz;
	short imx,imy,imz;
	float gx,gy,gz;
	float ax,ay,az;
	float mx,my,mz;
	if(t<=CALIHEAD+CALILEN)
	//	for (t=0;t<150;t++)
	{
		MPU_Get_Gyroint(&mpu->gyro_gxi,&mpu->gyro_gyi,&mpu->gyro_gzi,&mpu->gyro_gx,&mpu->gyro_gy,&mpu->gyro_gz);
		MPU_Get_Accel(&mpu->gyro_axi,&mpu->gyro_ayi,&mpu->gyro_azi,&mpu->gyro_ax,&mpu->gyro_ay,&mpu->gyro_az);
		MPU_Get_Mag(&mpu->gyro_mxi,&mpu->gyro_myi,&mpu->gyro_mzi,&mpu->gyro_mx,&mpu->gyro_my,&mpu->gyro_mz);
		//AHRSupdate(gx,gy,gz,ax,ay,az,mx,my,mz,&roll,&pitch,&yaw);
		getQunterandEuler(mpu);
		//OSA_printf("roll=%f pitch=%f yaw=%f\n",roll,pitch,yaw);

		t++;
	if (t>=CALIHEAD)
		{
			sumpitch+=pitch;
			sumroll+=roll;
			sumyaw+=yaw;
		}
		else
			return ;
	}
	if(t==CALIHEAD+CALILEN)
		{
			pitchoffset=-sumpitch/50.0f;
			rolloffset=-sumroll/50.0f;
			yawoffset=-sumyaw/50.0f;
		}
	else if(t>CALIHEAD+CALILEN)
		{
			t=CALIHEAD+CALILEN+1;
		}
}


void setgyyawbase(double angle)
{
	gyyawbase=angle;

}

void gyrokalmanfilterinit()
{
	 KF=KalmanFilter(1, 1, 0);
	 Xstate=Mat(1, 1, CV_32F); /* 状态矩阵：1个状态，角度 *///X
	 QprocessNoise=Mat(1, 1, CV_32F); //过程噪声矩阵
	 Zmeasurement= Mat::zeros(1, 1, CV_32F);//观测值，
	 KF.transitionMatrix = *(Mat_<float>(1, 1) << 1);  //转移矩阵F
	 setIdentity(KF.measurementMatrix);//H
	 setIdentity(KF.processNoiseCov, Scalar::all(1e-9));//Q
	 setIdentity(KF.measurementNoiseCov, Scalar::all(1e-2));//R
	 setIdentity(KF.errorCovPost, Scalar::all(1));//P
	 randn(KF.statePost, Scalar::all(0), Scalar::all(0.1));//Z

}
void kalmanfirst(GYRO_MPU*mpu)
{
	static int onece=1;
	if(onece)
		{
			 setIdentity(KF.statePost, Scalar::all(mpu->gyro_gz));
			 setIdentity(KF.statePre, Scalar::all(mpu->gyro_gz));
			 
			 onece=0;
		}
	


}

void kalmanfilter(GYRO_MPU*mpu)
{
	if(KALMANFILTER)
		{
			kalmanfirst(mpu);
			setIdentity(Zmeasurement, Scalar::all(mpu->gyro_gz));//Q
			
			KF.correct(Zmeasurement);
			Mat prediction = KF.predict();

			double predictZ = prediction.at<float>(0);

			mpu->gyro_gz=predictZ;
			//printf("predictZ=%f Zmeasurement=%f,KF.statePost=%f\n",predictZ,Zmeasurement.at<float>(0),KF.statePost);
			// randn( Zmeasurement, Scalar::all(0), Scalar::all(KF.measurementNoiseCov.at<float>(0)));
			// generate measurement
			// Zmeasurement += KF.measurementMatrix*Xstate;


			//randn( QprocessNoise, Scalar(0), Scalar::all(sqrt(KF.processNoiseCov.at<float>(0, 0))));
			//Xstate = KF.transitionMatrix*Xstate + QprocessNoise;


		}


}
void GetMpugyro(GYRO_MPU*mpu)
{
	double pi=3.1415926;
	if(BUTTERFLY)
		MPU_Get_Gyroint(&mpu->gyro_gxi,&mpu->gyro_gyi,&mpu->gyro_gzi,&mpu->gyro_gx,&mpu->gyro_gy,&mpu->gyro_gz);
	else
		MPU_Get_Gyro(&mpu->gyro_gxi,&mpu->gyro_gyi,&mpu->gyro_gzi,&mpu->gyro_gx,&mpu->gyro_gy,&mpu->gyro_gz);
	
	MPU_Get_Accel(&mpu->gyro_axi,&mpu->gyro_ayi,&mpu->gyro_azi,&mpu->gyro_ax,&mpu->gyro_ay,&mpu->gyro_az);
	MPU_Get_Mag(&mpu->gyro_mxi,&mpu->gyro_myi,&mpu->gyro_mzi,&mpu->gyro_mx,&mpu->gyro_my,&mpu->gyro_mz);

	if(getpanoflagenable())
		kalmanfilter(mpu);
		//AHRSupdate(gx,gy,gz,ax,ay,az,mx,my,mz,&roll,&pitch,&yaw);
	//OSA_printf("gx=%f gy=%f gz=%f\n",mpu->gyro_gx,mpu->gyro_gy,mpu->gyro_gz);
	getQunterandEuler(mpu);
	//OSA_printf("roll=%f pitch=%f yaw=%f  offsetx=%d offsety=%d offsetz=%d\n",mpu->roll,mpu->pitch,mpu->yaw,gyro_offsetx,gyro_offsety,gyro_offsetz);
	//OSA_printf("angle roll=%f angle pitch=%f angle yaw=%f\n",mpu->gyro_gx*180/pi,mpu->gyro_gy*180/pi,mpu->gyro_gz*180/pi);
}

