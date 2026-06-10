#pragma once
#include <vector>
#include <SFML/System/Vector2.hpp>
#include "Tile.hpp"

class VoronoiBuilder 
{
public:
    static std::vector<Tile> GenerateRawGrid(int mapWidth, int mapHeight, int cellSize, int iterations);

private:
    static std::vector<sf::Vector2f> InitializeSeeds(int mapWidth, int mapHeight, int cellSize);
    static void ApplyLloydRelaxation(std::vector<sf::Vector2f>& points, int mapWidth, int mapHeight, int iterations);
    static std::vector<Tile> CreateTiles(const std::vector<sf::Vector2f>& points, int mapWidth, int mapHeight);
};