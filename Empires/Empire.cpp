#include "Empire.hpp"
#include "City.hpp"
#include "../Map/Tile.hpp"
#include <algorithm>
#include <iostream>

Empire::Empire(int32_t id, const std::string &name, sf::Color color)
    : empireID(id), name(name), mapColor(color)
{
    // CENY DOMYSLNE SUROWCOW
    market[ResourceType::Grain].currentPrice = 0.8f;
    market[ResourceType::Fish].currentPrice = 1.2f;  
    market[ResourceType::Wood].currentPrice = 1.5f;
    market[ResourceType::Coal].currentPrice = 4.0f;
    market[ResourceType::LuxuryAlcohol].currentPrice = 12.0f;
    market[ResourceType::Paper].currentPrice = 15.0f;
    market[ResourceType::Gold].currentPrice = 50.0f;
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
void Empire::UpdatePrices()
{
    std::cout << "\n[GIELDA] --- Zmiany Cen Rynkowych ---" << std::endl;
    for (auto &[resource, commodity] : this->market)
    {
        float oldPrice = commodity.currentPrice;
        if (commodity.supplyLastTurn > 0.0f)
        {
            float ratio = commodity.demandLastTurn / commodity.supplyLastTurn;
            if (ratio > 1.1f)
                commodity.currentPrice *= 1.05f;
            else if (ratio < 0.9f)
                commodity.currentPrice *= 0.95f;
        }
        else if (commodity.demandLastTurn > 0.0f)
        {
            commodity.currentPrice *= 1.10f;
        }

        commodity.currentPrice = std::clamp(commodity.currentPrice, 0.5f, 500.0f);

        if (std::abs(commodity.currentPrice - oldPrice) > 0.01f)
        {
            std::cout << " -> " << MarketRegistry::GetResourceName(resource)
                      << ": " << oldPrice << " -> " << commodity.currentPrice
                      << " zl (Popyt: " << commodity.demandLastTurn << " | Podaz: " << commodity.supplyLastTurn << ")" << std::endl;
        }

        commodity.demandLastTurn = 0.0f;
        commodity.supplyLastTurn = 0.0f;
    }
}
void Empire::UpdateTurn(std::vector<Tile> &map, std::vector<City> &allCities)
{
    for (auto &[res, comm] : this->market)
        comm.supplyLastTurn = 0.0f;

    for (int32_t cityID : this->controlledCitiesIDs)
    {
        City &city = allCities[cityID];

        city.ProcessConstructionQueue(map);
        city.CollectWorkplacesFromTerritory(map);
        city.PerformEmploymentRegistry(this->popManager.GetAllPops());
        city.SimulateProduction(map);

        for (const auto &[res, amount] : city.warehouse)
        {
            this->market[res].supplyLastTurn += amount;
        }

        int32_t capitalTileID = city.centerTileID;
        this->popManager.UpdateTurn(
            city.warehouse,
            this->market,
            static_cast<uint16_t>(this->empireID),
            0,
            capitalTileID);
    }

    this->UpdatePrices();
}