#pragma once
#include "Tile.hpp"
#include <vector>
#include <string>
#include "FastNoiseLite.h"

struct ClimateConfig {
    float waterThreshold = -0.5f;
    float plainsThreshold = 0.3f;
    float hillsThreshold = 0.75f;
    float coldThreshold = -0.65f;
    float temperateThreshold = 0.15f;
    float dryThreshold = -0.45f;
    float normalThreshold = 0.15f;
};

enum class BiomeFlag : uint32_t 
{
    None         = 0,
    Ocean        = 1 << 0,
    IceSheet     = 1 << 1,
    Tundra       = 1 << 2,
    Taiga        = 1 << 3,
    Forest       = 1 << 4,
    Rainforest   = 1 << 5,
    Plains       = 1 << 6,
    Desert       = 1 << 7,
    MountainPeak = 1 << 8
};

inline BiomeFlag operator|(BiomeFlag a, BiomeFlag b) {
    return static_cast<BiomeFlag>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
}

struct ResourceConfig 
{
    int resourceID;
    std::string name;
    float baseSpawnChance;
    BiomeFlag allowedBiomesMask; 
};

class ClimateEngine 
{
public:
    ClimateEngine(unsigned int globalSeed);
    void ApplyClimate(std::vector<Tile>& map, int mapWidth, int mapHeight, const ClimateConfig& config);
    float GetVertexElevation(float x, float y, float mapWidth, float mapHeight) const;

private:
    FastNoiseLite elevNoise;
    FastNoiseLite tempNoise;
    FastNoiseLite moistNoise;
    FastNoiseLite continentalNoise;
    FastNoiseLite resourceNoise;

    Elevation GetElevation(float noise, const ClimateConfig &config) const;
    Temperature GetTemperature(float noise, const ClimateConfig &config) const;
    Moisture GetMoisture(float noise, const ClimateConfig &config) const;
    BiomeType DetermineBiome(Elevation elev, Temperature temp, Moisture moist) const;
    BiomeFlag GetBiomeFlag(BiomeType type) const;
};