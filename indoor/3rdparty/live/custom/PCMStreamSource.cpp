#include "PCMStreamSource.hh"
#include "InputFile.hh"
#include "GroupsockHelper.hh"

#ifdef __cplusplus
extern "C"
{
#endif

	extern bool audio_input_read(unsigned char **data, int *size);

#ifdef __cplusplus
}
#endif

PCMStreamSource *PCMStreamSource::createNew(UsageEnvironment &env, unsigned char sample_bit, unsigned char channel, unsigned int sample_rate)
{
	PCMStreamSource *newSource = new PCMStreamSource(env, sample_bit, channel, sample_rate);
	return newSource;
}

PCMStreamSource::PCMStreamSource(UsageEnvironment &env, unsigned char sample_bit, unsigned char channel, unsigned int sample_rate)
	: FramedSource(env)
{
}

PCMStreamSource::~PCMStreamSource()
{
}

void PCMStreamSource::doGetNextFrame()
{
	int fBitsPerSample = 16;
	int fNumChannels = 1;
	int fSamplingFrequency = 16000;

	int fPlayTimePerSample = 1e6 / (double)fSamplingFrequency;
	unsigned maxSamplesPerFrame = (1400 * 8) / (fNumChannels * fBitsPerSample);
	unsigned desiredSamplesPerFrame = (unsigned)(0.02 * fSamplingFrequency);
	unsigned samplesPerFrame = desiredSamplesPerFrame < maxSamplesPerFrame ? desiredSamplesPerFrame : maxSamplesPerFrame;
	int fPreferredFrameSize = (samplesPerFrame * fNumChannels * fBitsPerSample) / 8;

	fFrameSize = 0; // until it's set later

	unsigned bytesPerSample = (fNumChannels * fBitsPerSample) / 8;
	if (bytesPerSample == 0)
		bytesPerSample = 1; // because we can't read less than a byte at a time

#if 1
	unsigned numBytesRead;
	static FILE *fp = NULL;
	if (fp == NULL)
	{
		fp = fopen("test.pcm", "rb");
	}
	if (fp)
	{
		numBytesRead = fread(fTo, 1, fPreferredFrameSize, fp);
		if (numBytesRead <= 0)
		{
			fseek(fp, 0, SEEK_SET);
			numBytesRead = fread(fTo, 1, fPreferredFrameSize, fp);
		}
	}
	fFrameSize = numBytesRead;
	// printf("numBytesRead:[%d]\n", numBytesRead);
	// numBytesRead = ringbuffer_read(&audio_input_ringbuffer, fTo, fPreferredFrameSize);
#else
	unsigned char *audio_data = NULL;
	int data_size = 0;
	if (audio_input_read(&audio_data, &data_size) == true)
	{
		fFrameSize = data_size;
		if (fFrameSize > 0)
		{
			memmove(fTo, audio_data, fFrameSize);
		}
		free(audio_data);
	}
	else
	{
		fFrameSize = 0;
	}
#endif
	// printf("============>>> fFrameSize:[%d]\n", fFrameSize);
	if (fFrameSize == 0)
	{
		nextTask() = envir().taskScheduler().scheduleDelayedTask(10000, (TaskFunc *)FramedSource::afterGetting, this);
		return;
	}
	gettimeofday(&fPresentationTime, NULL);
	fDurationInMicroseconds = (unsigned)((fPlayTimePerSample * fFrameSize) / bytesPerSample);
	nextTask() = envir().taskScheduler().scheduleDelayedTask(fDurationInMicroseconds, (TaskFunc *)FramedSource::afterGetting, this);
}
