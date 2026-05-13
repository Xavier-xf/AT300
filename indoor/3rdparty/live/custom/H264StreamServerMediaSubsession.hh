#ifndef _H264_STREAM_SERVER_MEDIA_SUBSESSION_HH_
#define _H264_STREAM_SERVER_MEDIA_SUBSESSION_HH_

#include "OnDemandServerMediaSubsession.hh"

class H264StreamServerMediaSubsession : public OnDemandServerMediaSubsession
{

public:
        static H264StreamServerMediaSubsession *createNew(UsageEnvironment &env, Boolean reuseFirstSource);

protected:
        H264StreamServerMediaSubsession(UsageEnvironment &env, Boolean reuseFirstSource);
        ~H264StreamServerMediaSubsession();

protected: // redefined virtual functions
        FramedSource *createNewStreamSource(unsigned clientSessionId, unsigned &estBitrate);
        RTPSink *createNewRTPSink(Groupsock *rtpGroupsock,
                                  unsigned char rtpPayloadTypeIfDynamic,
                                  FramedSource *inputSource);

public:
};

#endif // _H264_STREAM_SERVER_MEDIA_SUBSESSION_HH_