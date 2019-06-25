#ifndef _CABIN_CAPTURE_H
#define _CABIN_CAPTURE_H
#define MAX_SIZE_TIME 5000000000
#if 1
#ifdef __cplusplus
#if 0
extern "C"
{
#endif
#endif
struct record;

extern char filename_bak[128];
extern char filename_gyro_bak[128];


typedef enum
{
	FLIP_METHOD_NONE,
	FLIP_METHOD_COUNTERCLOCKWISE,
	FLIP_METHOD_ROTATE_180,
	FLIP_METHOD_CLOCKWISE,
	FLIP_METHOD_HORIZONTAL_FLIP,
	FLIP_METHOD_UPPER_RIGHT_DIAGONAL,
	FLIP_METHOD_VERTICAL_FLIP,
	FLIP_METHOD_UPPER_LEFT_DIAGONAL,
	FLIP_METHOD_END
}FLIP_METHOD;


typedef enum
{
	XIMAGESRC,
	APPSRC
}CAPTURE_SRC;

typedef struct _recordHandle
{
	void* context;
	unsigned int width;
	unsigned int height;
	unsigned int framerate;
	unsigned int CHID;
	char format[30];
	unsigned short bEnable;
	FLIP_METHOD filp_method;
	unsigned int bitrate;
	unsigned int index;
	CAPTURE_SRC capture_src;
}RecordHandle;

typedef struct
{
	int year;
	int mon;
	int day;
	int hour;
	int min;
	int sec;
}date_t;


RecordHandle * CabinInit(int width, int height, int framerate, const char* format, int CHID, FLIP_METHOD filp_method);

int CabinPushData(RecordHandle *handle, char *pbuffer , int datasize);

int CabinUninit(RecordHandle *handle);

int CabinEnable(RecordHandle *handle, unsigned short bEnable);
void EOS(RecordHandle *recordHandle);
void SetTime(unsigned long Tnm,RecordHandle *recordHandle);
void set_video_cnt(int cnt);
int get_video_cnt();
void set_data_cnt(int cnt);
int get_data_cnt();

#if 0
#ifdef __cplusplus
}
#endif
#endif
#endif
#endif
