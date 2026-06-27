#include "GameInterface.hpp"
#include <iostream>
#include "BuildPanel.hpp"
GameInterface *GameInterface::instance = nullptr;

GameInterface::GameInterface(sf::RenderWindow *window, GameManager &gm) : gm(gm), turnCount(1)
{
    instance = this;

    if (!this->font.openFromFile("resources/fonts/ARIAL.TTF"))
    {
        this->font.openFromFile("C:/Windows/Fonts/Arial.ttf");
    }

    std::vector<std::string> statNames = {"Nauka: 0", "Kultura: 0", "Wiara: 0", "Zloto: 0", "Turystyka: 0"};
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
    this->sidePanel = std::make_unique<SidePanel>(sf::Vector2f({1650.0f, 20.0f}), sf::Vector2f({250.0f, 260.0f}), this->font);
    this->unitPanel = std::make_unique<UnitPanel>(sf::Vector2f({1650.0f, 300.0f}), sf::Vector2f({250.0f, 130.0f}), this->font);
    this->cityPanel = std::make_unique<CityPanel>(sf::Vector2f({1450.0f, 450.0f}), sf::Vector2f({400.0f, 180.0f}), this->font);

    this->buildPanel = std::make_unique<BuildPanel>(sf::Vector2f({20.0f, 850.0f}), sf::Vector2f({300.0f, 120.0f}), this->font);
    this->analyticsPanel = std::make_unique<AnalyticsPanel>(sf::Vector2f({20.0f, 580.0f}), sf::Vector2f({400.0f, 250.0f}), this->font);
    BuildPanel::gameManagerContext = &this->gm;
}

bool GameInterface::IsMouseOverUI(const sf::Vector2i &mousePos) const
{
    sf::Vector2f fMousePos = static_cast<sf::Vector2f>(mousePos);
    if (this->analyticsPanel && this->analyticsPanel->Contains(fMousePos))
        return true;
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
    if (this->buildPanel && this->buildPanel->Contains(fMousePos))
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

void GameInterface::Update(float dt, const sf::Vector2i &mousePos, bool mouseClicked, std::vector<Tile> &map)
{
    this->nextTurnButton->Update(mousePos, mouseClicked);
    bool isLeftMouseDown = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);

    if (this->buildPanel)
    {
        BuildPanel::mapContext = &map;
        this->buildPanel->Update(mousePos, mouseClicked, this->selectedCityPtr);
    }
    if (this->analyticsPanel)
    {
        this->analyticsPanel->Update(this->selectedCityPtr, mousePos, isLeftMouseDown, mouseClicked);
    }

    if (this->cityPanel)
    {
        this->cityPanel->Update(mousePos, isLeftMouseDown, mouseClicked);
    }
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
    if (this->buildPanel)
        this->buildPanel->Draw(window);
    if (this->analyticsPanel)
        this->analyticsPanel->Draw(window);
}

void GameInterface::UpdateSelection(const Tile *tile)
{
    if (this->sidePanel)
        this->sidePanel->UpdateSelection(tile);
    if(!this->analyticsPanel->isVisible)
        this->analyticsPanel->isVisible = true;
}

void GameInterface::UpdateCitySelection(const City *city, const std::string &empireName)
{
    this->selectedCityPtr = city;

    if (this->cityPanel)
        this->cityPanel->UpdateSelection(city, empireName, this->gm);

    if (this->buildPanel)
    {
        if (city != nullptr)
        {
            this->buildPanel->SetVisible(true);
            BuildPanel::currentCityContext = city;
        }
        else
        {
            this->buildPanel->SetVisible(false);
            BuildPanel::currentCityContext = nullptr;

            this->currentInterfaceState = InterfaceState::Default;
        }
    }
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
        float targetY = this->isUnitSelected ? 450.0f : 300.0f;
    }
}