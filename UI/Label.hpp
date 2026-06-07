#pragma once
#include <SFML/Graphics.hpp>
#include <string>

class Label
{
public:
    Label(const sf::Vector2f& position, const sf::Vector2f& size, const sf::Font& font, const std::string& text, unsigned int characterSize = 16);
    ~Label() = default;

    void SetText(const std::string& text);
    void Draw(sf::RenderWindow* window);

private:
    sf::RectangleShape background;
    sf::Text shapeText;
};