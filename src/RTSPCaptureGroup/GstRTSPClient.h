/*
 * GstRTSPClient.h
 *
 *  Created on: 2018年5月21日
 *      Author: lw
 */

#ifndef	_GSTRTSPCLIENT_H_
#define	_GSTRTSPCLIENT_H_

#include <gst/gst.h>
#include <gst/app/gstappsrc.h>
#include <gst/app/gstappsink.h>
#include <gst/gstclock.h>
#include <gst/gstelement.h>
#include <gst/gstparse.h>
#include <gst/gstpad.h>
#include <stdio.h>
#include "InterfaceProcessImageBehavior.h"

typedef struct _CustomData {
	GstElement *pipeline, *source, *rtph264depay, *h264parse, *avdec_h264, *videoconvert, *xvimagesink;

	GstBus *bus;
	GMainLoop *loop;

	GstStateChangeReturn ret;

} CustomData;

class GstRTSPClient {

public:
	GstRTSPClient();
	virtual ~GstRTSPClient();

	void SetInterfaceProcess(InterfaceProcessImageBehavior* process){ this->pProcessImageBehavior = process; }  /*设置新的接口方法*/

	int  InitMain();

	int Play();
	int Pause();
	int Ready();
	void SetSourceLocation(const char * source_location);

	InterfaceProcessImageBehavior * getInterfaceProcessImageBehavior() { return pProcessImageBehavior; }
	CustomData *getCustomData() {return &customData; };
private:
	CustomData customData;
	InterfaceProcessImageBehavior *  pProcessImageBehavior;
	pthread_t a_thread;

	void createThread();
};

#endif /* GSTRTSPCLIENT_H_ */
