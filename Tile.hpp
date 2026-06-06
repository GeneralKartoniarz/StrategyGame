#pragma once
#include <vector>
#include <SFML/System/Vector2.hpp>
using namespace std;
enum class TerrainType {
    Plains,
    Water,
    Mountain,
    Desert
};

struct Tile {
    sf::Vector2f position;
    std::vector<sf::Vector2f> vertices;
    std::size_t ID;
    float size;
    int ownerID;
    TerrainType type;
    std::vector<std::size_t> neighbors;
};