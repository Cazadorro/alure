#ifndef DECODERS_SNDFILE_H
#define DECODERS_SNDFILE_H

#include "alure2.h"

#include "sndfile.h"

namespace alure {

class SndFileDecoder : public Decoder {
    SNDFILE *mSndFile;
    SF_INFO mSndInfo;

public:
    SndFileDecoder(SNDFILE *sndfile, const SF_INFO &info);
    virtual ~SndFileDecoder();

    virtual void getFormat(ALuint *srate, SampleConfig *chans, SampleType *type) final;

    virtual ALuint getLength() final;
    virtual ALuint getPosition() final;
    virtual bool seek(ALuint pos) final;

    virtual ALsizei read(ALvoid *ptr, ALsizei count) final;

    static Decoder *openFile(const char *name);
};

} // namespace alure

#endif /* DECODERS_SNDFILE_H */