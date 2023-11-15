//
// Created by mkh on 07.11.2023.
//

#include <cstring>
#include <iostream>
#include "adts.h"

adts::adts( std::ifstream &f )
{
    f.read( (char*)m_raw, 7 );

    ::memcpy( &m_syncword, m_raw, sizeof(m_syncword) );
    m_syncword = be16toh( m_syncword );
    m_MPEGversion = MPEGVersion((m_syncword >> 3) & 1);
    m_protectionAbsence = m_syncword & 1;

    m_syncword >>= 4;
    if( m_syncword != SYNCWORD )
    {
        throw std::logic_error( "invalid file format" );
    }

    m_objectType = m_raw[2];
    m_freqIndex = ( m_objectType >> 2 ) &0x0f;
    m_channels = (m_objectType & 1) << 2;
    m_objectType = (m_objectType >> 6) + 1;
    m_channels |= ( m_raw[3] >> 6 ) & 3;

    m_frameLength = ( m_raw[3] & 3 ) << 11;
    m_frameLength |= ( m_raw[4] ) << 3;
    m_frameLength |= ( ( m_raw[5] >> 5 ) & 7 );

    if( ! m_protectionAbsence ) {
        f.read( (char*)&m_raw[7], 2 );
    }
}
