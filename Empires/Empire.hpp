#pragma once
#include <string>
#include <vector>
#include <map>
#include <SFML/Graphics.hpp>
#include "PopManager.hpp"

struct City;
struct Tile;

class Empire
{
public:
    Empire(int32_t id, const std::string &name, sf::Color color);
    ~Empire() = default;

    int32_t GetID() const { return empireID; }
    sf::Color GetColor() const { return mapColor; }
    const std::string &GetName() const { return name; }

    PopManager &GetPopManager() { return popManager; }

    void AddCity(int32_t cityID);
    void RemoveCity(int32_t cityID);
    const std::vector<int32_t> &GetCities() const { return controlledCitiesIDs; }

    void UpdateTurn(std::vector<Tile> &map, std::vector<City> &allCities);
    void AddUnit(int32_t unitID) { controlledUnitsIDs.push_back(unitID); }
    void UpdatePrices();
private:
    int32_t empireID;
    std::string name;
    sf::Color mapColor;

    PopManager popManager;
    std::vector<int32_t> controlledCitiesIDs;
    std::vector<int32_t> controlledUnitsIDs;

    std::map<ResourceType, MarketCommodity> market;
};