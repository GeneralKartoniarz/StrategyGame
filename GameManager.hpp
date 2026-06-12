#pragma once
#include <vector>
#include <cstdint>
#include "City.hpp"
#include "Empires/Empire.hpp"
#include "Map/NavigationGraph.hpp"
#include "Unit.hpp"

struct Tile;

class GameManager
{
public:
    GameManager() = default;
    ~GameManager() = default;

    void AddCity(const City &city);
    void AddEmpire(const Empire &empire);

    City &GetCity(int32_t id);
    const Empire &GetEmpire(int32_t id) const;
    const std::vector<City> &GetAllCities() const { return cities; }
    const std::vector<Empire> &GetAllEmpires() const { return empires; }

    void AddUnit(const Unit &unit) { units.push_back(unit); }
    const std::vector<Unit> &GetAllUnits() const { return units; }

    int32_t GetNearestNodeID(sf::Vector2f worldPos) const;

    void SetNavGraph(const NavigationGraph &graph) { navGraph = graph; }
    NavigationGraph &GetNavGraph() { return navGraph; }

    Unit &GetUnit(int32_t id);

    void UpdateUnits(float dt);
    void ResetMovementPoints();

    bool CanFoundCity(int32_t tileID, const std::vector<Tile>& map) const;
    void TransformSettlerToCity(int32_t unitID, int32_t tileID, const std::vector<Tile>& map);

private:
    NavigationGraph navGraph;
    std::vector<City> cities;
    std::vector<Empire> empires;
    std::vector<Unit> units;
};