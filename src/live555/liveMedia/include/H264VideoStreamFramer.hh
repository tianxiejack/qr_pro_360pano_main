/**********
This library is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the
Free Software Foundation; either version 3 of the License, or (at your
option) any later version. (See <http://www.gnu.org/copyleft/lesser.html>.)

This library is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
more details.

You should have received a copy of the GNU Lesser General Public License
along with this library; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
**********/
// "liveMedia"
// Copyright (c) 1996-2018 Live Networks, Inc.  All rights reserved.
// A filter that breaks up a H.264 Video Elementary Stream into NAL units.
// C++ header

#ifndef _H264_VIDEO_STREAM_FRAMER_HH
#define _H264_VIDEO_STREAM_FRAMER_HH

#ifndef _H264_OR_5_VIDEO_STREAM_FRAMER_HH
#include "H264or5VideoStreamFramer.hh"
#endif

#include <queue>
#include <iostream>
#include <assert.h>
#include "liveMedia.hh"
#include <OnDemandServerMediaSubsession.hh>

#define ENABLE_SPLITE_I_FRAME (1)

using namespace std;

typedef struct
{
    /* Size of payload (including any padding) in bytes. */
    int     i_payload;
    /* If param->b_annexb is set, Annex-B bytestream with startcode.
     * Otherwise, startcode is replaced with a 4-byte size.
     * This size is the size used in mp4/similar muxing; it is equal to i_payload-4 */
    uint8_t *p_payload;

} x264_nal_t;

class H264VideoStreamFramer: public H264or5VideoStreamFramer {
public:
  static H264VideoStreamFramer* createNew(UsageEnvironment& env, FramedSource* inputSource,
					  Boolean includeStartCodeInOutput = False);

protected:
  H264VideoStreamFramer(UsageEnvironment& env, FramedSource* inputSource,
			Boolean createParser, Boolean includeStartCodeInOutput);
      // called only by "createNew()"
  virtual ~H264VideoStreamFramer();

  // redefined virtual functions:
  virtual Boolean isH264VideoStreamFramer() const;
};


class x264Encoder
{
public:
    x264Encoder(void);
    ~x264Encoder(void);

public:
    void initilize();
    void unInitilize();
    void encodeFrame();
    bool isNalsAvailableInOutputQueue();
    x264_nal_t getNalUnit();
private:
    std::queue<x264_nal_t> outputQueue;

};



class H264LiveServerMediaSession:public OnDemandServerMediaSubsession
{
public:
    static H264LiveServerMediaSession* createNew(UsageEnvironment& env, bool reuseFirstSource);
    void checkForAuxSDPLine1();
    void afterPlayingDummy1();
protected:
    H264LiveServerMediaSession(UsageEnvironment& env, bool reuseFirstSource);
    virtual ~H264LiveServerMediaSession(void);
    void setDoneFlag() { fDoneFlag = ~0; }
protected:
    virtual FramedSource* createNewStreamSource(unsigned clientSessionId, unsigned& estBitrate);
    virtual RTPSink* createNewRTPSink(Groupsock* rtpGroupsock, unsigned char rtpPayloadTypeIfDynamic, FramedSource* inputSource);
private:
    char* fAuxSDPLine;
    char fDoneFlag;
    RTPSink* fDummySink;
protected:
    //virtual char const* sdpLines();
#if ENABLE_SPLITE_I_FRAME
    virtual char const* getAuxSDPLine(RTPSink* rtpSink, FramedSource* inputSource);
#endif
};


class LiveSourceWithx264:public FramedSource
{
public:
    static LiveSourceWithx264* createNew(UsageEnvironment& env);
    static EventTriggerId eventTriggerId;
protected:
    LiveSourceWithx264(UsageEnvironment& env);
    virtual ~LiveSourceWithx264(void);
private:
    virtual void doGetNextFrame();
    static void deliverFrame0(void* clientData);
    void deliverFrame();
    void encodeNewFrame();
    static unsigned referenceCount;
    std::queue<x264_nal_t> nalQueue;
    timeval currentTime;

      bool bDetectPPS ;
      bool bDetectIDR ;
      int get_ring_buf_count;
    // videoCaptureDevice is my BGR data source. You can have according to your need

    // Remember the x264 encoder wrapper we wrote in the start
    x264Encoder *encoder;
};







#endif


