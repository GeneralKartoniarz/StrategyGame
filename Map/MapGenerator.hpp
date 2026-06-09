#pragma once
#include <SFML/System/Vector2.hpp>

#define JCV_REAL_TYPE double
#include "jc_voronoi.h"

#include <vector>
#include <queue>
#include "Tile.hpp"
#include "FastNoiseLite.h"
struct ClimateConfig
{
    float waterThreshold = -0.4f;
    float plainsThreshold = 0.2f;
    float hillsThreshold = 0.6f;

    float coldThreshold = -0.2f;
    float temperateThreshold = 0.2f;

    float dryThreshold = -0.3f;
    float normalThreshold = 0.0f; 
};
class MapGenerator
{
public:
    sf::Vector2f CalculateCentroid(const jcv_site *site);
    std::vector<Tile> GetMap(int mapWidth, int mapHeight, int cellSize, int iterations, ClimateConfig config = ClimateConfig());
    std::vector<jcv_point> InitializeSeeds(int mapWidth, int mapHeight, int cellSize);
    std::vector<Tile> CreateTiles(const std::vector<jcv_point> &points, int mapWidth, int mapHeight);

private:
    std::vector<Tile> MergeTiles(const std::vector<Tile> &smallTiles, int targetClusterSize, int mapWidth, int mapHeight, const ClimateConfig &config);
    FastNoiseLite elevNoise;
    FastNoiseLite tempNoise;
    FastNoiseLite moistNoise;
    FastNoiseLite continentalNoise;
};