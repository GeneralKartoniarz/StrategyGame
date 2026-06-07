#pragma once
#include <vector>
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics.hpp>
#include <string>
using namespace std;

enum class Elevation {
    Water,
    Plains,
    Hills,
    Mountains
};

enum class Temperature {
    Cold,
    Temperate,
    Hot
};

enum class Moisture {
    Dry,
    Normal,
    Wet
};

enum class BiomeType {
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

struct Terrain {
    float elevationNoise = 0.0f;
    float temperatureNoise = 0.0f;
    float moistureNoise = 0.0f;

    Elevation elevation;
    Temperature temperature;
    Moisture moisture;
    BiomeType biome;
};

struct CellEdge
{
    sf::Vector2f p1;
    sf::Vector2f p2;
    int neighborID;
};

struct Tile
{
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
};