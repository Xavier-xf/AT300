#ifndef _PCM_STREAM_SOURCE_HH_
#define _PCM_STREAM_SOURCE_HH_

#include "FramedFileSource.hh"

class PCMStreamSource : public FramedSource
{
public:
  static PCMStreamSource *createNew(UsageEnvironment &env, unsigned char sample_bit = 16, unsigned char channel = 1, unsigned int sample_rate = 16000);

protected:
  PCMStreamSource(UsageEnvironment &env, unsigned char sample_bit, unsigned char channel, unsigned int sample_rate);
  ~PCMStreamSource();

private:
  // 重定义虚函数
  virtual void doGetNextFrame();
};

#endif // _PCM_STREAM_SOURCE_HH_
