#include "Empire.hpp"
#include "City.hpp"
#include "../Map/Tile.hpp"
#include <algorithm>
#include <iostream>

Empire::Empire(int32_t id, const std::string& name, sf::Color color)
    : empireID(id), name(name), mapColor(color)
{
    marketPrices[ResourceType::Grain] = 2.0f;
    marketPrices[ResourceType::Fish] = 3.0f;
    marketPrices[ResourceType::Wood] = 1.5f;
    marketPrices[ResourceType::Coal] = 4.0f;
    marketPrices[ResourceType::LuxuryAlcohol] = 12.0f;
    marketPrices[ResourceType::Paper] = 15.0f;
    marketPrices[ResourceType::Gold] = 50.0f;
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

void Empire::UpdateTurn(std::vector<Tile>& map, std::vector<City>& allCities)
{
    for (int32_t cityID : this->controlledCitiesIDs)
    {
        City& city = allCities[cityID];

        city.ProcessConstructionQueue(map);

        city.CollectWorkplacesFromTerritory(map);
        city.PerformEmploymentRegistry(this->popManager.GetAllPops());
        city.SimulateProduction(map);

        int32_t capitalTileID = city.centerTileID;
        this->popManager.UpdateTurn(
            city.warehouse,
            this->marketPrices,
            static_cast<uint16_t>(this->empireID),
            0,
            capitalTileID
        );
    }
}