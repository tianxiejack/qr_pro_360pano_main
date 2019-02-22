/*
 * main.h
 *
 *  Created on: 2018年8月23日
 *      Author: fsmdn121
 */

#ifndef MAIN_H
#define MAIN_H

#define PROCESS_FRAME processFrame_pano
#define M_MAIN main_pano

int main_cap(int argc, char **argv);
int main_gst(int argc, char **argv);
int main_osd(int argc, char **argv);
int main_process(int argc, char **argv);
int main_pano(int argc, char **argv);

void processFrame_cap(int cap_chid,unsigned char *src, struct v4l2_buffer capInfo, int format);
void processFrame_gst(int cap_chid,unsigned char *src, struct v4l2_buffer capInfo, int format);
void processFrame_osd(int cap_chid,unsigned char *src, struct v4l2_buffer capInfo, int format);
void processFrame_process(int cap_chid,unsigned char *src, struct v4l2_buffer capInfo, int format);
void processFrame_pano(int cap_chid,unsigned char *src, struct v4l2_buffer capInfo, int format);


#endif /* MAIN_H_ */
