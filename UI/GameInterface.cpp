#include "GameInterface.hpp"
#include <iostream>

GameInterface* GameInterface::instance = nullptr;

GameInterface::GameInterface(sf::RenderWindow* window) : turnCount(1)
{
    instance = this;

    if (!this->font.openFromFile("resurces/fonts/ARIAL.TTF")) 
    {
        this->font.openFromFile("C:/Windows/Fonts/Arial.ttf"); 
    }

    std::vector<std::string> statNames = { "Nauka: 0", "Kultura: 0", "Wiara: 0", "Zloto: 0", "Turystyka: 0" };
    float startX = 20.0f;
    float startY = 20.0f;
    float labelWidth = 140.0f;
    float labelHeight = 35.0f;
    float gap = 10.0f;

    for (int i = 0; i < 5; ++i)
    {
        sf::Vector2f pos({startX + i * (labelWidth + gap), startY});
        this->statLabels.push_back(std::make_unique<Label>(pos, sf::Vector2f({labelWidth, labelHeight}), this->font, statNames[i]));
    }

    this->turnCounterLabel = std::make_unique<Label>(sf::Vector2f({1730.0f, 900.0f}), sf::Vector2f({150.0f, 30.0f}), this->font, "TURA 1", 14);

    this->nextTurnButton = std::make_unique<Button>(sf::Vector2f({1780.0f, 940.0f}), 50.0f, this->font, "NEXT", &GameInterface::OnNextTurnClick);
}
bool GameInterface::IsMouseOverUI(const sf::Vector2i& mousePos) const
{
    sf::Vector2f fMousePos = static_cast<sf::Vector2f>(mousePos);
    if (this->nextTurnButton && this->nextTurnButton->Contains(fMousePos))
    {
        return true;
    }

    if (this->turnCounterLabel && this->turnCounterLabel->Contains(fMousePos))
    {
        return true;
    }

    for (const auto& label : this->statLabels)
    {
        if (label && label->Contains(fMousePos))
        {
            return true;
        }
    }

    return false;
}
GameInterface::~GameInterface()
{
    if (instance == this)
    {
        instance = nullptr;
    }
}

void GameInterface::OnNextTurnClick()
{
    if (instance)
    {
        instance->NextTurn();
    }
}

void GameInterface::NextTurn()
{
    this->turnCount++;
    this->turnCounterLabel->SetText("TURA " + std::to_string(this->turnCount));
    std::cout << "Menedzer interfejsu: Rozpoczeto ture numer " << this->turnCount << std::endl;
}

void GameInterface::Update(float dt, const sf::Vector2i& mousePos, bool mouseClicked)
{
    this->nextTurnButton->Update(mousePos, mouseClicked);
}

void GameInterface::Draw(sf::RenderWindow* window)
{
    for (auto& label : this->statLabels)
    {
        label->Draw(window);
    }

    this->turnCounterLabel->Draw(window);
    this->nextTurnButton->Draw(window);
}