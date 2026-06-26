#pragma once
#include <vector>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <SFML/Graphics.hpp>
#include "City.hpp"
#include "Empires/Empire.hpp"
#include "Map/NavigationGraph.hpp"
#include "Unit.hpp"
#include "namegen.hpp"
struct Tile;

class GameManager
{
public:
    GameManager(sf::RenderWindow *window);
    ~GameManager() = default;

    void AddCity(const City &city);
    void AddEmpire(const Empire &empire);

    City &GetCity(int32_t centerTileID);
    const Empire &GetEmpire(int32_t id) const;

    const std::vector<City> &GetAllCities() const { return cities; }
    const std::vector<Empire> &GetAllEmpires() const { return empires; }

    uint32_t RegisterCityName(const std::string &name);
    std::string GetCityName(uint32_t nameID) const;

    void AddUnit(const Unit &unit) { units.push_back(unit); }
    const std::vector<Unit> &GetAllUnits() const { return units; }

    int32_t GetNearestNodeID(sf::Vector2f worldPos) const;

    void SetNavGraph(const NavigationGraph &graph) { navGraph = graph; }
    NavigationGraph &GetNavGraph() { return navGraph; }

    Unit &GetUnit(int32_t id);
    bool RecruitSettler(int32_t cityID, const std::vector<Tile>& map);

    void UpdateUnits(float dt);
    void ResetMovementPoints();

    bool CanFoundCity(int32_t tileID, const std::vector<Tile> &map) const;
    void TransformSettlerToCity(int32_t unitID, int32_t tileID, uint32_t nameID, const std::vector<Tile> &map);
    void NextTurn(std::vector<Tile> &map);

    bool TryPlaceBuildingAt(const sf::Vector2i &mousePos, BuildingType type, std::vector<Tile> &map, const sf::View &worldView);
    dasmig::ng &GetNameGenerator() { return nameGenerator; }
    void InitializeNameGenerator();
    uint32_t RegisterPopName(const std::wstring &name);
    std::wstring GetPopName(uint32_t id) const;

private:
    sf::RenderWindow *window;
    NavigationGraph navGraph;
    std::vector<City> cities;
    std::vector<Empire> empires;
    std::vector<Unit> units;
    dasmig::ng nameGenerator;
    std::vector<std::wstring> registeredPopNames;
    std::unordered_map<uint32_t, std::string> cityNamesRegistry;

    uint32_t nextCityNameID = 0;
};