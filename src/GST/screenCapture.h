#ifndef _SCREENCAPTURE_H
#define _SCREENCAPTURE_H

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
