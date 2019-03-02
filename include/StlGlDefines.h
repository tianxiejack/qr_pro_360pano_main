/*
 * StlGlDefines.h
 *
 *  Created on: Nov 3, 2016
 *      Author: hoover
 */

#ifndef STLGLDEFINES_H_
#define STLGLDEFINES_H_

#define DEFAULT_IMAGE_WIDTH 1920
#define DEFAULT_IMAGE_HEIGHT 1080
#define DEFAULT_IMAGE_DEPTH 2

#define HOT_WIDTH 720//1280
#define HOT_HEIGHT 288//1024
#define HOT_CC 1
#define TV_WIDTH 1920
#define TV_HEIGHT 1080
#define TV_CC 2

#define MAX_QP		51
#define MIN_QP_2M	41
#define MIN_QP_4M	38
#define MIN_QP_8M	34

#define MAX_I		51
#define MIN_I_2M	41
#define MIN_I_4M	38
#define MIN_I_8M	34

#define BITRATE_2M 1400000
#define BITRATE_4M 2800000
#define BITRATE_8M 5600000

#define GST_ENCBITRATE_2M	(1400000)
#define GST_ENCBITRATE_4M	(2800000)
#define GST_ENCBITRATE_8M	(5600000)

enum{
	TV_DEV_ID=1,
	HOT_DEV_ID,
	QUE_CHID_COUNT
};
enum CCT_COUNT {
		T_180_T,
		T_360_T,
		RADAR_T,
		CCT_COUNT
		};
#define T180_TGA "180.tga"
#define T360_TGA			"360.tga"
#define RADAR_TGA "radar.tga"


#endif /* STLGLDEFINES_H_ */
