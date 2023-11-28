//
// Created by mkh on 07.11.2023.
//

#include "histogram.h"
#include "aacdec.h"
#include <cmath>

histogram::histogram( sf::Vector2u const& vp, size_t count, char const *fontname, int scale )
: m_viewport( vp )
, m_vertices( sf::PrimitiveType::Lines, count * 2 )
, m_scaler( scale > 0 ? aacdec::PACKLEN / scale : aacdec::PACKLEN * abs(scale) )
, m_coords( vp, count, fontname, m_scaler )
{
}

void histogram::update( uint32_t const *data )
{
    float y_center = m_viewport.y / 2;

    for( size_t v(0), h(0); v < m_vertices.getVertexCount(); v += 2, ++h)
    {
        float clipped = float(data[h]) / m_scaler;
        if( 1.f - clipped )
        {
            float up_y = y_center * (1.f - clipped);
            float down_y = y_center * (1.f + clipped);
            float x = coords::X_OFFSET + m_viewport.x * (1.f - (float(h) / (m_vertices.getVertexCount() / 2)));

            m_vertices[v].position = sf::Vector2f( x, up_y );
            m_vertices[v + 1].position = sf::Vector2f( x, down_y );
        }
    }
}

void histogram::draw( sf::RenderTarget& target, const sf::RenderStates& states) const
{
    sf::RenderStates st = states.transform * getTransform();
    target.draw( m_vertices, st );
    target.draw( m_coords, st );
}
