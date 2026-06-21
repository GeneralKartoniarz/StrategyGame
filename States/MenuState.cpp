#include "MenuState.hpp"
#include "TestState.hpp"
#include <iostream>

MenuState* MenuState::instance = nullptr;

MenuState::MenuState(sf::RenderWindow* windowPtr) : States(windowPtr)
{
    instance = this;

    this->bgTexture = std::make_unique<sf::Texture>();
    if (this->bgTexture->loadFromFile("resources/textures/background.png"))
    {
        this->bgSprite = std::make_unique<sf::Sprite>(*this->bgTexture);
        
        sf::Vector2f targetSize = static_cast<sf::Vector2f>(this->windowPtr->getSize());
        this->bgSprite->setScale({
            targetSize.x / this->bgSprite->getLocalBounds().size.x,
            targetSize.y / this->bgSprite->getLocalBounds().size.y
        });
    }

    if (!this->font.openFromFile("resources/fonts/ARIAL.TTF"))
    {
        this->font.openFromFile("C:/Windows/Fonts/Arial.ttf");
    }

    this->titleLabel = std::make_unique<Label>(sf::Vector2f(760.f, 300.f), sf::Vector2f(400.f, 80.f), this->font, "Super Hiper Strategia", 24);
    this->playButton = std::make_unique<Button>(sf::Vector2f(810.f, 500.f), sf::Vector2f(300.f, 80.f), this->font, "Nowa Gra", &MenuState::OnPlayClick);
}

MenuState::~MenuState()
{
    if (instance == this) instance = nullptr;
}

void MenuState::OnPlayClick()
{
    if (instance)
    {
        instance->requestGameState = true;
    }
}

void MenuState::HandleEvent(const sf::Event& event)
{
}

void MenuState::Update(float dt)
{
    sf::Vector2i mousePos = sf::Mouse::getPosition(*this->windowPtr);
    bool mouseClicked = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);

    this->playButton->Update(mousePos, mouseClicked);

    if (this->requestGameState)
    {
        this->quit = true;
        this->nextState = std::make_unique<TestState>(this->windowPtr);
    }
}

void MenuState::Render(sf::RenderWindow* windowPtr)
{
    if (this->bgSprite)
    {
        windowPtr->draw(*this->bgSprite);
    }
    
    this->titleLabel->Draw(windowPtr);
    this->playButton->Draw(windowPtr);
}