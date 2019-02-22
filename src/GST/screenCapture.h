#ifndef _CABIN_CAPTURE_H
#define _CABIN_CAPTURE_H

#ifdef __cplusplus
extern "C"
{
#endif

/*************************/
int gst_videnc_create(void);
//int gst_videnc_destory(void);
int gst_videnc_bitrate(int mode);
int gst_videnc_record(int enable);



#ifdef __cplusplus
}
#endif



#endif
