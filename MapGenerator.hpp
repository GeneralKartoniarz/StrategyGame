#pragma once
#include <SFML/System/Vector2.hpp>
#include <vector>
#include "Tile.hpp"
class MapGenerator {
public:
    std::vector<Tile> GenerateSeeds(int mapWidth, int mapHeight, int cellSize);
};