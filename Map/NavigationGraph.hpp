#pragma once
#include <vector>
#include <SFML/System/Vector2.hpp>
#include <cstdint>

struct NavNode 
{
    sf::Vector2f position;
    std::vector<int32_t> connectedNodes;
    
    float gCost = 0.0f;
    float hCost = 0.0f; 
    int32_t parentID = -1; 
    
    float GetFCost() const { return gCost + hCost; }
};

struct NavigationGraph
{
    std::vector<NavNode> nodes;
};