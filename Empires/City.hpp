#pragma once
#include <cstdint>
#include <vector>
#include <map>
#include "../Economy.hpp"
#include "../Industry.hpp"

struct Tile;
struct Pop;


struct Workplace
{
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
    
    std::vector<int32_t> jurisdictionTiles;
    std::vector<Workplace> workplaces;
    std::map<ResourceType, float> warehouse;
    
    std::vector<ConstructionTask> buildQueue;

    void CollectWorkplacesFromTerritory(const std::vector<Tile>& map);
    void PerformEmploymentRegistry(const std::vector<Pop>& empirePops);
    void SimulateProduction(const std::vector<Tile>& map);
    
    void ProcessConstructionQueue(std::vector<Tile>& map);
};