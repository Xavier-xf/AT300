#include "liveMedia.hh"
#include "BasicUsageEnvironment.hh"
#include "H264StreamSource.hh"
#include "PCMStreamSource.hh"

UsageEnvironment *env;

struct sessionState_t
{
  FramedSource *video_source;
  FramedSource *audio_source;
  RTPSink *video_sink;
  RTPSink *audio_sink;
  RTCPInstance *video_rtcpInstance;
  RTCPInstance *audio_rtcpInstance;
  Groupsock *rtpGroupsock;
  Groupsock *rtcpGroupsock;
} sessionState;

void videoplay(); // forward
void audioplay();

extern "C" int multicast_streamer(void)
{
  TaskScheduler *scheduler = BasicTaskScheduler::createNew();
  env = BasicUsageEnvironment::createNew(*scheduler);

  char const *destinationAddressStr = "239.255.42.42";
  const unsigned short rtpPortNum = 6666;
  const unsigned short rtcpPortNum = rtpPortNum + 1;
  const unsigned char ttl = 255; // low, in case routers don't admin scope

  NetAddressList destinationAddresses(destinationAddressStr);
  struct sockaddr_storage destinationAddress;
  copyAddress(destinationAddress, destinationAddresses.firstAddress());

  const Port rtpPort(rtpPortNum);
  const Port rtcpPort(rtcpPortNum);

  sessionState.rtpGroupsock = new Groupsock(*env, destinationAddress, rtpPort, ttl);
  sessionState.rtcpGroupsock = new Groupsock(*env, destinationAddress, rtcpPort, ttl);

  OutPacketBuffer::maxSize = 4 * 1024 * 1024;
  sessionState.video_sink = H264VideoRTPSink::createNew(*env, sessionState.rtpGroupsock, 96);
  const unsigned estimatedSessionBandwidth = 4 * 1024; // in kbps; for RTCP b/w share
  const unsigned maxCNAMElen = 100;
  unsigned char CNAME[maxCNAMElen + 1];
  gethostname((char *)CNAME, maxCNAMElen);
  CNAME[maxCNAMElen] = '\0'; // just in case
  sessionState.video_rtcpInstance = RTCPInstance::createNew(*env, sessionState.rtcpGroupsock,
                                                            estimatedSessionBandwidth, CNAME,
                                                            sessionState.video_sink, NULL /* we're a server */,
                                                            False);

  sessionState.audio_sink = SimpleRTPSink::createNew(*env, sessionState.rtpGroupsock, 97, 16000, "audio", "PCM", 1, False, False);
  const unsigned estimatedSessionBandwidth1 = 256; // in kbps; for RTCP b/w share
  gethostname((char *)CNAME, maxCNAMElen);
  CNAME[maxCNAMElen] = '\0'; // just in case
  sessionState.audio_rtcpInstance = RTCPInstance::createNew(*env, sessionState.rtcpGroupsock,
                                                            estimatedSessionBandwidth1, CNAME,
                                                            sessionState.audio_sink, NULL /* we're a server */,
                                                            False);
  videoplay();
  audioplay();

  env->taskScheduler().doEventLoop(); // does not return
  return 0;                           // only to prevent compiler warning
}

void videoafterPlaying(void *clientData); // forward

void videoplay()
{
  H264StreamSource *fileSource = H264StreamSource::createNew(*env);
  if (fileSource == NULL)
  {
    *env << "Unable to open as a H264 video source\n";
    exit(1);
  }
  sessionState.video_source = H264VideoStreamFramer::createNew(*env, fileSource);
  *env << "Beginning video streaming...\n";
  sessionState.video_sink->startPlaying(*sessionState.video_source, videoafterPlaying, NULL);
}

void videoafterPlaying(void * /*clientData*/)
{
  *env << "...done video streaming\n";
  sessionState.video_sink->stopPlaying();
  Medium::close(sessionState.video_source);
  videoplay();
}

void audioafterPlaying(void *clientData); // forward

void audioplay()
{
  sessionState.audio_source = PCMStreamSource::createNew(*env, 16, 1, 16000);
  if (sessionState.audio_source == NULL)
  {
    *env << "Unable to open as a pcm audio source\n";
    exit(1);
  }
  *env << "Beginning audio streaming...\n";
  sessionState.audio_sink->startPlaying(*sessionState.audio_source, audioafterPlaying, NULL);
}

void audioafterPlaying(void * /*clientData*/)
{
  *env << "...done audio streaming\n";
  sessionState.audio_sink->stopPlaying();
  Medium::close(sessionState.audio_source);
  exit(0);
}
