//
// Created by mkh on 14.11.2023.
//

#include "coords.h"

tick::tick( const sf::Vector2f &caption_pos,
            const sf::Vector2f & notch_pos,
            const sf::Vector2f & notch_size,
            sf::Font *font,
            char const *text )
: m_caption( *font )
, m_notch( notch_size )
{
    m_caption.setString( text );
    m_caption.setCharacterSize(15);
    m_caption.setFillColor( sf::Color::Red );
    m_caption.setStyle( sf::Text::Regular );
    sf::FloatRect r = m_caption.getLocalBounds();
    m_caption.setOrigin( sf::Vector2f( r.width / 2, r.height / 2 ) );
    m_caption.setPosition( sf::Vector2f( caption_pos.x, caption_pos.y + 10.f) );

    m_notch.setFillColor( sf::Color( 255, 0, 0 ) );
    m_notch.setPosition( notch_pos );
    m_notch.setOrigin(sf::Vector2f(m_notch.getSize().x / 2, m_notch.getSize().y / 2));
}

void tick::draw( sf::RenderTarget& target, const sf::RenderStates& states) const
{
    sf::RenderStates st = states.transform * getTransform();
    target.draw( m_caption, st );
    target.draw( m_notch, st );
}


axis::axis( sf::Font *font, char const *str )
: caption( *font )
{
    caption.setString( str );
    caption.setCharacterSize(20);
    caption.setFillColor( sf::Color::Red );
    caption.setStyle( sf::Text::Bold );
    sf::FloatRect r = caption.getLocalBounds();
    caption.setOrigin( sf::Vector2f( r.width / 2, r.height / 2 ) );
}

void axis::draw( sf::RenderTarget& target, const sf::RenderStates& states) const
{
    sf::RenderStates st = states.transform * getTransform();
    target.draw( line, 2, sf::PrimitiveType::Lines );
    target.draw( caption, st );
    for( size_t t(0); t < ticks.size(); ++t )
    {
        target.draw( ticks[t], st );
    }
}



coords::coords( const sf::Vector2u &vp, size_t count, char const *fontname, size_t y_tick_count )
:  m_x_axis( &m_font, std::string("Db").c_str() )
,  m_y_axis( &m_font, std::string("sum").c_str() )
{
    if( !m_font.loadFromFile(fontname)) {
        throw std::logic_error( std::string("font error: ") + fontname );
    }
    f_set_x_axis( vp, count );
    f_set_y_axis( vp, y_tick_count );
}

void coords::draw( sf::RenderTarget& target, const sf::RenderStates& states) const
{
    sf::RenderStates st = states.transform * getTransform();
    target.draw( m_x_axis, st );
    target.draw( m_y_axis, st );
}

void coords::f_set_x_axis( const sf::Vector2u &vp, size_t count )
{
    float y_center = vp.y / 2;
    m_x_axis.caption.setPosition( sf::Vector2f(vp.x - 40.f, vp.y / 2 + 10.f) );
    m_x_axis.line[0] = sf::Vertex(sf::Vector2f(X_OFFSET, y_center));
    m_x_axis.line[1] = sf::Vertex(sf::Vector2f(vp.x, y_center));
    for( int t(0); t < count; t += 10 )
    {
        sf::Vector2f pos(coords::X_OFFSET + vp.x * (float(t) / count), y_center);
        m_x_axis.ticks.push_back( tick(pos,
                                       pos,
                                       sf::Vector2f(2.f, 5.f),
                                       &m_font,
                                       std::to_string(-1 * t).c_str()) );
    }
}

#include "iostream"
void coords::f_set_y_axis( const sf::Vector2u &vp, size_t count )
{
    float y_center = vp.y / 2;
    m_y_axis.caption.setPosition( sf::Vector2f(X_OFFSET + 10.f, 10.f) );
    m_y_axis.line[0] = sf::Vertex(sf::Vector2f(X_OFFSET, 0.f));
    m_y_axis.line[1] = sf::Vertex(sf::Vector2f(X_OFFSET, vp.y));
    size_t delta = count / 10;
    for( size_t t(0); t < count; t += delta )
    {
        sf::Vector2f pos(coords::X_OFFSET + 20.f, y_center * (1.f - (float(t) / count)));
        m_y_axis.ticks.push_back( tick( sf::Vector2f(pos.x, pos.y - 10.f),
                                        sf::Vector2f(X_OFFSET, pos.y),
                                        sf::Vector2f(5.f, 2.f),
                                        &m_font,
                                        t ? std::to_string(t).c_str() : "") );
    }
}