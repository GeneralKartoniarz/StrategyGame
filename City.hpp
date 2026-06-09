#pragma once
#include <vector>
#include <cstdint>

struct City 
{
    uint32_t nameID;
    int32_t centerTileID;
    int32_t ownerEmpireID;
    std::vector<int32_t> jurisdictionTiles;
};