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
/*
 * [PL] STRUKTURA: Tile
 * LOGIKA: Podstawowa jednostka terytorialna na mapie (prowincja). Przechowuje dane
 * geograficzne (wielokąty Voronoia), klimatyczne (Terrain) oraz gospodarcze (Manufaktury).
 * POWIĄZANIA: Industry.hpp (BuildingType, Manufacture), sf::Vector2f z SFML.
 * * [EN] STRUCTURE: Tile
 * LOGIC: The basic territorial unit on the map (province). Stores geographical data
 * (Voronoi polygons), climate data (Terrain), and economic data (Manufactures).
 * DEPENDENCIES: Industry.hpp (BuildingType, Manufacture), sf::Vector2f from SFML.
 */
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

    /*
     * [PL] METODA: CanAddManufacture
     * LOGIKA: Sprawdza, czy na danym kafelku można wybudować nową manufakturę, weryfikując
     * obostrzenia biomu oraz limity poziomu/ilości budynków (z uwzględnieniem kolejki budowy).
     * [DO ZMIANY]: W nowym systemie rynkowym limity mogą opierać się na wolnej sile
     * roboczej i rentowności, a nie wyłącznie na twardych barierach liczbowych z tej funkcji.
     * * [EN] METHOD: CanAddManufacture
     * LOGIC: Checks if a new manufacture can be built on the tile, verifying biome
     * restrictions and building amount/level limits (including the construction queue).
     * [TO CHANGE]: In the new market system, limits might be based on available
     * workforce and profitability rather than just hard numerical caps.
     */
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