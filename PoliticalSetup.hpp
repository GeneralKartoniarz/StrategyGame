#pragma once
#include "GameManager.hpp"
#include "Map/Tile.hpp"
#include <vector>

class PoliticalSetup 
{
public:
    static void CreateTestEmpire(GameManager& gm, const std::vector<Tile>& map);
};