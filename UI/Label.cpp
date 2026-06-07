#include "Label.hpp"

Label::Label(const sf::Vector2f &position, const sf::Vector2f &size, const sf::Font &font, const std::string &text, unsigned int characterSize)
    : shapeText(font)
{
    this->background.setPosition(position);
    this->background.setSize(size);
    this->background.setFillColor(sf::Color(25, 25, 25, 220));
    this->background.setOutlineThickness(1.0f);
    this->background.setOutlineColor(sf::Color(180, 160, 110)); 

    this->shapeText.setString(text);
    this->shapeText.setCharacterSize(characterSize);
    this->shapeText.setFillColor(sf::Color(230, 230, 230));
    this->shapeText.setString(sf::String::fromUtf8(text.begin(), text.end()));
    sf::FloatRect textBounds = this->shapeText.getLocalBounds();

    this->shapeText.setOrigin({textBounds.position.x + textBounds.size.x / 2.0f, textBounds.position.y + textBounds.size.y / 2.0f});
    this->shapeText.setPosition({position.x + size.x / 2.0f, position.y + size.y / 2.0f});
}

void Label::SetText(const std::string &text)
{
    this->shapeText.setString(text);
    sf::FloatRect textBounds = this->shapeText.getLocalBounds();
    this->shapeText.setOrigin({textBounds.position.x + textBounds.size.x / 2.0f, textBounds.position.y + textBounds.size.y / 2.0f});
    this->shapeText.setString(sf::String::fromUtf8(text.begin(), text.end()));
}

void Label::Draw(sf::RenderWindow *window)
{
    window->draw(this->background);
    window->draw(this->shapeText);
}
bool Label::Contains(const sf::Vector2f &point) const
{
    return this->background.getGlobalBounds().contains(point);
}