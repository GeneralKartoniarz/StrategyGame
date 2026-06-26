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

    this->sectionTitle = std::make_unique<Label>(
        position + sf::Vector2f(10.0f, 10.0f),
        sf::Vector2f(size.x - 20.0f, 25.0f),
        font, "PRODUKCJA MIEJSKA", 12);

    this->costTrackerLabel = std::make_unique<Label>(
        position + sf::Vector2f(10.0f, 40.0f),
        sf::Vector2f(size.x - 20.0f, 25.0f),
        font, "Zasoby: Zboze: 0 | Drewno: 0", 11);

    this->buildButtons.push_back(std::make_unique<Button>(
        position + sf::Vector2f(10.0f, 75.0f),
        sf::Vector2f(130.0f, 35.0f),
        font, "FARMA (10 W)", &BuildPanel::OnBuildFarmClick));

    this->buildButtons.push_back(std::make_unique<Button>(
        position + sf::Vector2f(150.0f, 75.0f),
        sf::Vector2f(130.0f, 35.0f),
        font, "OSADNIK (200 Z)", &BuildPanel::OnRecruitSettlerClick));
}

void BuildPanel::Update(const sf::Vector2i &mousePos, bool mouseClicked, const City *activeCity)
{
    if (!this->isVisible || !activeCity)
        return;

    std::string info = "Spichlerz -> Zboze: " + std::to_string(static_cast<int>(activeCity->warehouse.at(ResourceType::Grain))) +
                       " | Drewno: " + std::to_string(static_cast<int>(activeCity->warehouse.at(ResourceType::Wood)));
    this->costTrackerLabel->SetText(info);

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
    this->sectionTitle->Draw(window);
    this->costTrackerLabel->Draw(window);

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

void BuildPanel::OnRecruitSettlerClick()
{
    if (currentCityContext && gameManagerContext && mapContext)
    {
        int32_t cityTileID = currentCityContext->centerTileID;

        int32_t realCityVectorIndex = -1;
        const auto& allCities = gameManagerContext->GetAllCities();
        
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