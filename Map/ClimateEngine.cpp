#include "ClimateEngine.hpp"
#include "NameGenerator.hpp"
#include <cmath>
#include <algorithm>

ClimateEngine::ClimateEngine(unsigned int globalSeed)
{
    elevNoise.SetFrequency(0.0040f);
    tempNoise.SetFrequency(0.0038f);
    moistNoise.SetFrequency(0.018f);
    continentalNoise.SetFrequency(0.004f);

    elevNoise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    elevNoise.SetSeed(globalSeed);

    tempNoise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    tempNoise.SetSeed(globalSeed + 1);

    moistNoise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    moistNoise.SetSeed(globalSeed + 2);

    continentalNoise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    continentalNoise.SetSeed(globalSeed + 3);

    resourceNoise.SetSeed(globalSeed + 4);
    resourceNoise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    resourceNoise.SetFrequency(0.05f);
}

float ClimateEngine::GetVertexElevation(float x, float y, float mapWidth, float mapHeight) const
{
    sf::Vector2f mapCenter(mapWidth / 2.0f, mapHeight / 2.0f);
    float maxDist = std::min(mapWidth, mapHeight) / 2.0f;
    float distToCenter = std::sqrt(std::pow(x - mapCenter.x, 2) + std::pow(y - mapCenter.y, 2));
    float falloff = std::pow(distToCenter / maxDist, 2.5f);

    float continental = continentalNoise.GetNoise(x, y);
    float detailElevation = elevNoise.GetNoise(x, y);

    return continental + (detailElevation * 0.4f) - falloff;
}

void ClimateEngine::ApplyClimate(std::vector<Tile> &map, int mapWidth, int mapHeight, const ClimateConfig &config)
{
    sf::Vector2f mapCenter(mapWidth / 2.0f, mapHeight / 2.0f);
    float maxDist = std::min(mapWidth, mapHeight) / 2.0f;

    std::vector<ResourceConfig> possibleResources = {
        {1, "Ruda Żelaza", 0.6f, (BiomeFlag::MountainPeak | BiomeFlag::Tundra | BiomeFlag::Taiga)},
        {2, "Złoto", 0.8f, (BiomeFlag::MountainPeak | BiomeFlag::Desert | BiomeFlag::Rainforest)},
        {3, "Żyzna Gleba", 0.5f, (BiomeFlag::Plains | BiomeFlag::Forest)},
        {4, "Wieloryby", 0.75f, BiomeFlag::Ocean},
        {5, "Perły", 0.85f, BiomeFlag::Ocean},
        {6, "Ławica Ryb", 0.4f, BiomeFlag::Ocean},
        {7, "Zwierzęta Futerkowe", 0.5f, (BiomeFlag::Tundra | BiomeFlag::Taiga)},
        {8, "Kakao", 0.4f, BiomeFlag::Rainforest},
        {9, "Ropa", 0.2f, (BiomeFlag::Desert | BiomeFlag::Tundra | BiomeFlag::Taiga)},
        {10, "Węgiel", 0.6f, (BiomeFlag::MountainPeak | BiomeFlag::Tundra | BiomeFlag::Taiga | BiomeFlag::Plains | BiomeFlag::Forest)},
        {11, "Konie", 0.6f, (BiomeFlag::Plains | BiomeFlag::Forest)},
        {12, "Uran", 0.9f, (BiomeFlag::MountainPeak | BiomeFlag::Tundra | BiomeFlag::Taiga | BiomeFlag::Forest | BiomeFlag::Desert | BiomeFlag::Plains)}

    };

    for (auto &tile : map)
    {
        float distToCenter = std::sqrt(std::pow(tile.position.x - mapCenter.x, 2) + std::pow(tile.position.y - mapCenter.y, 2));
        float falloff = std::pow(distToCenter / maxDist, 2.5f);

        float continental = continentalNoise.GetNoise(tile.position.x, tile.position.y);
        float detailElevation = elevNoise.GetNoise(tile.position.x, tile.position.y);
        float combinedElev = continental + (detailElevation * 0.4f);

        tile.terrain.elevationNoise = combinedElev - falloff;
        tile.terrain.temperatureNoise = tempNoise.GetNoise(tile.position.x, tile.position.y);
        tile.terrain.moistureNoise = moistNoise.GetNoise(tile.position.x, tile.position.y);

        tile.terrain.elevation = GetElevation(tile.terrain.elevationNoise, config);
        tile.terrain.temperature = GetTemperature(tile.terrain.temperatureNoise, config);
        tile.terrain.moisture = GetMoisture(tile.terrain.moistureNoise, config);

        tile.terrain.biome = DetermineBiome(tile.terrain.elevation, tile.terrain.temperature, tile.terrain.moisture);

        tile.name = (tile.terrain.biome == BiomeType::Ocean) ? "Bezkresny Ocean" : NameGenerator::GetRandomName();
        tile.terrain.resourceName = "Brak";

        BiomeFlag currentTileFlag = GetBiomeFlag(tile.terrain.biome);
        float resValue = (resourceNoise.GetNoise(tile.position.x, tile.position.y) + 1.0f) / 2.0f;

        for (const auto &res : possibleResources)
        {
            if ((static_cast<uint32_t>(currentTileFlag) & static_cast<uint32_t>(res.allowedBiomesMask)) != 0)
            {
                if (resValue > res.baseSpawnChance)
                {
                    tile.terrain.resourceName = res.name;
                    break;
                }
            }
        }
    }
}

Elevation ClimateEngine::GetElevation(float noise, const ClimateConfig &config) const
{
    if (noise < config.waterThreshold)
        return Elevation::Water;
    if (noise < config.plainsThreshold)
        return Elevation::Plains;
    if (noise < config.hillsThreshold)
        return Elevation::Hills;
    return Elevation::Mountains;
}

Temperature ClimateEngine::GetTemperature(float noise, const ClimateConfig &config) const
{
    if (noise < config.coldThreshold)
        return Temperature::Cold;
    if (noise < config.temperateThreshold)
        return Temperature::Temperate;
    return Temperature::Hot;
}

Moisture ClimateEngine::GetMoisture(float noise, const ClimateConfig &config) const
{
    if (noise < config.dryThreshold)
        return Moisture::Dry;
    if (noise < config.normalThreshold)
        return Moisture::Normal;
    return Moisture::Wet;
}

BiomeType ClimateEngine::DetermineBiome(Elevation elev, Temperature temp, Moisture moist) const
{
    if (elev == Elevation::Water)
        return BiomeType::Ocean;
    if (elev == Elevation::Mountains)
        return BiomeType::MountainPeak;

    switch (temp)
    {
    case Temperature::Cold:
        if (moist == Moisture::Dry)
            return BiomeType::IceSheet;
        return BiomeType::Tundra;
    case Temperature::Temperate:
        if (moist == Moisture::Dry)
            return BiomeType::Plains;
        if (moist == Moisture::Normal)
            return BiomeType::Forest;
        return BiomeType::Taiga;
    case Temperature::Hot:
        if (moist == Moisture::Dry)
            return BiomeType::Desert;
        if (moist == Moisture::Normal)
            return BiomeType::Plains;
        return BiomeType::Rainforest;
    }
    return BiomeType::Plains;
}

BiomeFlag ClimateEngine::GetBiomeFlag(BiomeType type) const
{
    switch (type)
    {
    case BiomeType::Ocean:
        return BiomeFlag::Ocean;
    case BiomeType::IceSheet:
        return BiomeFlag::IceSheet;
    case BiomeType::Tundra:
        return BiomeFlag::Tundra;
    case BiomeType::Taiga:
        return BiomeFlag::Taiga;
    case BiomeType::Forest:
        return BiomeFlag::Forest;
    case BiomeType::Rainforest:
        return BiomeFlag::Rainforest;
    case BiomeType::Plains:
        return BiomeFlag::Plains;
    case BiomeType::Desert:
        return BiomeFlag::Desert;
    case BiomeType::MountainPeak:
        return BiomeFlag::MountainPeak;
    default:
        return BiomeFlag::None;
    }
}