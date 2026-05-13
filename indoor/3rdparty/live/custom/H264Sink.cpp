#if (defined(__WIN32__) || defined(_WIN32)) && !defined(_WIN32_WCE)
#include <io.h>
#include <fcntl.h>
#endif
#include "GroupsockHelper.hh"
#include "OutputFile.hh"
#include "H264Sink.hh"

#ifdef __cplusplus
extern "C"
{
#endif

    extern bool video_decode_push(char type, unsigned char *data, int len);

#ifdef __cplusplus
}
#endif

H264Sink::H264Sink(UsageEnvironment &env, FILE *fid, unsigned bufferSize,
                   char const *perFrameFileNamePrefix)
    : MediaSink(env), fOutFid(fid), fBufferSize(bufferSize), fSamePresentationTimeCounter(0)
{
    fBuffer = new unsigned char[bufferSize];
    if (perFrameFileNamePrefix != NULL)
    {
        fPerFrameFileNamePrefix = strDup(perFrameFileNamePrefix);
        fPerFrameFileNameBuffer = new char[strlen(perFrameFileNamePrefix) + 100];
    }
    else
    {
        fPerFrameFileNamePrefix = NULL;
        fPerFrameFileNameBuffer = NULL;
    }
    fPrevPresentationTime.tv_sec = ~0;
    fPrevPresentationTime.tv_usec = 0;
}

H264Sink::~H264Sink()
{
    delete[] fPerFrameFileNameBuffer;
    delete[] fPerFrameFileNamePrefix;
    delete[] fBuffer;
    if (fOutFid != NULL)
        fclose(fOutFid);
}

H264Sink *H264Sink::createNew(UsageEnvironment &env, char const *fileName,
                              unsigned bufferSize, Boolean oneFilePerFrame)
{
    do
    {
        FILE *fid;
        char const *perFrameFileNamePrefix;
        if (oneFilePerFrame)
        {
            // Create the fid for each frame
            fid = NULL;
            perFrameFileNamePrefix = fileName;
        }
        else
        {
            // Normal case: create the fid once
            fid = OpenOutputFile(env, fileName);
            if (fid == NULL)
                break;
            perFrameFileNamePrefix = NULL;
        }

        return new H264Sink(env, fid, bufferSize, perFrameFileNamePrefix);
    } while (0);

    return NULL;
}

Boolean H264Sink::continuePlaying()
{
    if (fSource == NULL)
        return False;

    fSource->getNextFrame(fBuffer, fBufferSize,
                          afterGettingFrame, this,
                          onSourceClosure, this);

    return True;
}

void H264Sink::afterGettingFrame(void *clientData, unsigned frameSize,
                                 unsigned numTruncatedBytes,
                                 struct timeval presentationTime,
                                 unsigned /*durationInMicroseconds*/)
{
    H264Sink *sink = (H264Sink *)clientData;
    sink->afterGettingFrame(frameSize, numTruncatedBytes, presentationTime);
}

void H264Sink::addData(unsigned char const *data, unsigned dataSize,
                       struct timeval presentationTime)
{
    if (fPerFrameFileNameBuffer != NULL && fOutFid == NULL)
    {
        // Special case: Open a new file on-the-fly for this frame
        if (presentationTime.tv_usec == fPrevPresentationTime.tv_usec &&
            presentationTime.tv_sec == fPrevPresentationTime.tv_sec)
        {
            // The presentation time is unchanged from the previous frame, so we add a 'counter'
            // suffix to the file name, to distinguish them:
            sprintf(fPerFrameFileNameBuffer, "%s-%lu.%06lu-%u", fPerFrameFileNamePrefix,
                    presentationTime.tv_sec, presentationTime.tv_usec, ++fSamePresentationTimeCounter);
        }
        else
        {
            sprintf(fPerFrameFileNameBuffer, "%s-%lu.%06lu", fPerFrameFileNamePrefix,
                    presentationTime.tv_sec, presentationTime.tv_usec);
            fPrevPresentationTime = presentationTime; // for next time
            fSamePresentationTimeCounter = 0;         // for next time
        }
        fOutFid = OpenOutputFile(envir(), fPerFrameFileNameBuffer);
    }

    static int flag = 0;
    unsigned char type = data[0] & 0x1F;
    if (type == 5)
    {
        flag |= 0x01;
    }
    else if (type == 7)
    {
        flag |= 0x10;
    }
    else if (type == 8)
    {
        flag |= 0x0100;
    }
    else if (type == 1)
    {
        flag |= 0x1000;
    }

    if (flag & 0x10)
    {
        printf("====>> frame type:[0x%02x] size:[%d]\n", data[0], dataSize);
        unsigned char head[4] = {0x00, 0x00, 0x00, 0x01};
        int receive_frame_size = dataSize + 4;
        int frame_type = 0;

        unsigned char *receive_frame_buffer = (unsigned char *)malloc(receive_frame_size);
        memcpy(receive_frame_buffer, head, sizeof(head));
        memcpy(&receive_frame_buffer[4], data, dataSize);

        // video_decode_push(0,(unsigned char*)receive_frame_buffer, receive_frame_size);

        if (fOutFid != NULL && data != NULL)
        {
            fwrite(receive_frame_buffer, 1, receive_frame_size, fOutFid);
        }

        free(receive_frame_buffer);
    }
}

void H264Sink::afterGettingFrame(unsigned frameSize,
                                 unsigned numTruncatedBytes,
                                 struct timeval presentationTime)
{
    if (numTruncatedBytes > 0)
    {
        envir() << "H264Sink::afterGettingFrame(): The input frame data was too large for our buffer size ("
                << fBufferSize << ").  "
                << numTruncatedBytes << " bytes of trailing data was dropped!  Correct this by increasing the \"bufferSize\" parameter in the \"createNew()\" call to at least "
                << fBufferSize + numTruncatedBytes << "\n";
    }
    addData(fBuffer, frameSize, presentationTime);

    if (fOutFid == NULL || fflush(fOutFid) == EOF)
    {
        // The output file has closed.  Handle this the same way as if the input source had closed:
        if (fSource != NULL)
            fSource->stopGettingFrames();
        onSourceClosure();
        return;
    }

    if (fPerFrameFileNameBuffer != NULL)
    {
        if (fOutFid != NULL)
        {
            fclose(fOutFid);
            fOutFid = NULL;
        }
    }

    // Then try getting the next frame:
    continuePlaying();
}