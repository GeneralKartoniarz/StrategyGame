#pragma once
#include <SFML/System/Vector2.hpp>
#include <vector>

class MapGenerator {
public:
    std::vector<sf::Vector2f> GenerateSeeds(int mapWidth, int mapHeight, int cellSize);
};