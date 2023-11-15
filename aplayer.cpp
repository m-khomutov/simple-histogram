//
// Created by mkh on 07.11.2023.
//

#include <stdexcept>
#include "aplayer.h"
#include <iostream>

#define PCM_DEVICE "default"

aplayer::aplayer( int rate, int channels )
{
    unsigned int pcm;
    if( pcm = snd_pcm_open( &m_handle, PCM_DEVICE, SND_PCM_STREAM_PLAYBACK, 0) < 0)
    {
        throw std::logic_error(std::string("ERROR: Can't open \"") + PCM_DEVICE + "\" PCM device. " + snd_strerror(pcm));
    }
    if( (pcm = snd_pcm_set_params( m_handle,
                                   SND_PCM_FORMAT_FLOAT,
                                   SND_PCM_ACCESS_RW_INTERLEAVED,
                                   channels,
                                   rate, 1, (uint64_t)1152 * 1000000L / (uint64_t)rate )) < 0 )
    {
        throw std::logic_error( snd_strerror(pcm));
    }
}

aplayer::~aplayer()
{
    snd_pcm_drain( m_handle );
    snd_pcm_close( m_handle );
}

void aplayer::play( uint8_t *samples, size_t count )
{
    snd_pcm_sframes_t frames;
    if( (frames = snd_pcm_writei( m_handle, samples, count)) < 0 )
    {
        frames = snd_pcm_recover( m_handle, frames, 0 );
    }
    if( frames < 0 )
    {
        std::cerr<< "snd_pcm_writei failed: " << snd_strerror(frames) << std::endl;
        return;
    }
    if( frames > 0 && frames < count )
    {
        std::cerr << "Short write (expected " << count << ", wrote " << frames << std::endl;
    }
}
