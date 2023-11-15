//
// Created by mkh on 07.11.2023.
//

#ifndef AACHISTOGRAM_APLAYER_H
#define AACHISTOGRAM_APLAYER_H

extern "C"
{
#include <alsa/asoundlib.h>
}

class aplayer {
public:
    aplayer( int rate, int channels );
    ~aplayer();

    void play( uint8_t *samples, size_t count );

private:
    snd_pcm_t *m_handle;
};


#endif //AACHISTOGRAM_APLAYER_H
