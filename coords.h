//
// Created by mkh on 14.11.2023.
//

#ifndef AACHISTOGRAM_COORDS_H
#define AACHISTOGRAM_COORDS_H


#include <SFML/Graphics.hpp>

class tick: public sf::Drawable, public sf::Transformable {
public:
    tick( sf::Vector2f const& caption_pos,
          sf::Vector2f const& notch_pos,
          sf::Vector2f const& notch_size,
          sf::Font *font,
          char const *text );

private:
    sf::Text m_caption;
    sf::RectangleShape m_notch;

private:
    void draw( sf::RenderTarget& target, const sf::RenderStates& states) const override;
};

struct axis: public sf::Drawable, public sf::Transformable
{
    axis( sf::Font *font, char const *str );

    sf::Text caption;
    sf::Vertex line[2];
    std::vector< tick > ticks;

private:
    void draw( sf::RenderTarget& target, const sf::RenderStates& states) const override;
};

class coords: public sf::Drawable, public sf::Transformable {
public:
    static const size_t X_OFFSET = 10;

    coords( sf::Vector2u const& vp, size_t count, char const *fontname, size_t y_tick_count );

private:
    sf::Font m_font;
    axis m_x_axis;
    axis m_y_axis;

private:
    void draw( sf::RenderTarget& target, const sf::RenderStates& states) const override;

    void f_set_x_axis( const sf::Vector2u &vp, size_t count );
    void f_set_y_axis( const sf::Vector2u &vp, size_t count );
};


#endif //AACHISTOGRAM_COORDS_H
