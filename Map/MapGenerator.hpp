#pragma once
#include <SFML/System/Vector2.hpp>
#include <vector>
#include <queue>
#include "Tile.hpp"
#include "FastNoiseLite.h"
#include "ClimateEngine.hpp"
#include "NavigationGraph.hpp"
struct TopologyNode
{
    sf::Vector2f position;
    float elevation;
    bool isWater;
    bool isRiver = false;
    std::vector<int> neighbors;
};

struct TopologyGraph
{
    std::vector<TopologyNode> nodes;
};

class MapGenerator
{
public:
    std::vector<Tile> GetMap(int mapWidth, int mapHeight, int cellSize, int iterations, ClimateConfig config = ClimateConfig());
    TopologyGraph ExtractTopology(const std::vector<Tile>& map, int mapWidth, int mapHeight, const ClimateEngine& climate);
    void GenerateRivers(TopologyGraph& graph, int numRivers);
    NavigationGraph BuildNavigationGraph(const std::vector<Tile>& map);

private:
    std::vector<Tile> MergeTiles(const std::vector<Tile> &smallTiles, int targetClusterSize, int mapWidth, int mapHeight);
    FastNoiseLite elevNoise;
    FastNoiseLite tempNoise;
    FastNoiseLite moistNoise;
    FastNoiseLite continentalNoise;
};