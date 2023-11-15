//
// Created by mkh on 07.11.2023.
//

#ifndef AACHISTOGRAM_ADTS_H
#define AACHISTOGRAM_ADTS_H

#include <fstream>

class adts {
public:
    enum MPEGVersion { MPEG4 = 0, MPEG2 };
    enum Profile { };

    adts( std::ifstream &f );

    size_t size() const
    {
        return m_protectionAbsence ? SIZE - 2 : SIZE;
    }
    uint8_t const *raw() const
    {
        return m_raw;
    }
    size_t framesize() const
    {
        return m_frameLength;
    }
    uint32_t frequency() const
    {
        return Frequency[m_freqIndex];
    }
    uint32_t channels() const
    {
        return m_channels;
    }

private:
    static const uint16_t SYNCWORD = 0x0fff;

    const uint32_t Frequency[13] {
            96000, 88200, 64000, 48000, 44100, 32000, 24000, 22050, 16000, 12000, 11025, 8000, 7350,
    };
    static const size_t SIZE = 9;

    uint8_t m_raw[ SIZE ];

    uint16_t m_syncword {0xfff};
    MPEGVersion m_MPEGversion {MPEG4};
    uint8_t layer {0};
    bool m_protectionAbsence {true};
    uint8_t m_objectType;
    uint8_t m_freqIndex;
    bool m_private {false};
    uint8_t m_channels;
    bool m_originality {true};
    bool m_home {false};
    bool m_copyrightID[2];
    uint16_t m_frameLength {0};
    uint16_t m_fullness;
    uint8_t m_frameCount;
    uint16_t m_crc;
};


#endif //AACHISTOGRAM_ADTS_H
