#pragma once
#include <cstdint>
#include <SFML/System/Vector2.hpp>

enum class UnitType : uint8_t
{
    Settler,
    Warrior,
    Distance,
    Cavalry
};

struct Unit
{
    int32_t ID;
    int32_t ownerEmpireID;
    UnitType type;
    sf::Vector2f position;

    int32_t currentNodeID = -1;
    std::vector<int32_t> movementPath;

    int32_t currentMovementPoints;
    int32_t maxMovementPoints;
};