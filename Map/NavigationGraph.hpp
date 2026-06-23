#pragma once
#include <vector>
#include <SFML/System/Vector2.hpp>
#include <cstdint>
/*
 * [PL] STRUKTURA: NavNode
 * LOGIKA: Specjalistyczny węzeł wykorzystywany przez algorytmy szukania drogi (A* lub Dijkstra). 
 * Przechowuje fizyczne koszty ruchu (gCost, hCost) i listę połączonych identyfikatorów.
 * * [EN] STRUCTURE: NavNode
 * LOGIC: A specialized node used by pathfinding algorithms (A* or Dijkstra). 
 * Stores physical movement costs (gCost, hCost) and a list of connected node IDs.
 */
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