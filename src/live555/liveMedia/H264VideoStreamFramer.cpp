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
// Implementation

#include "H264VideoStreamFramer.hh"
#include <gst_ring_buffer.h>
#include <demo_global_def.h>

H264VideoStreamFramer* H264VideoStreamFramer
::createNew(UsageEnvironment& env, FramedSource* inputSource, Boolean includeStartCodeInOutput) {
  return new H264VideoStreamFramer(env, inputSource, True, includeStartCodeInOutput);
}

H264VideoStreamFramer
::H264VideoStreamFramer(UsageEnvironment& env, FramedSource* inputSource, Boolean createParser, Boolean includeStartCodeInOutput)
  : H264or5VideoStreamFramer(264, env, inputSource, createParser, includeStartCodeInOutput) {
}

H264VideoStreamFramer::~H264VideoStreamFramer() {

}

Boolean H264VideoStreamFramer::isH264VideoStreamFramer() const {
  return True;
}

x264Encoder::x264Encoder(void)
{

}


x264Encoder::~x264Encoder(void)
{

}

void x264Encoder::initilize()
{

}

void x264Encoder::unInitilize()
{

}

void x264Encoder::encodeFrame()
{
    x264_nal_t* nals ;
    int i_nals = 0;
    int frameSize = -1;

     //outputQueue.push(nals[i]);

}

bool x264Encoder::isNalsAvailableInOutputQueue()
{
    if(outputQueue.empty() == true)
    {
        return false;
    }
    else
    {
        return true;
    }
}

x264_nal_t x264Encoder::getNalUnit()
{
    x264_nal_t nal;
    nal = outputQueue.front();
    outputQueue.pop();
    return nal;
}



H264LiveServerMediaSession* H264LiveServerMediaSession::createNew(UsageEnvironment& env, bool reuseFirstSource)
{
    return new H264LiveServerMediaSession(env, reuseFirstSource);
}

H264LiveServerMediaSession::H264LiveServerMediaSession(UsageEnvironment& env, bool reuseFirstSource):OnDemandServerMediaSubsession(env,reuseFirstSource),fAuxSDPLine(NULL), fDoneFlag(0), fDummySink(NULL)
{

}


H264LiveServerMediaSession::~H264LiveServerMediaSession(void)
{
    delete[] fAuxSDPLine;
}


static void afterPlayingDummy(void* clientData)
{
    H264LiveServerMediaSession *session = (H264LiveServerMediaSession*)clientData;
    session->afterPlayingDummy1();
}

void H264LiveServerMediaSession::afterPlayingDummy1()
{
    envir().taskScheduler().unscheduleDelayedTask(nextTask());
    setDoneFlag();
}

static void checkForAuxSDPLine(void* clientData)
{
    H264LiveServerMediaSession* session = (H264LiveServerMediaSession*)clientData;
    session->checkForAuxSDPLine1();
}

void H264LiveServerMediaSession::checkForAuxSDPLine1()
{
    char const* dasl;
    if(fAuxSDPLine != NULL)
    {
        setDoneFlag();
    }
    else if(fDummySink != NULL && (dasl = fDummySink->auxSDPLine()) != NULL)
    {
        fAuxSDPLine = strDup(dasl);
        fDummySink = NULL;
        setDoneFlag();
    }
    else
    {
        int uSecsDelay = 10000;
        nextTask() = envir().taskScheduler().scheduleDelayedTask(uSecsDelay, (TaskFunc*)checkForAuxSDPLine, this);
    }
}


FramedSource* H264LiveServerMediaSession::createNewStreamSource(unsigned clientSessionID, unsigned& estBitRate)
{
    // Based on encoder configuration i kept it 90000
    estBitRate = 4000000;
    LiveSourceWithx264 *source = LiveSourceWithx264::createNew(envir());
    // are you trying to keep the reference of the source somewhere? you shouldn't.
    // Live555 will create and delete this class object many times. if you store it somewhere
    // you will get memory access violation. instead you should configure you source to always read from your data source
    return H264VideoStreamDiscreteFramer::createNew(envir(),source);
}

RTPSink* H264LiveServerMediaSession::createNewRTPSink(Groupsock* rtpGroupsock, unsigned char rtpPayloadTypeIfDynamic, FramedSource* inputSource)
{
    return H264VideoRTPSink::createNew(envir(), rtpGroupsock, rtpPayloadTypeIfDynamic);
}

/*
char const* H264LiveServerMediaSession::sdpLines()
{
   return fSDPLines = 
    "m=video 0 RTP/AVP 96\r\n"
    "c=IN IP4 0.0.0.0\r\n"
    "b=AS:4000000\r\n"
    "a=rtpmap:96 H264/90000\r\n"
   // "a=fmtp:96 packetization-mode=1;profile-level-id=000000;sprop-parameter-sets=H264\r\n"
	"a=fmtp:96 packetization-mode=1;profile-level-id=64001E;sprop-parameter-sets=H264\r\n"
    "a=control:track1\r\n";
}
*/

#if ENABLE_SPLITE_I_FRAME

char const* H264LiveServerMediaSession::getAuxSDPLine(RTPSink* rtpSink, FramedSource* inputSource)
{
    if(fAuxSDPLine != NULL) return fAuxSDPLine;
    if(fDummySink == NULL)
    {
        fDummySink = rtpSink;
        fDummySink->startPlaying(*inputSource, afterPlayingDummy, this);
        checkForAuxSDPLine(this);
    }

    envir().taskScheduler().doEventLoop(&fDoneFlag);
    return fAuxSDPLine;
}

#endif



LiveSourceWithx264* LiveSourceWithx264::createNew(UsageEnvironment& env)
{
    return new LiveSourceWithx264(env);
}

EventTriggerId LiveSourceWithx264::eventTriggerId = 0;

unsigned LiveSourceWithx264::referenceCount = 0;

LiveSourceWithx264::LiveSourceWithx264(UsageEnvironment& env):FramedSource(env)
{
    if(referenceCount == 0)
    {

    }
    ++referenceCount;

    encoder = new x264Encoder();
    encoder->initilize();
    bDetectPPS = false;
    bDetectIDR = false;
    get_ring_buf_count=0;

    if(eventTriggerId == 0)
    {
        eventTriggerId = envir().taskScheduler().createEventTrigger(deliverFrame0);
    }
}


LiveSourceWithx264::~LiveSourceWithx264(void)
{
    --referenceCount;

    encoder->unInitilize();
    envir().taskScheduler().deleteEventTrigger(eventTriggerId);
    eventTriggerId = 0;
}


void LiveSourceWithx264::encodeNewFrame()
{
        while( get_ring_buf()==SDK_EFAIL )  // ring_buffer为空
        {
        	//让出cpu
            usleep(100);
        }
        //printf("--------------------get_ring_buf_count++=%d\n",++get_ring_buf_count);
        //从ring_buf中取出一帧
        x264_nal_t nal ;
        nal.p_payload = (uint8_t*)get_ring_buf_ptr();
        nal.i_payload = get_ring_buf_length();
#if ENABLE_SPLITE_I_FRAME
        //把一帧放入queue
	   if ( nal.p_payload[0] == 0 && nal.p_payload[1] == 0 && nal.p_payload[2] == 0 && nal.p_payload[3] == 1 &&  (nal.p_payload[4] & 0x1F)==7  )
		{
				int i = 0;
				uint8_t* tmpBuf = nal.p_payload;
				 int pps_len=0;
				 int sps_len=0;
				 //printf("----------detect SPS;\n");
				for(tmpBuf+=5,i=5; i<nal.i_payload; i++, tmpBuf++ )
				{
					if ( tmpBuf[0] == 0 && tmpBuf[1] == 0 && tmpBuf[2] == 0 && tmpBuf[3] == 1 && (tmpBuf[4] & 0x1F)==8 )
					{
								x264_nal_t spsNal ;
								spsNal.p_payload = nal.p_payload;
								spsNal.i_payload = i;
								sps_len=i;
								nalQueue.push(spsNal);
								//printf(" receive buffer: nal.i_payload=%d,nal.p_payload=%p\n", nal.i_payload, nal.p_payload);
								assert(!bDetectPPS);
								bDetectPPS=true;
					}
					if( ( tmpBuf[0] == 0 && tmpBuf[1] == 0 && tmpBuf[2] == 0 && tmpBuf[3] == 1 && (tmpBuf[4] & 0x1F)==5)
							||( tmpBuf[0] == 0 && tmpBuf[1] == 0 && tmpBuf[2] == 1 && (tmpBuf[3] & 0x1F)==5 ) )
					{
							   assert(sps_len<=i);
								x264_nal_t ppsNal ;
								ppsNal.p_payload = nal.p_payload+sps_len;
								pps_len= i - sps_len;
								ppsNal.i_payload = pps_len;
								nalQueue.push(ppsNal);
								//printf("move%d byte----------detect IDR; Add ppsNal: ppsNal.i_payload=%d,ppsNal.p_payload=%p\n",  i, ppsNal.i_payload, ppsNal.p_payload);
								nal.p_payload = tmpBuf;
								nal.i_payload = nal.i_payload-i;
								assert(!bDetectIDR);
								bDetectIDR=true;
								//printf("bDetectIDR=true;\n");
								break;
					}
				}
		}
#endif

        nalQueue.push(nal);
}

void LiveSourceWithx264::deliverFrame0(void* clientData)
{
    ((LiveSourceWithx264*)clientData)->deliverFrame();
}

void LiveSourceWithx264::doGetNextFrame()
{
    timeval beginTime;
    timeval endTime;
    gettimeofday(&beginTime,NULL);
    if(nalQueue.empty() == true)
    {
        encodeNewFrame();
        //gettimeofday(&currentTime,NULL);
        deliverFrame();
    }
    else
    {
        deliverFrame();
    }
    if(!bDetectPPS && !bDetectIDR)
 	{
    	 //printf("--------------------get_ring_buf_count--=%d\n",--get_ring_buf_count);
     	get_ring_buf_release(); //回收ring_buffer内存
 	}
    gettimeofday(&endTime,NULL);
   // printf("during time=%fms\n", (endTime.tv_sec-beginTime.tv_sec)*1000 + (endTime.tv_usec-beginTime.tv_usec)/1000);
}

void LiveSourceWithx264::deliverFrame()
{
    if(!isCurrentlyAwaitingData()) return;
    x264_nal_t nal = nalQueue.front();
    nalQueue.pop();
    assert(nal.p_payload != NULL);
    // You need to remove the start code which is there in front of every nal unit.
    // the start code might be 0x00000001 or 0x000001. so detect it and remove it. pass remaining data to live555
    int trancate = 0;

    if ( nal.p_payload[0] == 0 && nal.p_payload[1] == 0 && nal.p_payload[2] == 0 && nal.p_payload[3] == 1 )
    {
    	trancate = 4;
    }
    else if( nal.p_payload[0] == 0 && nal.p_payload[1] == 0 && nal.p_payload[2] == 1  )
    {
    	trancate = 3;
    }

    if ( nal.p_payload[0] == 0 && nal.p_payload[1] == 0 && nal.p_payload[2] == 0 && nal.p_payload[3] == 1&& (nal.p_payload[4] & 0x1F)==8 )
    {
    	bDetectPPS=false;
    	//printf("bDetectPPS=false;\n");
    }
    if( ( nal.p_payload[0] == 0 && nal.p_payload[1] == 0 && nal.p_payload[2] == 1 && (nal.p_payload[3] & 0x1F)==5)||
    		 ( nal.p_payload[0] == 0 && nal.p_payload[1] == 0 && nal.p_payload[2] == 0 && nal.p_payload[3] ==1 && (nal.p_payload[4] & 0x1F)==5))
    {
    	bDetectIDR=false;
    	//printf("bDetectIDR=false;\n");
    }
    if(nal.i_payload-trancate > fMaxSize)
    {
        fFrameSize = fMaxSize;
        fNumTruncatedBytes = nal.i_payload-trancate - fMaxSize;
        assert(nal.i_payload-trancate > fMaxSize);
    }
    else
    {
        fFrameSize = nal.i_payload-trancate;
    }
    gettimeofday(&currentTime,NULL);
    fPresentationTime = currentTime;
    memmove(fTo,nal.p_payload+trancate,fFrameSize);
    FramedSource::afterGetting(this);
}



















