#pragma once
#include <SFML/System/Vector2.hpp>
#include "jc_voronoi.h"
#include <vector>
#include "Tile.hpp"
class MapGenerator
{
public:
    sf::Vector2f CalculateCentroid(const jcv_site *site);
    std::vector<Tile> GetMap(int mapWidth, int mapHeight, int cellSize, int iterations);
    std::vector<jcv_point> InitializeSeeds(int mapWidth, int mapHeight, int cellSize);
    std::vector<Tile> CreateTiles(const std::vector<jcv_point> &points, int mapWidth, int mapHeight);
};