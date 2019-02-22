#include "liveMedia.hh"
#include "BasicUsageEnvironment.hh"
#include "UsageEnvironment.hh"
#include <iostream>
#include <stdio.h>
#include"rtspmediaserver.h"
#define USE_NET_RECEIVE		(0)			//1:网络接收；0:本地接收
#define ACCESS_CONTROL		(0)			//1:使用用户密码验证，用户名admin，密码admin12345；0:不使用用户密码验证
#define USE_STREAM_NAME	(1)			//1:使用字节流的名字；0:不使用字节流的名字


#include <gst_ring_buffer.h>

using namespace std;
UsageEnvironment* env;

// To make the second and subsequent client for each stream reuse the same
// input stream as the first client (rather than playing the file from the
// start for each client), change the following "False" to "True":
Boolean reuseFirstSource = False;

static void announceStream(RTSPServer* rtspServer, ServerMediaSession* sms,
			   char const* streamName); // fwd


int rtspmediaserver()
{

	printf(" Build Timestamp: date %s %s\r\n", __DATE__, __TIME__);

	create_ring_buffer();

  // Begin by setting up our usage environment:
  TaskScheduler* scheduler = BasicTaskScheduler::createNew();
  env = BasicUsageEnvironment::createNew(*scheduler);

  UserAuthenticationDatabase* authDB = NULL;

#if ACCESS_CONTROL
  // To implement client access control to the RTSP server, do the following:
  authDB = new UserAuthenticationDatabase;
  /* authDB->addUserRecord("username1", "password1"); // replace these with real strings */
  authDB->addUserRecord("admin", "admin12345");
  // Repeat the above with each <username>, <password> that you wish to allow
  // access to the server.
#endif

  // Create the RTSP server.  Try first with the default port number (554),
    // and then with the alternative port number (8554):
    RTSPServer* rtspServer;
    portNumBits rtspServerPortNum = 8554;
    int streamIndex=0;
    rtspServerPortNum += streamIndex;
    rtspServer = RTSPServer::createNew(*env, rtspServerPortNum, authDB);
    if (rtspServer == NULL) {
      rtspServer = RTSPServer::createNew(*env, rtspServerPortNum, authDB);
    }
    if (rtspServer == NULL) {
      *env << "Failed to create RTSP server: " << env->getResultMsg() << "\n";
      exit(1);
    }

  char const* descriptionString
    = "Session streamed by \"testOnDemandRTSPServer\"";

  // Set up each of the possible streams that can be served by the
  // RTSP server.  Each such stream is implemented using a
  // "ServerMediaSession" object, plus one or more
  // "ServerMediaSubsession" objects for each audio/video substream.


  	  OutPacketBuffer::maxSize = 600000;
#if USE_STREAM_NAME
      std::string streamName = "live";
      char strTmp[8] = "";
      sprintf(strTmp, "%d", streamIndex+1);
      streamName += strTmp;
#else
  	  std::string streamName = "";
#endif
      ServerMediaSession* sms = ServerMediaSession::createNew(*env, streamName.c_str(), streamName.c_str(), "Live H264 Stream");
      H264LiveServerMediaSession *liveSubSession = H264LiveServerMediaSession::createNew(*env, true);
      sms->addSubsession(liveSubSession);
      rtspServer->addServerMediaSession(sms);

      announceStream(rtspServer, sms, streamName.c_str());


  // Also, attempt to create a HTTP server for RTSP-over-HTTP tunneling.
  // Try first with the default HTTP port (80), and then with the alternative HTTP
  // port numbers (8000 and 8080).
  if (rtspServer->setUpTunnelingOverHTTP(80) || rtspServer->setUpTunnelingOverHTTP(8000) || rtspServer->setUpTunnelingOverHTTP(8080)) {
    *env << "\n(We use port " << rtspServer->httpServerPortNum() << " for optional RTSP-over-HTTP tunneling.)\n";
  } else {
    *env << "\n(RTSP-over-HTTP tunneling is not available.)\n";
  }

  env->taskScheduler().doEventLoop(); // does not return

#if USE_NET_RECEIVE
  bitsInterfacesDestroy();
#endif

  return 0; // only to prevent compiler warning
}

static void announceStream(RTSPServer* rtspServer, ServerMediaSession* sms,
			   char const* streamName) {
  char* url = rtspServer->rtspURL(sms);
  UsageEnvironment& env = rtspServer->envir();
  env << "Play using the URL \"" << url << "\"\n";
  delete[] url;
}
