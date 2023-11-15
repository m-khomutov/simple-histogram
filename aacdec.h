//
// Created by mkh on 07.11.2023.
//

#ifndef AACHISTOGRAM_AACDEC_H
#define AACHISTOGRAM_AACDEC_H

#include <vector>

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavutil/opt.h>
}


class aplayer;

class aacdec {
public:
    static const size_t PACKLEN = 1024;
    static const int32_t MIN_LEVEL_DB = -130;
    static constexpr float MAX_LEVEL_VALUE = 1.f;

    aacdec();
    ~aacdec();

    void decode( uint8_t *buf, size_t size, aplayer *aplayer );
    uint32_t const * histogram() const
    {
        return m_histogram;
    }

private:
    AVCodec *m_codec;
    AVCodecContext *m_context {nullptr};
    AVPacket m_avpkt;
    AVFrame *m_frame = {nullptr};

    std::vector< float > m_samples;
    uint32_t m_histogram[-1 * MIN_LEVEL_DB];
};


#endif //AACHISTOGRAM_AACDEC_H
