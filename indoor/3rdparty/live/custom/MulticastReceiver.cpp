
#include <iostream>
#include <thread>
#include <unistd.h>
#include "liveMedia.hh"
#include "GroupsockHelper.hh"
#include "BasicUsageEnvironment.hh"
#include "H264Sink.hh"
#include "PCMSink.hh"

void video_afterPlaying(void *clientData); // forward
void audio_afterPlaying(void *clientData); // forward
void audio_main();

// A structure to hold the state of the current session.
// It is used in the "afterPlaying()" function to clean up the session.
struct sessionState_t
{
    FramedSource *video_source;
    FramedSource *audio_source;
    H264Sink *video_sink;
    PCMSink *audio_sink;
    RTCPInstance *video_rtcpInstance;
    RTCPInstance *audio_rtcpInstance;
} sessionState;

UsageEnvironment *video_env;
UsageEnvironment *audio_env;

extern "C" int multicast_receiver(void)
{
    std::thread t([]
                  { audio_main(); });
    // Begin by setting up our usage environment:
    TaskScheduler *scheduler = BasicTaskScheduler::createNew();
    video_env = BasicUsageEnvironment::createNew(*scheduler);

    // Create the data sink for 'stdout':
    sessionState.video_sink = H264Sink::createNew(*video_env, "recv.264");
    // Note: The string "stdout" is handled as a special case.
    // A real file name could have been used instead.

    // Create 'groupsocks' for RTP and RTCP:
    char const *sessionAddressStr = "239.255.42.42";
    // Note: If the session is unicast rather than multicast,
    // then replace this string with "0.0.0.0"
    const unsigned short rtpPortNum = 6666;
    const unsigned short rtcpPortNum = rtpPortNum + 1;
    const unsigned char ttl = 255; // low, in case routers don't admin scope

    NetAddressList sessionAddresses(sessionAddressStr);
    struct sockaddr_storage sessionAddress;
    copyAddress(sessionAddress, sessionAddresses.firstAddress());

    const Port rtpPort(rtpPortNum);
    const Port rtcpPort(rtcpPortNum);

    Groupsock rtpGroupsock(*video_env, sessionAddress, rtpPort, ttl);
    Groupsock rtcpGroupsock(*video_env, sessionAddress, rtcpPort, ttl);

    RTPSource *rtpSource;
    rtpSource = H264VideoRTPSource::createNew(*video_env, &rtpGroupsock, 96);
    const unsigned estimatedSessionBandwidth = 500; // in kbps; for RTCP b/w share
    const unsigned maxCNAMElen = 100;
    unsigned char CNAME[maxCNAMElen + 1];
    gethostname((char *)CNAME, maxCNAMElen);
    CNAME[maxCNAMElen] = '\0'; // just in case
    sessionState.video_rtcpInstance = RTCPInstance::createNew(*video_env, &rtcpGroupsock,
                                                              estimatedSessionBandwidth, CNAME,
                                                              NULL /* we're a client */, rtpSource);
    sessionState.video_source = rtpSource;
    *video_env << "Beginning receiving multicast video stream...\n";
    sessionState.video_sink->startPlaying(*sessionState.video_source, video_afterPlaying, NULL);

    video_env->taskScheduler().doEventLoop(); // does not return

    return 0; // only to prevent compiler warning
}

void audio_main()
{
    // Begin by setting up our usage environment:
    TaskScheduler *scheduler = BasicTaskScheduler::createNew();
    audio_env = BasicUsageEnvironment::createNew(*scheduler);

    sessionState.audio_sink = PCMSink::createNew(*audio_env, "recv.pcm");

    char const *sessionAddressStr = "239.255.42.42";
    const unsigned short rtpPortNum = 6666;
    const unsigned short rtcpPortNum = rtpPortNum + 1;
    const unsigned char ttl = 255; // low, in case routers don't admin scope

    NetAddressList sessionAddresses(sessionAddressStr);
    struct sockaddr_storage sessionAddress;
    copyAddress(sessionAddress, sessionAddresses.firstAddress());

    const Port rtpPort(rtpPortNum);
    const Port rtcpPort(rtcpPortNum);

    Groupsock rtpGroupsock(*audio_env, sessionAddress, rtpPort, ttl);
    Groupsock rtcpGroupsock(*audio_env, sessionAddress, rtcpPort, ttl);

    RTPSource *rtpSource;
    rtpSource = SimpleRTPSource::createNew(*audio_env, &rtpGroupsock, 97, 16000, "audio/PCM", 0, False /*no 'M' bit*/);
    const unsigned estimatedSessionBandwidth = 256; // in kbps; for RTCP b/w share
    const unsigned maxCNAMElen = 100;
    unsigned char CNAME[maxCNAMElen + 1];
    gethostname((char *)CNAME, maxCNAMElen);
    CNAME[maxCNAMElen] = '\0'; // just in case
    sessionState.audio_rtcpInstance = RTCPInstance::createNew(*audio_env, &rtcpGroupsock,
                                                              estimatedSessionBandwidth, CNAME,
                                                              NULL /* we're a client */, rtpSource);
    sessionState.audio_source = rtpSource;
    *audio_env << "Beginning receiving multicast audio stream...\n";
    sessionState.audio_sink->startPlaying(*sessionState.audio_source, audio_afterPlaying, NULL);

    audio_env->taskScheduler().doEventLoop(); // does not return
}

void video_afterPlaying(void * /*clientData*/)
{
    *video_env << "...done receiving\n";

    // End by closing the media:
    Medium::close(sessionState.video_rtcpInstance);
    Medium::close(sessionState.video_sink);
    Medium::close(sessionState.video_source);
}
void audio_afterPlaying(void * /*clientData*/)
{
    *audio_env << "...done receiving\n";

    // End by closing the media:
    Medium::close(sessionState.audio_rtcpInstance);
    Medium::close(sessionState.audio_sink);
    Medium::close(sessionState.audio_source);
}
