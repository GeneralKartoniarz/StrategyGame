#pragma once
#include <SFML/Graphics.hpp>
#include <string>

class Label
{
public:
    Label(const sf::Vector2f &position, const sf::Vector2f &size, const sf::Font &font, const std::string &text, unsigned int characterSize = 16);
    ~Label() = default;
    bool Contains(const sf::Vector2f &point) const;
    void SetText(const std::string &text);
    void Draw(sf::RenderWindow *window);
    void SetPosition(sf::Vector2f pos);
private:
    sf::RectangleShape background;
    sf::Text shapeText;
};