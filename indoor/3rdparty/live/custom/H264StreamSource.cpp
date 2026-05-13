#include "H264StreamSource.hh"
#include "InputFile.hh"
#include "GroupsockHelper.hh"

#ifdef __cplusplus
extern "C"
{
#endif

  extern bool video_input_read(unsigned char **data, int *size);

#ifdef __cplusplus
}
#endif

H264StreamSource::H264StreamSource(UsageEnvironment &env)
    : FramedSource(env)
{
}

H264StreamSource::~H264StreamSource()
{
}

H264StreamSource *H264StreamSource::createNew(UsageEnvironment &env)
{
  return new H264StreamSource(env);
}

void H264StreamSource::doGetNextFrame()
{
#if 1
  int fMaxSize = 128 * 1024;
  static FILE *fp = NULL;
  if (fp == NULL)
  {
    fp = fopen("test.264", "rb");
  }
  unsigned char *buffer = (unsigned char *)malloc(fMaxSize);
  if (fp)
  {
    fFrameSize = fread(buffer, 1, fMaxSize, fp);
    if (fFrameSize <= 0)
    {
      fseek(fp, 0, SEEK_SET);
      fFrameSize = fread(buffer, 1, fMaxSize, fp);
    }
  }
  if (fFrameSize > 0)
  {
    memmove(fTo, buffer, fFrameSize);
  }
  free(buffer);
#else
  unsigned char *video_data = NULL;
  int data_size = 0;
  if (video_input_read(&video_data, &data_size) == true)
  {
    fFrameSize = data_size;
    if (fFrameSize > 0)
    {
      memmove(fTo, video_data, fFrameSize);
    }
    free(video_data);
    gettimeofday(&fPresentationTime, NULL);
  }
  else
  {
    fFrameSize = data_size;
  }
#endif

  // static unsigned long long timestamp = 0;
  // timestamp = fPresentationTime.tv_sec * 1000 + fPresentationTime.tv_usec / 1000;
  // printf("timestamp:[%lld]\n", timestamp);

  nextTask() = envir().taskScheduler().scheduleDelayedTask(0, (TaskFunc *)FramedSource::afterGetting, this);
}

unsigned H264StreamSource::maxFrameSize() const
{
  return 150000;
}

void H264StreamSource::doStopGettingFrames()
{
}
