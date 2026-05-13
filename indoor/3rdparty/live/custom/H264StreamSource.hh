#ifndef _H264_STREAM_SOURCE_HH_
#define _H264_STREAM_SOURCE_HH_

#include "FramedFileSource.hh"

class H264StreamSource : public FramedSource
{
public:
    static H264StreamSource *createNew(UsageEnvironment &env);
    unsigned int maxFrameSize() const;

protected:
    H264StreamSource(UsageEnvironment &env);
    ~H264StreamSource();

private:
    // 重定义虚函数
    virtual void doGetNextFrame();
    virtual void doStopGettingFrames();
};

#endif // _H264_STREAM_SOURCE_HH_
