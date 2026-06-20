#pragma once
#include <vector>
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics.hpp>
#include "../Industry.hpp"
#include <string>
using namespace std;

enum class Elevation
{
    Water,
    Plains,
    Hills,
    Mountains
};

enum class Temperature
{
    Cold,
    Temperate,
    Hot
};

enum class Moisture
{
    Dry,
    Normal,
    Wet
};

enum class BiomeType
{
    Ocean,
    IceSheet,
    Tundra,
    Desert,
    Plains,
    Forest,
    Taiga,
    Rainforest,
    MountainPeak
};

struct Terrain
{
    float elevationNoise = 0.0f;
    float temperatureNoise = 0.0f;
    float moistureNoise = 0.0f;

    Elevation elevation;
    Temperature temperature;
    Moisture moisture;
    BiomeType biome;

    string resourceName = "Brak";
};

struct CellEdge
{
    sf::Vector2f p1;
    sf::Vector2f p2;
    int neighborID;
};

struct Tile
{
    std::vector<Manufacture> manufactures;
    sf::Vector2f position;
    std::vector<sf::Vector2f> vertices;
    std::vector<std::vector<sf::Vector2f>> subPolygons;
    std::size_t ID;
    float size;
    int ownerID;
    string name;
    std::vector<CellEdge> cellEdges;
    std::vector<sf::Vertex> provinceBorders;
    Terrain terrain;
    std::vector<std::size_t> neighbors;
    bool CanAddManufacture(BuildingType newType, const std::vector<ConstructionTask> &cityQueue = {}) const
    {
        if (!BuildingRegistry::IsBiomeAllowed(newType, this->terrain.biome))
        {
            return false;
        }
        int projectedLevel = 0;
        std::vector<BuildingType> projectedTypes;

        for (const auto &m : manufactures)
        {
            projectedTypes.push_back(m.type);
            if (m.type == newType)
                projectedLevel = m.level;
        }
        for (const auto &task : cityQueue)
        {
            if (task.targetTileID == static_cast<int32_t>(this->ID))
            {
                if (task.type == newType)
                    projectedLevel++;
                if (std::find(projectedTypes.begin(), projectedTypes.end(), task.type) == projectedTypes.end())
                {
                    projectedTypes.push_back(task.type);
                }
            }
        }
        if (projectedLevel > 0)
        {
            return projectedLevel < 5;
        }

        return projectedTypes.size() < 3;
    }
};