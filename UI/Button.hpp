#pragma once
#include <utility>
#include <functional>

#include <SFML/Graphics.hpp>
#include <string>

class Button
{
public:
    Button(const sf::Vector2f &position, const sf::Vector2f &size, const sf::Font &font, const std::string &text, void (*onClickAction)());
    Button(const sf::Vector2f &position, float radius, const sf::Font &font, const std::string &text, void (*onClickAction)());
    ~Button() = default;
    bool Contains(const sf::Vector2f &point) const;
    void Update(const sf::Vector2i &mousePos, bool mouseClicked);
    void Draw(sf::RenderWindow *window);

private:
    enum class ButtonState
    {
        Idle,
        Hover,
        Pressed
    };
    ButtonState state;

    std::unique_ptr<sf::Shape> background;
    sf::Text shapeText;
    void (*action)();
    sf::Color idleColor;
    sf::Color hoverColor;
    sf::Color pressedColor;
};