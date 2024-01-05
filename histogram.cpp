//
// Created by mkh on 07.11.2023.
//

#include "histogram.h"
#include "aacdec.h"
#include <cmath>

histogram::histogram( sf::Vector2u const& vp, size_t count, char const *fontname, float scale )
: m_viewport( vp )
, m_vertices( sf::PrimitiveType::LineStrip, count )
, m_scaler( 512 * scale )
, m_coef_x( m_viewport.x / m_vertices.getVertexCount() )
, m_coef_y( m_viewport.y / float(m_scaler) )
, m_coords( vp, count, fontname, m_scaler )
{
}

void histogram::update( uint64_t const *data )
{
    for( size_t v(0); v < m_vertices.getVertexCount(); ++v )
    {
        m_vertices[v].position = sf::Vector2f( coords::X_OFFSET + v * m_coef_x,
                                               m_viewport.y - (coords::Y_OFFSET + float(data[v]) * m_coef_y) );
    }
}

void histogram::draw( sf::RenderTarget& target, sf::RenderStates states) const
{
    sf::RenderStates st = states.transform * getTransform();
    target.draw( m_vertices, st );
    target.draw( m_coords, st );
}
