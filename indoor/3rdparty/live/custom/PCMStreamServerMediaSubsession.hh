#ifndef _PCM_STREAM_SERVER_MEDIA_SUBSESSION_HH
#define _PCM_STREAM_SERVER_MEDIA_SUBSESSION_HH

#include "OnDemandServerMediaSubsession.hh"

class PCMStreamServerMediaSubsession : public OnDemandServerMediaSubsession
{
public:
  static PCMStreamServerMediaSubsession *createNew(UsageEnvironment &env, Boolean reuseFirstSource);

protected:
  PCMStreamServerMediaSubsession(UsageEnvironment &env, Boolean reuseFirstSource);
  ~PCMStreamServerMediaSubsession();

protected:
  // 重定义虚函数
  FramedSource *createNewStreamSource(unsigned clientSessionId, unsigned &estBitrate);
  RTPSink *createNewRTPSink(Groupsock *rtpGroupsock, unsigned char rtpPayloadTypeIfDynamic, FramedSource *inputSource);
};

#endif
