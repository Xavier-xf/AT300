#include "H264StreamServerMediaSubsession.hh"
#include "H264StreamSource.hh"
#include "H264VideoStreamFramer.hh"
#include "H264VideoRTPSink.hh"
#include "H264VideoStreamDiscreteFramer.hh"

H264StreamServerMediaSubsession *H264StreamServerMediaSubsession::createNew(UsageEnvironment &env, Boolean reuseFirstSource)
{
	return new H264StreamServerMediaSubsession(env, reuseFirstSource);
}

H264StreamServerMediaSubsession::H264StreamServerMediaSubsession(UsageEnvironment &env, Boolean reuseFirstSource)
	: OnDemandServerMediaSubsession(env, reuseFirstSource)
{
	printf("[%s:%d]====================\n", __func__, __LINE__);
}

H264StreamServerMediaSubsession::~H264StreamServerMediaSubsession()
{
	printf("[%s:%d]====================\n", __func__, __LINE__);
}

FramedSource *H264StreamServerMediaSubsession::createNewStreamSource(unsigned clientSessionId, unsigned &estBitrate)
{
	// 创建视频源,参照H264VideoFileServerMediaSubsession
	H264StreamSource *liveSource = H264StreamSource::createNew(envir());
	if (liveSource == NULL)
	{
		return NULL;
	}
	// Create a framer for the Video Elementary Stream:
	return H264VideoStreamFramer::createNew(envir(), liveSource);
	// printf("[%s:%d]====================\n", __func__, __LINE__);
	// return H264VideoStreamDiscreteFramer::createNew(envir(), liveSource);
}

RTPSink *H264StreamServerMediaSubsession ::createNewRTPSink(Groupsock *rtpGroupsock,
															unsigned char rtpPayloadTypeIfDynamic,
															FramedSource * /*inputSource*/)
{
	return H264VideoRTPSink::createNew(envir(), rtpGroupsock, rtpPayloadTypeIfDynamic);
}
