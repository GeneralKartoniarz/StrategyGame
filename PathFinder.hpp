#pragma once
#include <vector>
#include <cstdint>
#include "Map/NavigationGraph.hpp"

class Pathfinder 
{
public:
    static std::vector<int32_t> FindPath(const NavigationGraph& graph, int32_t startNodeID, int32_t targetNodeID);

private:
    static float Heuristic(sf::Vector2f a, sf::Vector2f b);
};