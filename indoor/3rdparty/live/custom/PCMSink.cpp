#if (defined(__WIN32__) || defined(_WIN32)) && !defined(_WIN32_WCE)
#include <io.h>
#include <fcntl.h>
#endif
#include "GroupsockHelper.hh"
#include "OutputFile.hh"
#include "PCMSink.hh"

////////// PCMSink //////////

PCMSink::PCMSink(UsageEnvironment &env, FILE *fid, unsigned bufferSize,
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

PCMSink::~PCMSink()
{
    delete[] fPerFrameFileNameBuffer;
    delete[] fPerFrameFileNamePrefix;
    delete[] fBuffer;
    if (fOutFid != NULL)
        fclose(fOutFid);
}

PCMSink *PCMSink::createNew(UsageEnvironment &env, char const *fileName,
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

        return new PCMSink(env, fid, bufferSize, perFrameFileNamePrefix);
    } while (0);

    return NULL;
}

Boolean PCMSink::continuePlaying()
{
    if (fSource == NULL)
        return False;

    fSource->getNextFrame(fBuffer, fBufferSize,
                          afterGettingFrame, this,
                          onSourceClosure, this);

    return True;
}

void PCMSink::afterGettingFrame(void *clientData, unsigned frameSize,
                                unsigned numTruncatedBytes,
                                struct timeval presentationTime,
                                unsigned /*durationInMicroseconds*/)
{
    PCMSink *sink = (PCMSink *)clientData;
    sink->afterGettingFrame(frameSize, numTruncatedBytes, presentationTime);
}

void PCMSink::addData(unsigned char const *data, unsigned dataSize,
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
    printf("====>> audio frame size:[%d]\n", dataSize);
    if (fOutFid != NULL && data != NULL)
    {
        fwrite(data, 1, dataSize, fOutFid);
    }
}

void PCMSink::afterGettingFrame(unsigned frameSize,
                                unsigned numTruncatedBytes,
                                struct timeval presentationTime)
{
    if (numTruncatedBytes > 0)
    {
        envir() << "PCMSink::afterGettingFrame(): The input frame data was too large for our buffer size ("
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