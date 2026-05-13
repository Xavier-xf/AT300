#include "liveMedia.hh"

#include "BasicUsageEnvironment.hh"
#include "H264StreamServerMediaSubsession.hh"
#include "PCMStreamServerMediaSubsession.hh"
#include <GroupsockHelper.hh> // for "weHaveAnIPv*Address()"

#define ACCESS_CONTROL

UsageEnvironment *env;
Boolean reuseFirstSource = True;

static void announceStream(RTSPServer *rtspServer, ServerMediaSession *sms,
                           char const *streamName, char const *inputFileName); // forward

static void announceURL(RTSPServer *rtspServer, ServerMediaSession *sms);

extern "C" int rtsp_media_server_open(const char *username, const char *password)
{
  // Begin by setting up our usage environment:
  TaskScheduler *scheduler = BasicTaskScheduler::createNew();
  env = BasicUsageEnvironment::createNew(*scheduler);

  UserAuthenticationDatabase *authDB = NULL;
#ifdef ACCESS_CONTROL
  // To implement client access control to the RTSP server, do the following:
  authDB = new UserAuthenticationDatabase;
  authDB->addUserRecord(username, password); // replace these with real strings
  // Repeat the above with each <username>, <password> that you wish to allow
  // access to the server.
#endif

  // Create the RTSP server:
#ifdef SERVER_USE_TLS
  // Serve RTSPS: RTSP over a TLS connection:
  RTSPServer *rtspServer = RTSPServer::createNew(*env, 322, authDB);
#else
  // Serve regular RTSP (over a TCP connection):
  RTSPServer *rtspServer = RTSPServer::createNew(*env, 8554, authDB);
#endif
  if (rtspServer == NULL)
  {
    *env << "Failed to create RTSP server: " << env->getResultMsg() << "\n";
    return 0;
  }
#ifdef SERVER_USE_TLS
#ifndef STREAM_USING_SRTP
#define STREAM_USING_SRTP True
#endif
  rtspServer->setTLSState(PATHNAME_TO_CERTIFICATE_FILE, PATHNAME_TO_PRIVATE_KEY_FILE,
                          STREAM_USING_SRTP);
#endif

  char const *descriptionString = "Session streamed by \"testOnDemandRTSPServer\"";

  {
    OutPacketBuffer::maxSize = 512 * 1024;

    char const *streamName = "livemedia";
    char const *inputFileName = "live.264";
    ServerMediaSession *sms = ServerMediaSession::createNew(*env, streamName, streamName,
                                                            descriptionString);
    sms->addSubsession(H264StreamServerMediaSubsession ::createNew(*env, reuseFirstSource));

    sms->addSubsession(PCMStreamServerMediaSubsession ::createNew(*env, reuseFirstSource));

    rtspServer->addServerMediaSession(sms);

    announceStream(rtspServer, sms, streamName, inputFileName);
  }

#ifdef SERVER_USE_TLS
  // (Attempt to) use the default HTTPS port (443) instead:
  char const *httpProtocolStr = "HTTPS";
  if (rtspServer->setUpTunnelingOverHTTP(443))
  {
#else
  char const *httpProtocolStr = "HTTP";
  if (rtspServer->setUpTunnelingOverHTTP(80) || rtspServer->setUpTunnelingOverHTTP(8000) || rtspServer->setUpTunnelingOverHTTP(8080))
  {
#endif
    *env << "\n(We use port " << rtspServer->httpServerPortNum() << " for optional RTSP-over-" << httpProtocolStr << " tunneling.)\n";
  }
  else
  {
    *env << "\n(RTSP-over-" << httpProtocolStr << " tunneling is not available.)\n";
  }

  env->taskScheduler().doEventLoop(); // does not return

  return 0; // only to prevent compiler warning
}

static void announceStream(RTSPServer *rtspServer, ServerMediaSession *sms,
                           char const *streamName, char const *inputFileName)
{
  UsageEnvironment &env = rtspServer->envir();

  env << "\n\"" << streamName << "\" stream, from the file \""
      << inputFileName << "\"\n";
  announceURL(rtspServer, sms);
}

static void announceURL(RTSPServer *rtspServer, ServerMediaSession *sms)
{
  if (rtspServer == NULL || sms == NULL)
    return; // sanity check

  UsageEnvironment &env = rtspServer->envir();

  env << "Play this stream using the URL ";
  if (weHaveAnIPv4Address(env))
  {
    char *url = rtspServer->ipv4rtspURL(sms);
    env << "\"" << url << "\"";
    delete[] url;
    if (weHaveAnIPv6Address(env))
      env << " or ";
  }
  if (weHaveAnIPv6Address(env))
  {
    char *url = rtspServer->ipv6rtspURL(sms);
    env << "\"" << url << "\"";
    delete[] url;
  }
  env << "\n";
}