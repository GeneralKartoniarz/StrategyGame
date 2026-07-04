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
struct EconomySample
{
    float population = 0.0f;
    float treasury = 0.0f;
    float averageSatisfaction = 0.0f;
    float foodSupply = 0.0f;
};
struct City
{
    uint32_t nameID;
    int32_t centerTileID;
    int32_t ownerEmpireID;
    float money = 0.0f;
    float childSupportFund = 0.0f;
    float producedFood = 0.0f;
    std::vector<int32_t> jurisdictionTiles;
    std::vector<Workplace> workplaces;
    std::map<ResourceType, float> warehouse;
    std::vector<ConstructionTask> buildQueue;
    std::vector<EconomySample> economyHistory;
    std::map<ResourceType, float> localDemandLastTurn;
    std::map<ResourceType, float> localProductionLastTurn;
    std::map<ResourceType, float> uiLocalDemandDisplay;
    std::map<ResourceType, float> uiLocalProductionDisplay;
    constexpr static size_t MAX_HISTORY_SAMPLES = 150;
    void RecordTurnStatistics(float totalPop, float avgSat);
    void CollectWorkplacesFromTerritory(const std::vector<Tile> &map);
    void PerformEmploymentRegistry(PopManager &popManager);
    float CalculateCurrentGDP() const;
    void SimulateProduction(const std::vector<Tile> &map, std::map<ResourceType, MarketCommodity> &market);
    std::map<SocialClass, float> DistributeWages();
    void ProcessConstructionQueue(std::vector<Tile> &map);
};