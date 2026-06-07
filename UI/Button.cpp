#include "Button.hpp"

Button::Button(const sf::Vector2f &position, const sf::Vector2f &size, const sf::Font &font, const std::string &text, void (*onClickAction)())
    : action(onClickAction), state(ButtonState::Idle), idleColor(35, 35, 35, 240), hoverColor(60, 55, 45, 240), pressedColor(180, 160, 110),
      shapeText(font)
{
    auto rect = std::make_unique<sf::RectangleShape>(size);
    rect->setPosition(position);
    this->background = std::move(rect);

    this->background->setOutlineThickness(1.5f);
    this->background->setOutlineColor(sf::Color(180, 160, 110));

    this->shapeText.setFont(font);
    this->shapeText.setString(text);
    this->shapeText.setCharacterSize(16);
    this->shapeText.setFillColor(sf::Color::White);

    sf::FloatRect textBounds = this->shapeText.getLocalBounds();
    this->shapeText.setOrigin({textBounds.position.x + textBounds.size.x / 2.0f, textBounds.position.y + textBounds.size.y / 2.0f});
    this->shapeText.setPosition({position.x + size.x / 2.0f, position.y + size.y / 2.0f});
}

Button::Button(const sf::Vector2f &position, float radius, const sf::Font &font, const std::string &text, void (*onClickAction)())
    : action(onClickAction), state(ButtonState::Idle), idleColor(115, 20, 20), hoverColor(150, 40, 40), pressedColor(180, 160, 110),
      shapeText(font)
{
    auto circle = std::make_unique<sf::CircleShape>(radius);
    circle->setPosition(position);
    this->background = std::move(circle);

    this->background->setOutlineThickness(2.0f);
    this->background->setOutlineColor(sf::Color(180, 160, 110));

    this->shapeText.setFont(font);
    this->shapeText.setString(text);
    this->shapeText.setCharacterSize(18);
    this->shapeText.setFillColor(sf::Color::White);

    sf::FloatRect textBounds = this->shapeText.getLocalBounds();
    this->shapeText.setOrigin({textBounds.position.x + textBounds.size.x / 2.0f, textBounds.position.y + textBounds.size.y / 2.0f});
    this->shapeText.setPosition({position.x + radius, position.y + radius});
}

void Button::Update(const sf::Vector2i &mousePos, bool mouseClicked)
{
    if (this->background->getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos)))
    {
        if (mouseClicked)
        {
            if (this->state != ButtonState::Pressed)
            {
                this->state = ButtonState::Pressed;
                if (this->action)
                    this->action();
            }
        }
        else
        {
            this->state = ButtonState::Hover;
        }
    }
    else
    {
        this->state = ButtonState::Idle;
    }

    if (this->state == ButtonState::Idle)
        this->background->setFillColor(this->idleColor);
    else if (this->state == ButtonState::Hover)
        this->background->setFillColor(this->hoverColor);
    else if (this->state == ButtonState::Pressed)
        this->background->setFillColor(this->pressedColor);
}

void Button::Draw(sf::RenderWindow *window)
{
    window->draw(*this->background);
    window->draw(this->shapeText);
}