//
// Created by mkh on 07.11.2023.
//

#include <cstring>
#include "adts.h"

adts::adts( std::ifstream &f )
{
    f.read( (char*)m_raw, 7 );
    bitreader br( m_raw, 7 );

    m_syncword = br.bits ( 12 );
    if( m_syncword != SYNCWORD )
    {
        throw std::logic_error( "invalid file format" );
    }
    m_MPEGversion = MPEGVersion( br.bit () );
    m_layer = br.bits ( 2 );
    m_protectionAbsence = br.bit ();

    m_profile = br.bits ( 2 );
    m_sampling_frequency_index = br.bits ( 4 );
    m_private = br.bit ();
    m_channel_configuration = br.bits ( 3 );
    m_original = br.bit ();
    m_home = br.bit ();
    m_copyrightID[0] = br.bit ();
    m_copyrightID[1] = br.bit ();

    m_frameLength = br.bits ( 13 );
    m_fullness = br.bits ( 11 );
    m_frameCount = br.bits ( 2 );

    if( ! m_protectionAbsence ) {
        f.read( (char*)&m_raw[7], 2 );
    }
}
