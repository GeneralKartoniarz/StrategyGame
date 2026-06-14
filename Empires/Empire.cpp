#include "Empire.hpp"
#include <algorithm>


Empire::Empire(int32_t id, const std::string& name, sf::Color color)
    : empireID(id), name(name), mapColor(color)
{
}

void Empire::AddCity(int32_t cityID)
{
    this->controlledCitiesIDs.push_back(cityID);
}

void Empire::RemoveCity(int32_t cityID)
{
    auto it = std::remove(this->controlledCitiesIDs.begin(), this->controlledCitiesIDs.end(), cityID);
    if (it != this->controlledCitiesIDs.end())
    {
        this->controlledCitiesIDs.erase(it, this->controlledCitiesIDs.end());
    }
}
void Empire::UpdateTurn(std::vector<Tile>& map, const std::vector<City>& allCities)
{
    float totalFoodProduced = 0.0f;

    for (int32_t cityID : this->controlledCitiesIDs)
    {
        const City& city = allCities[cityID]; 

        for (int32_t tileID : city.jurisdictionTiles)
        {
            const Tile& tile = map[tileID];
            

            //TODO  MAKE IT MORE NATURAL???? SMTH LIKE THIS
            if (tile.terrain.biome == BiomeType::Plains) totalFoodProduced += 2.0f;
            else if (tile.terrain.biome == BiomeType::Ocean) totalFoodProduced += 1.0f;
            
            if (tile.terrain.resourceName == "Żyzna Gleba") totalFoodProduced += 3.0f;
            if (tile.terrain.resourceName == "Ławica Ryb") totalFoodProduced += 2.0f;
        }
    }

    float remainingFood = 0.0f;
    int32_t capitalTileID = !this->controlledCitiesIDs.empty() ? allCities[this->controlledCitiesIDs[0]].centerTileID : 0;

    this->popManager.UpdateTurn(
        totalFoodProduced,
        remainingFood,
        static_cast<uint16_t>(this->empireID),
        0,
        capitalTileID
    );
}