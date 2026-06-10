#pragma once
#include "GameManager.hpp"
#include "Map/Tile.hpp"
#include <vector>
#include <random>
#include <SFML/Graphics.hpp>

class PoliticalSetup 
{
public:
    static void CreateTestEmpire(GameManager& gm, const std::vector<Tile>& map);
    static void CreateEmpires(GameManager& gm, const std::vector<Tile>& map, int count);
};