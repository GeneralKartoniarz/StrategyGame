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

    std::vector<dasmig::culture> allowedCultures = {
        dasmig::culture::albanian, dasmig::culture::american, dasmig::culture::austrian,
        dasmig::culture::belgian, dasmig::culture::bolivian, dasmig::culture::botswanan,
        dasmig::culture::brazilian, dasmig::culture::british, dasmig::culture::bulgarian,
        dasmig::culture::canadian, dasmig::culture::chilean, dasmig::culture::chinese,
        dasmig::culture::croatian, dasmig::culture::czech, dasmig::culture::danish,
        dasmig::culture::dutch, dasmig::culture::egyptian, dasmig::culture::estonian,
        dasmig::culture::filipino, dasmig::culture::finnish, dasmig::culture::french,
        dasmig::culture::georgian, dasmig::culture::german, dasmig::culture::greek,
        dasmig::culture::haitian, dasmig::culture::hungarian, dasmig::culture::indian,
        dasmig::culture::italian, dasmig::culture::japanese, dasmig::culture::kazakh,
        dasmig::culture::korean, dasmig::culture::lithuanian, dasmig::culture::luxembourgish,
        dasmig::culture::malaysian, dasmig::culture::mexican, dasmig::culture::moldovan,
        dasmig::culture::norwegian, dasmig::culture::polish, dasmig::culture::portuguese,
        dasmig::culture::russian, dasmig::culture::saudi, dasmig::culture::serbian,
        dasmig::culture::slovenian, dasmig::culture::southafrican, dasmig::culture::spanish,
        dasmig::culture::swedish, dasmig::culture::swiss, dasmig::culture::turkish};

    dasmig::culture wylosowana = allowedCultures[std::rand() % allowedCultures.size()];
    this->cultureRaw = static_cast<uint8_t>(wylosowana);
    std::cout << "SZUKAJ" << std::to_string(cultureRaw);
}
/*
 * [PL] METODA: AddCity / RemoveCity
 * LOGIKA: Zarządza ewidencją miast podległych władcy.
 * [EN] METHOD: AddCity / RemoveCity
 * LOGIC: Manages the registry of cities subordinate to the ruler.
 */
void Empire::AddCity(int32_t cityID)
{
    this->controlledCitiesIDs.push_back(cityID);
}
/*
 * [PL] METODA: AddCity / RemoveCity
 * LOGIKA: Zarządza ewidencją miast podległych władcy.
 * [EN] METHOD: AddCity / RemoveCity
 * LOGIC: Manages the registry of cities subordinate to the ruler.
 */
void Empire::RemoveCity(int32_t cityID)
{
    auto it = std::remove(this->controlledCitiesIDs.begin(), this->controlledCitiesIDs.end(), cityID);
    if (it != this->controlledCitiesIDs.end())
    {
        this->controlledCitiesIDs.erase(it, this->controlledCitiesIDs.end());
    }
}
/*
 * [PL] METODA: UpdatePrices
 * LOGIKA: Makroekonomiczny sterownik wolnego rynku w skali państwa (korekta 5-10%).
 * [DO ZMIANY]: Złoto musi zostać wykluczone z mechaniki zmian (standard walutowy).
 * [EN] METHOD: UpdatePrices
 * LOGIC: Macroeconomic driver of the free market on an empire scale (5-10% adjustment).
 * [TO CHANGE]: Gold must be excluded from price fluctuation (currency standard).
 */
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

        commodity.currentPrice = std::clamp(commodity.currentPrice, 0.05f, 500.0f);
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
/*
 * [PL] METODA: UpdateTurn
 * LOGIKA: Globalna pętla tury imperium. Wywołuje mechaniki dla każdego miasta.
 * POWIĄZANIA: City, PopManager.
 * [EN] METHOD: UpdateTurn
 * LOGIC: Global empire turn loop. Triggers mechanics for each subordinate city.
 * DEPENDENCIES: City, PopManager.
 */
void Empire::UpdateTurn(std::vector<Tile> &map, std::vector<City> &allCities, GameManager &gm)
{
    for (auto &[res, comm] : this->market)
        comm.supplyLastTurn = 0.0f;

    for (int32_t cityID : this->controlledCitiesIDs)
    {
        City &city = allCities[cityID];

        city.ProcessConstructionQueue(map);
        city.CollectWorkplacesFromTerritory(map);
        city.PerformEmploymentRegistry(this->popManager);
        city.SimulateProduction(map, this->market);
        //MINTING VALUE
        float mintingAmount = 150.0f; 
        city.money += mintingAmount;

        float welfareAllocationRate = 0.15f;
        city.childSupportFund = city.money * welfareAllocationRate;
        city.money -= city.childSupportFund;

        std::map<SocialClass, float> averageClassWages = city.DistributeWages();

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
            capitalTileID,
            gm,
            this->cultureRaw, 
            city, 
            averageClassWages);
    }

    this->UpdatePrices();
}