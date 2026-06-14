#include "GameInterface.hpp"
#include <iostream>

GameInterface *GameInterface::instance = nullptr;

GameInterface::GameInterface(sf::RenderWindow *window, GameManager &gm) : gm(gm), turnCount(1)
{
    instance = this;

    if (!this->font.openFromFile("resurces/fonts/ARIAL.TTF"))
    {
        this->font.openFromFile("C:/Windows/Fonts/Arial.ttf");
    }

    std::vector<std::string> statNames = {"Nauka: 0", "Kultura: 0", "Wiara: 0", "Złoto: 0", "Turystyka: 0"};
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

    this->sidePanel = std::make_unique<SidePanel>(sf::Vector2f({1650.0f, 20.0f}), sf::Vector2f({250.0f, 130.0f}), this->font);
    this->unitPanel = std::make_unique<UnitPanel>(sf::Vector2f({1650.0f, 170.0f}), sf::Vector2f({250.0f, 130.0f}), this->font);
    // TODO stale wartosci nie mam sily juz freezuje na amen wystarczy dodac zmienne w klasie
    this->cityPanel = std::make_unique<CityPanel>(sf::Vector2f({1450.0f, 320.0f}), sf::Vector2f({400.0f, 180.0f}), this->font);
}

bool GameInterface::IsMouseOverUI(const sf::Vector2i &mousePos) const
{
    sf::Vector2f fMousePos = static_cast<sf::Vector2f>(mousePos);
    if (this->sidePanel && this->sidePanel->Contains(fMousePos))
        return true;
    if (this->unitPanel && this->unitPanel->Contains(fMousePos))
        return true;
    if (this->nextTurnButton && this->nextTurnButton->Contains(fMousePos))
        return true;
    if (this->turnCounterLabel && this->turnCounterLabel->Contains(fMousePos))
        return true;
    if (this->cityPanel && this->cityPanel->Contains(fMousePos))
        return true;
    for (const auto &label : this->statLabels)
    {
        if (label && label->Contains(fMousePos))
            return true;
    }
    return false;
}

GameInterface::~GameInterface()
{
    if (instance == this)
        instance = nullptr;
}

void GameInterface::OnNextTurnClick()
{
    if (instance)
        instance->NextTurn();
}
void GameInterface::NextTurn()
{
    this->turnCount++;
    this->turnCounterLabel->SetText("TURA " + std::to_string(this->turnCount));

    if (this->onNextTurnAction)
    {
        this->onNextTurnAction();
    }
}
void GameInterface::Update(float dt, const sf::Vector2i &mousePos, bool mouseClicked)
{
    this->nextTurnButton->Update(mousePos, mouseClicked);
}

void GameInterface::Draw(sf::RenderWindow *window)
{
    for (auto &label : this->statLabels)
        label->Draw(window);
    this->sidePanel->Draw(window);
    this->turnCounterLabel->Draw(window);
    this->nextTurnButton->Draw(window);
    this->unitPanel->Draw(window);
    this->cityPanel->Draw(window);
}

void GameInterface::UpdateSelection(const Tile *tile)
{
    if (this->sidePanel)
        this->sidePanel->UpdateSelection(tile);
}
void GameInterface::UpdateCitySelection(const City *city, const std::string &empireName)
{
    if (this->cityPanel)
        this->cityPanel->UpdateSelection(city, empireName, this->gm);
}
void GameInterface::UpdateUnitSelection(const Unit *unit)
{
    this->isUnitSelected = (unit != nullptr);

    if (this->unitPanel)
        this->unitPanel->UpdateSelection(unit);

    this->UpdateCityPanelPosition();
}

void GameInterface::UpdateCityPanelPosition()
{
    if (this->cityPanel)
    {
        float targetY = this->isUnitSelected ? 320.0f : 170.0f;
    }

    
}
