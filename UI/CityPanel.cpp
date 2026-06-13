#include "CityPanel.hpp"

CityPanel::CityPanel(sf::Vector2f position, sf::Vector2f size, const sf::Font& font)
    : isVisible(false)
{
    this->background.setPosition(position);
    this->background.setSize(size);
    this->background.setFillColor(sf::Color(30, 30, 35, 220));
    this->background.setOutlineThickness(2.0f);
    this->background.setOutlineColor(sf::Color(100, 100, 105));

    this->cityNameLabel = std::make_unique<Label>(position + sf::Vector2f(10.0f, 10.0f), sf::Vector2f(size.x - 20.0f, 30.0f), font, "Miasto", 16);
    this->ownerLabel = std::make_unique<Label>(position + sf::Vector2f(10.0f, 45.0f), sf::Vector2f(size.x - 20.0f, 25.0f), font, "Właściciel: ", 12);
    this->tilesCountLabel = std::make_unique<Label>(position + sf::Vector2f(10.0f, 75.0f), sf::Vector2f(size.x - 20.0f, 25.0f), font, "Terytorium: 0", 12);
}

void CityPanel::UpdateSelection(const City* city, const std::string& empireName, const GameManager& gm)
{
    if (!city)
    {
        this->isVisible = false;
        return;
    }

    this->isVisible = true;

    std::string cityName = gm.GetCityName(city->nameID);
    this->cityNameLabel->SetText(cityName);

    this->ownerLabel->SetText("Wlasciciel: " + empireName);
    this->tilesCountLabel->SetText("Terytorium: " + std::to_string(city->jurisdictionTiles.size()) + " hexow");
}

void CityPanel::Draw(sf::RenderWindow* window)
{
    if (!this->isVisible) return;

    window->draw(this->background);
    this->cityNameLabel->Draw(window);
    this->ownerLabel->Draw(window);
    this->tilesCountLabel->Draw(window);
}

bool CityPanel::Contains(sf::Vector2f point) const
{
    return this->isVisible && this->background.getGlobalBounds().contains(point);
}
void CityPanel::SetPosition(sf::Vector2f position)
{
    this->background.setPosition(position);
    this->cityNameLabel->SetPosition(sf::Vector2f(position.x + 10.0f, position.y + 10.0f));
    this->ownerLabel->SetPosition(sf::Vector2f(position.x + 10.0f, position.y + 45.0f));
    this->tilesCountLabel->SetPosition(sf::Vector2f(position.x + 10.0f, position.y + 75.0f));
}