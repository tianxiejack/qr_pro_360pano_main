#ifndef _BITSINTERFACESAPI_H_
#define _BITSINTERFACESAPI_H_

typedef enum
{
    VCODEC_FRAME_TYPE_I_FRAME,
    /**< I-frame or Key Frame */

    VCODEC_FRAME_TYPE_P_FRAME,
    /**< P-frame */

    VCODEC_FRAME_TYPE_B_FRAME,
    /**< B-frame */

} VCODEC_FRAME_TYPE_E;

typedef enum
{
    VCODEC_TYPE_H264,
    /**< H264 */
    VCODEC_TYPE_MPEG4,
    /**< MPEG4 */
    VCODEC_TYPE_MJPEG,
    /**< MJPEG */
    VCODEC_TYPE_MPEG2,
    /**< MPEG2 */
    VCODEC_TYPE_H265,
    /**< H265 */
    VCODEC_TYPE_MAX,
    /**< Max Codec Type */

} VCODEC_TYPE_E;

typedef struct
{
	unsigned int flag;	// 0xFFFF0001
	unsigned int size;
	unsigned int chnId;	// no use in current version
	unsigned int strmId;
	VCODEC_TYPE_E codecType;
	VCODEC_FRAME_TYPE_E frameType;
	unsigned int timestamp;
	unsigned int reserved;
} DATAHEADER;

void demoInterfacesDestroy(void);
int demoInterfacesPut(DATAHEADER *pPrm, char* pData);
void demoInterfacesCreate(char *ip_addr, Uint32 port);
#endif
