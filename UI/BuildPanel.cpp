#include "BuildPanel.hpp"
#include "Empires/City.hpp"
#include "../GameManager.hpp"

#include <iostream>
#include <memory>
#include <string>

const City *BuildPanel::currentCityContext = nullptr;
GameManager *BuildPanel::gameManagerContext = nullptr;
const std::vector<Tile> *BuildPanel::mapContext = nullptr;
BuildPanel::BuildPanel(const sf::Vector2f &position, const sf::Vector2f &size, const sf::Font &font)
    : panelFont(font)
{
    this->background.setPosition(position);
    this->background.setSize(size);
    this->background.setFillColor(sf::Color(25, 25, 30, 240));
    this->background.setOutlineThickness(1.5f);
    this->background.setOutlineColor(sf::Color(180, 160, 110));

    float contentWidth = 270.0f;        
    float buttonHeight = 35.0f;         
    float gapX = 10.0f;                 
    float gapY = 10.0f;                 
    float contentHeight = (buttonHeight * 2.0f) + gapY; 
    float startX = (size.x - contentWidth) / 2.0f;
    float startY = (size.y - contentHeight) / 2.0f;
    this->buildButtons.push_back(std::make_unique<Button>(
        position + sf::Vector2f(startX, startY),
        sf::Vector2f(130.0f, buttonHeight),
        font, "FARMA (10 W)", &BuildPanel::OnBuildFarmClick));
    this->buildButtons.push_back(std::make_unique<Button>(
        position + sf::Vector2f(startX + 130.0f + gapX, startY),
        sf::Vector2f(130.0f, buttonHeight),
        font, "TARTAK (10 W)", &BuildPanel::OnBuildSawMillClick));
    this->buildButtons.push_back(std::make_unique<Button>(
        position + sf::Vector2f(startX, startY + buttonHeight + gapY),
        sf::Vector2f(270.0f, buttonHeight),
        font, "OSADNIK (200 Z)", &BuildPanel::OnRecruitSettlerClick));
}

void BuildPanel::Update(const sf::Vector2i &mousePos, bool mouseClicked, const City *activeCity)
{
    if (!this->isVisible || !activeCity)
        return;
    for (auto &btn : this->buildButtons)
    {
        btn->Update(mousePos, mouseClicked);
    }
}

void BuildPanel::Draw(sf::RenderWindow *window)
{
    if (!this->isVisible)
        return;

    sf::View oldView = window->getView();
    window->setView(window->getDefaultView());

    window->draw(this->background);
    for (auto &btn : this->buildButtons)
    {
        btn->Draw(window);
    }

    window->setView(oldView);
}

bool BuildPanel::Contains(const sf::Vector2f &point) const
{
    return this->isVisible && this->background.getGlobalBounds().contains(point);
}

void BuildPanel::OnBuildFarmClick()
{
    if (GameInterface::GetInstance())
    {
        GameInterface::GetInstance()->currentInterfaceState = InterfaceState::PlacingBuilding;
        GameInterface::GetInstance()->buildingUnderCursor = BuildingType::Farm;

        std::cout << "[INTERFEJS] Wybierz kafelek jurysdykcji dla Farmy..." << std::endl;
    }
}
void BuildPanel::OnBuildSawMillClick()
{
    if (GameInterface::GetInstance())
    {
        GameInterface::GetInstance()->currentInterfaceState = InterfaceState::PlacingBuilding;
        GameInterface::GetInstance()->buildingUnderCursor = BuildingType::SawMill;

        std::cout << "[INTERFEJS] Wybierz kafelek jurysdykcji dla Tartaka..." << std::endl;
    }
}
void BuildPanel::OnRecruitSettlerClick()
{
    if (currentCityContext && gameManagerContext && mapContext)
    {
        int32_t cityTileID = currentCityContext->centerTileID;

        int32_t realCityVectorIndex = -1;
        const auto &allCities = gameManagerContext->GetAllCities();

        for (size_t i = 0; i < allCities.size(); ++i)
        {
            if (allCities[i].centerTileID == cityTileID)
            {
                realCityVectorIndex = static_cast<int32_t>(i);
                break;
            }
        }

        if (realCityVectorIndex != -1)
        {
            if (GameInterface::GetInstance())
            {
                GameInterface::GetInstance()->UpdateCitySelection(nullptr, "");
            }
            currentCityContext = nullptr;

            bool success = gameManagerContext->RecruitSettler(realCityVectorIndex, *mapContext);

            if (success)
            {
                std::cout << "[UI] Sukces: Osadnik sformowany, interfejs miasta zresetowany." << std::endl;
            }
            else
            {
                std::cout << "[UI] Backend odrzucił werbunek." << std::endl;
            }
        }
    }
}