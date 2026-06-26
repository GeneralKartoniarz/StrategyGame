#pragma once
#include <cstdint>
#include <vector>
#include <map>
#include "../Economy.hpp"
#include "../Industry.hpp"

struct Tile;
struct Pop;
class PopManager;

struct Workplace
{
    float revenuePool = 0.0f;
    ResourceType producedResource;
    uint8_t requiredClassRaw;
    int32_t maxEmployees;
    int32_t currentEmployees;
};

struct City
{
    uint32_t nameID;
    int32_t centerTileID;
    int32_t ownerEmpireID;
    float money = 0.0f;
    float childSupportFund = 0.0f;
    std::vector<int32_t> jurisdictionTiles;
    std::vector<Workplace> workplaces;
    std::map<ResourceType, float> warehouse;
    std::vector<ConstructionTask> buildQueue;

    void CollectWorkplacesFromTerritory(const std::vector<Tile> &map);
    void PerformEmploymentRegistry(PopManager &popManager);
    void SimulateProduction(const std::vector<Tile> &map, std::map<ResourceType, MarketCommodity> &market);
    std::map<SocialClass, float> DistributeWages();
    void ProcessConstructionQueue(std::vector<Tile> &map);
};