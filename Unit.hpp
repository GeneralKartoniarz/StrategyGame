#pragma once
#include <cstdint>
#include <SFML/System/Vector2.hpp>
#include <optional>
#include <vector>

class GameManager;

struct ColonizationComponent {
    void FoundCity(GameManager& gm, int32_t tileID);
};
struct CombatComponent {
    int32_t attackPower = 20;
    int32_t defensePower = 15;
};
enum class UnitType : uint8_t
{
    Settler,
    Warrior,
    Distance,
    Cavalry
};

class Unit {
public:
    int32_t ID = -1;
    int32_t ownerEmpireID = -1;
    UnitType type;
    
    sf::Vector2f position;
    
    int32_t currentNodeID = -1;
    int32_t nextNodeID = -1;
    std::vector<int32_t> movementPath;
    
    int32_t maxMovementPoints = 3;
    int32_t currentMovementPoints = 3;

    std::optional<ColonizationComponent> colonization;
    std::optional<CombatComponent> combat;
};