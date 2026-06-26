#include "CityPanel.hpp"
#include <algorithm>

CityPanel::CityPanel(sf::Vector2f position, sf::Vector2f size, const sf::Font &font)
    : isVisible(false)
{
    if (size.x < 460.0f) size.x = 460.0f;
    if (size.y < 220.0f) size.y = 220.0f;

    this->background.setFillColor(sf::Color(25, 25, 30, 240));
    this->background.setOutlineThickness(1.0f);
    this->background.setOutlineColor(sf::Color(80, 80, 85));
    this->background.setSize(size);

    float margin = 10.0f;
    float spacing = 10.0f;
    float fullWidth = size.x - (2.0f * margin);
    float halfWidth = (fullWidth - spacing) / 2.0f;

    this->cityNameLabel = std::make_unique<Label>(position, sf::Vector2f(fullWidth, 30.0f), font, "Miasto", 16);
    this->ownerLabel = std::make_unique<Label>(position, sf::Vector2f(halfWidth, 25.0f), font, "Władca: ", 12);
    this->totalPopLabel = std::make_unique<Label>(position, sf::Vector2f(halfWidth, 25.0f), font, "Ludność: 0", 12);
    this->tilesCountLabel = std::make_unique<Label>(position, sf::Vector2f(halfWidth, 25.0f), font, "Prowincje: 0", 12);
    this->satisfactionLabel = std::make_unique<Label>(position, sf::Vector2f(halfWidth, 25.0f), font, "Zadowolenie: 100%", 12);
    this->treasuryLabel = std::make_unique<Label>(position, sf::Vector2f(halfWidth, 25.0f), font, "Skarbiec: 0 $", 12);
    this->unemploymentLabel = std::make_unique<Label>(position, sf::Vector2f(halfWidth, 25.0f), font, "Bezrobocie: 0%", 12);
    this->ClassDistributionLabel = std::make_unique<Label>(position, sf::Vector2f(fullWidth, 50.0f), font, "Struktura...", 11);

    this->SetPosition(position);
}

void CityPanel::SetPosition(sf::Vector2f position)
{
    this->background.setPosition(position);
    
    float margin = 10.0f;
    float spacing = 10.0f;
    float fullWidth = this->background.getSize().x - (2.0f * margin);
    float halfWidth = (fullWidth - spacing) / 2.0f;

    float leftX = position.x + margin;
    float rightX = position.x + margin + halfWidth + spacing;

    this->cityNameLabel->SetPosition(sf::Vector2f(leftX, position.y + 10.0f));
    this->ownerLabel->SetPosition(sf::Vector2f(leftX, position.y + 45.0f));
    this->totalPopLabel->SetPosition(sf::Vector2f(rightX, position.y + 45.0f));
    
    this->tilesCountLabel->SetPosition(sf::Vector2f(leftX, position.y + 75.0f));
    this->satisfactionLabel->SetPosition(sf::Vector2f(rightX, position.y + 75.0f));
    
    this->treasuryLabel->SetPosition(sf::Vector2f(leftX, position.y + 105.0f));
    this->unemploymentLabel->SetPosition(sf::Vector2f(rightX, position.y + 105.0f));

    this->ClassDistributionLabel->SetPosition(sf::Vector2f(leftX, position.y + 135.0f));
}

void CityPanel::UpdateCityData(const City &city, const PopManager &popMgr)
{
    std::vector<const Pop *> cityPops;
    const auto &allEmpirePops = popMgr.GetAllPops();

    for (const auto &pop : allEmpirePops)
    {
        auto it = std::find(city.jurisdictionTiles.begin(), city.jurisdictionTiles.end(), pop.locationTileID);
        if (it != city.jurisdictionTiles.end())
        {
            cityPops.push_back(&pop);
        }
    }

    size_t totalPeopleCount = cityPops.size() * 10;
    this->totalPopLabel->SetText("Ludn.: " + std::to_string(totalPeopleCount));

    float avgSat = popMgr.GetAverageSatisfaction(cityPops);
    int satisfactionPercent = static_cast<int>((avgSat / 255.0f) * 100.0f);
    this->satisfactionLabel->SetText("Zadow.: " + std::to_string(satisfactionPercent) + "%");

    this->treasuryLabel->SetText("Skarbiec: " + std::to_string(static_cast<int>(city.money)) + " $");

    int32_t bound = 0, laborers = 0, specialists = 0, capitalists = 0, elite = 0;
    int32_t laborForce = 0;
    int32_t employedCount = 0;

    for (const auto *popPtr : cityPops)
    {
        if (popPtr->age >= 16 && !popPtr->IsFemale())
        {
            laborForce++;
            if (popPtr->IsEmployed()) employedCount++;
        }

        switch (popPtr->socialClass)
        {
            case SocialClass::Bound:      bound++; break;
            case SocialClass::Laborer:    laborers++; break;
            case SocialClass::Specialist: specialists++; break;
            case SocialClass::Capitalist: capitalists++; break;
            case SocialClass::Elite:      elite++; break;
            default: break;
        }
    }

    int unemploymentRate = laborForce > 0 ? static_cast<int>((1.0f - (static_cast<float>(employedCount) / laborForce)) * 100.0f) : 0;
    this->unemploymentLabel->SetText("Bezrobocie: " + std::to_string(unemploymentRate) + "%");

    std::string distStr = "Zwiazani: " + std::to_string(bound) + " | Robotnicy: " + std::to_string(laborers) + " | Specjaliści: " + std::to_string(specialists) + "\n" +
                          "Posiadacze: " + std::to_string(capitalists) + " | Elita: " + std::to_string(elite);
    this->ClassDistributionLabel->SetText(distStr);
}

void CityPanel::Draw(sf::RenderWindow *window)
{
    if (!this->isVisible) return;   
    sf::View oldView = window->getView();
    window->setView(window->getDefaultView());

    window->draw(this->background);
    this->cityNameLabel->Draw(window);
    this->ownerLabel->Draw(window);
    this->tilesCountLabel->Draw(window);
    this->totalPopLabel->Draw(window);
    this->satisfactionLabel->Draw(window);
    this->treasuryLabel->Draw(window);
    this->unemploymentLabel->Draw(window);
    this->ClassDistributionLabel->Draw(window);

    window->setView(oldView);
}
void CityPanel::UpdateSelection(const City *city, const std::string &empireName, const GameManager &gm)
{
    if (!city)
    {
        this->isVisible = false;
        return;
    }

    this->isVisible = true;
    this->cityNameLabel->SetText(gm.GetCityName(city->nameID));
    this->ownerLabel->SetText("Władca: " + empireName);
    this->tilesCountLabel->SetText("Prowincje: " + std::to_string(city->jurisdictionTiles.size()));
}
bool CityPanel::Contains(sf::Vector2f point) const
{
    return this->isVisible && this->background.getGlobalBounds().contains(point);
}