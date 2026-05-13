#include "PCMStreamServerMediaSubsession.hh"
#include "PCMStreamSource.hh"
#include "uLawAudioFilter.hh"
#include "SimpleRTPSink.hh"

PCMStreamServerMediaSubsession *PCMStreamServerMediaSubsession::createNew(UsageEnvironment &env, Boolean reuseFirstSource)
{
    return new PCMStreamServerMediaSubsession(env, reuseFirstSource);
}


PCMStreamServerMediaSubsession::PCMStreamServerMediaSubsession(UsageEnvironment& env,Boolean reuseFirstSource): OnDemandServerMediaSubsession(env,reuseFirstSource)
{

}


PCMStreamServerMediaSubsession::~PCMStreamServerMediaSubsession()
{

}


FramedSource* PCMStreamServerMediaSubsession::createNewStreamSource(unsigned clientSessionId, unsigned& estBitrate)
{
	//创建视频源,参照H264VideoFileServerMediaSubsession
	PCMStreamSource* liveSource = PCMStreamSource::createNew(envir());
	if (liveSource == NULL)
	{
		return NULL;

	}
	// Create a framer for the Video Elementary Stream:
	return uLawFromPCMAudioSource::createNew(envir(), liveSource, 1/*little-endian*/);
}


RTPSink* PCMStreamServerMediaSubsession::createNewRTPSink(Groupsock* rtpGroupsock,unsigned char rtpPayloadTypeIfDynamic,FramedSource* /*inputSource*/) 
{
	/*PCMU payloadFormatCode 0*/
	/*rtpPayloadTypeIfDynamic:https://www.ietf.org/assignments/rtp-parameters/rtp-parameters.xml#rtp-parameters-1*/

    return SimpleRTPSink::createNew(envir(), rtpGroupsock, rtpPayloadTypeIfDynamic, 8000, "audio", "PCMU", 1, False, False); 
} 