#include "SidePanel.hpp"
#include "../Industry.hpp"
SidePanel::SidePanel(const sf::Vector2f& position, const sf::Vector2f& size, const sf::Font& font)
    : nameText(font), idText(font), biomeText(font) 
{
    this->background.setPosition(position);
    this->background.setSize(size);
    this->background.setFillColor(sf::Color(25, 25, 25, 230));
    this->background.setOutlineThickness(2.0f);
    this->background.setOutlineColor(sf::Color(180, 160, 110));

    auto setupText = [](sf::Text& t, const sf::Vector2f& pos) {
        t.setCharacterSize(16);
        t.setFillColor(sf::Color(230, 230, 230));
        t.setPosition(pos);
    };

    setupText(this->nameText, {position.x + 15.0f, position.y + 15.0f});
    setupText(this->idText, {position.x + 15.0f, position.y + 60.0f});
    setupText(this->biomeText, {position.x + 15.0f, position.y + 90.0f});

    this->UpdateSelection(nullptr);
}

std::string SidePanel::BiomeToString(BiomeType biome)
{
    switch (biome)
    {
        case BiomeType::Ocean:        return "Głęboki Ocean";
        case BiomeType::IceSheet:     return "Ladolod";
        case BiomeType::Tundra:       return "Zimna Tundra";
        case BiomeType::Desert:       return "Spalona Pustynia";
        case BiomeType::Plains:       return "Rozlegle Rowniny";
        case BiomeType::Forest:       return "Gesty Las";
        case BiomeType::Taiga:        return "Surowa Tajga";
        case BiomeType::Rainforest:   return "Parujaca Dzungla";
        case BiomeType::MountainPeak: return "Gorskie Szczyty";
        default:                      return "Nieznane Pustkowie";
    }
}

void SidePanel::UpdateSelection(const Tile* tile)
{
    if (tile)
    {
        std::string nameStr = "Prowincja:\n" + tile->name;
        this->nameText.setString(sf::String::fromUtf8(nameStr.begin(), nameStr.end()));

        this->idText.setString("ID: " + std::to_string(tile->ID));
        
        std::string biomeStr = "Teren: " + BiomeToString(tile->terrain.biome) + "\nSurowiec: "+ tile->terrain.resourceName;
        
        biomeStr += "\n\n--- Infrastruktura ---\n";
        if (tile->manufactures.empty())
        {
            biomeStr += "Brak zabudowan";
        }
        else
        {
            for (const auto& m : tile->manufactures)
            {
                biomeStr += "- " + BuildingRegistry::GetBuildingName(m.type) + " (Poz. " + std::to_string(m.level) + ")\n";
            }
        }
        
        this->biomeText.setString(sf::String::fromUtf8(biomeStr.begin(), biomeStr.end()));
    }
    else
    {
        this->nameText.setString("Prowincja:\nBrak zaznaczenia");
        this->idText.setString("ID: ---");
        this->biomeText.setString("Teren: ---");
    }
}

void SidePanel::Draw(sf::RenderWindow* window)
{
    window->draw(this->background);
    window->draw(this->nameText);
    window->draw(this->idText);
    window->draw(this->biomeText);
}

bool SidePanel::Contains(const sf::Vector2f& point) const
{
    return this->background.getGlobalBounds().contains(point);
}