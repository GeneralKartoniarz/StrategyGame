#include "GameManager.hpp"
#include <cmath>
#include "Map/Tile.hpp"
#include <filesystem>
#include <iostream>
#include <algorithm>
#include <limits>
#include "UI/BuildPanel.hpp"
#include "UI/GameInterface.hpp"

GameManager::GameManager(sf::RenderWindow *window)
    : window(window)
{
}

void GameManager::AddCity(const City &city)
{
    this->cities.push_back(city);
}

void GameManager::AddEmpire(const Empire &empire)
{
    this->empires.push_back(empire);
}
void GameManager::InitializeNameGenerator()
{
 std::filesystem::path resPath = std::filesystem::current_path() / "resources" / "full";
    
    std::cout << "[DEBUG] Szukam bazy w: " << resPath.string() << std::endl;

    if (std::filesystem::exists(resPath))
    {
        this->nameGenerator.load(resPath);
        
        if (this->nameGenerator.has_resources())
            std::cout << "[INFO] Sukces! Baza załadowana." << std::endl;
        else
            std::cout << "[ERROR] Ścieżka istnieje, ale biblioteka nie wczytała żadnych plików .names!" << std::endl;
    }
    else
    {
        std::cout << "[ERROR] Folder nie istnieje w: " << resPath.string() << std::endl;
    }
}
uint32_t GameManager::RegisterPopName(const std::wstring& name)
{
    this->registeredPopNames.push_back(name);
    return static_cast<uint32_t>(this->registeredPopNames.size() - 1);
}

std::wstring GameManager::GetPopName(uint32_t id) const
{
    if (id < this->registeredPopNames.size()) return this->registeredPopNames[id];
    return L"Anonim";
}
City &GameManager::GetCity(int32_t centerTileID)
{
    for (auto &city : this->cities)
    {
        if (city.centerTileID == centerTileID)
        {
            return city;
        }
    }
    return this->cities[0];
}

bool GameManager::TryPlaceBuildingAt(const sf::Vector2i &mousePos, BuildingType type, std::vector<Tile> &map, const sf::View &worldView)
{
    if (!BuildPanel::currentCityContext)
        return false;

    sf::Vector2f worldPos = this->window->mapPixelToCoords(mousePos, worldView);

    for (auto &tile : map)
    {
        bool mouseInsideTile = false;

        for (const auto &poly : tile.subPolygons)
        {
            size_t ptCount = poly.size();
            if (ptCount < 3)
                continue;

            for (size_t i = 0, j = ptCount - 1; i < ptCount; j = i++)
            {
                if (((poly[i].y > worldPos.y) != (poly[j].y > worldPos.y)) &&
                    (worldPos.x < (poly[j].x - poly[i].x) * (worldPos.y - poly[i].y) / (poly[j].y - poly[i].y) + poly[i].x))
                {
                    mouseInsideTile = !mouseInsideTile;
                }
            }
            if (mouseInsideTile)
                break;
        }

        if (!mouseInsideTile)
            continue;

        City &city = this->GetCity(BuildPanel::currentCityContext->centerTileID);
        int32_t currentTileID = static_cast<int32_t>(tile.ID);

        std::cout << "[MYSZ] Kliknieto w kafelek o ID: " << currentTileID << std::endl;

        auto it = std::find(city.jurisdictionTiles.begin(), city.jurisdictionTiles.end(), currentTileID);
        if (it == city.jurisdictionTiles.end())
        {
            std::cout << "[PLAC BUDOWY] Nie mozesz tu budowac! Ten kafelek lezy poza granicami osady." << std::endl;
            return false;
        }

        if (!tile.CanAddManufacture(type, city.buildQueue))
        {
            std::cout << "[PLAC BUDOWY] Blad! Przekroczono limit struktur lub poziom!" << std::endl;
            return false;
        }

        float cost = (type == BuildingType::Farm) ? 10.0f : 20.0f;
        if (city.warehouse[ResourceType::Wood] < cost)
        {
            std::cout << "[PLAC BUDOWY] Brak surowcow! Wymagane: " << cost << " j. drewna." << std::endl;
            return false;
        }

        city.warehouse[ResourceType::Wood] -= cost;

        ConstructionTask task;
        task.type = type;
        task.targetTileID = currentTileID;
        task.turnsLeft = 3;

        city.buildQueue.push_back(task);

        std::cout << "[PLAC BUDOWY] Sukces! Rozpoczeto wznoszenie struktury na kafelku: "
                  << currentTileID << ". Budowa potrwa jeszcze 3 tury." << std::endl;

        return true;
    }

    return false;
}

const Empire &GameManager::GetEmpire(int32_t id) const
{
    return this->empires[id];
}

Unit &GameManager::GetUnit(int32_t id)
{
    return this->units[id];
}
bool BuildingRegistry::IsBiomeAllowed(BuildingType bType, BiomeType biome)
{
    if (bType == BuildingType::Farm)
    {
        return biome != BiomeType::Ocean && 
               biome != BiomeType::IceSheet && 
               biome != BiomeType::MountainPeak;
    }
    
    // if (bType == BuildingType::Fishery) return biome == BiomeType::Ocean;

    return true;
}

std::string BuildingRegistry::GetBuildingName(BuildingType bType)
{
    switch (bType)
    {
        case BuildingType::Farm: return "Farma";
        // case BuildingType::Fishery: return "Przystan Rybacka";
        default: return "Nieznany Obiekt";
    }
}
int32_t GameManager::GetNearestNodeID(sf::Vector2f worldPos) const
{
    if (this->navGraph.nodes.empty())
        return -1;

    int32_t nearestID = -1;
    float minDistanceSq = std::numeric_limits<float>::max();

    for (size_t i = 0; i < this->navGraph.nodes.size(); ++i)
    {
        float dx = worldPos.x - this->navGraph.nodes[i].position.x;
        float dy = worldPos.y - this->navGraph.nodes[i].position.y;
        float distanceSq = dx * dx + dy * dy;

        if (distanceSq < minDistanceSq)
        {
            minDistanceSq = distanceSq;
            nearestID = static_cast<int32_t>(i);
        }
    }
    return nearestID;
}

void GameManager::UpdateUnits(float dt)
{
    float speed = 60.0f;

    for (auto &unit : this->units)
    {
        if (unit.nextNodeID != -1)
        {
            sf::Vector2f targetPos = this->navGraph.nodes[unit.nextNodeID].position;
            sf::Vector2f dir = targetPos - unit.position;
            float dist = std::sqrt(dir.x * dir.x + dir.y * dir.y);

            if (dist <= speed * dt)
            {
                unit.position = targetPos;
                unit.currentNodeID = unit.nextNodeID;
                unit.nextNodeID = -1;
                unit.currentMovementPoints -= 1;
            }
            else
            {
                dir /= dist;
                unit.position += dir * (speed * dt);
            }
        }
        else if (!unit.movementPath.empty() && unit.currentMovementPoints > 0)
        {
            unit.nextNodeID = unit.movementPath.front();
            unit.movementPath.erase(unit.movementPath.begin());
        }
    }
}

void GameManager::NextTurn(std::vector<Tile> &map)
{
    for (auto &empire : this->empires)
    {
        empire.UpdateTurn(map, this->cities, *this);
    }
    this->ResetMovementPoints();
}

void GameManager::ResetMovementPoints()
{
    for (auto &unit : this->units)
    {
        unit.currentMovementPoints = unit.maxMovementPoints;
    }
}

bool GameManager::CanFoundCity(int32_t tileID, const std::vector<Tile> &map) const
{
    if (map[tileID].terrain.biome == BiomeType::Ocean)
        return false;

    std::vector<int32_t> visited;
    std::vector<int32_t> queue;

    queue.push_back(tileID);
    visited.push_back(tileID);

    for (int step = 0; step < 2; ++step)
    {
        std::vector<int32_t> nextWave;
        for (int32_t currentID : queue)
        {
            for (std::size_t nIdx : map[currentID].neighbors)
            {
                int32_t neighborID = static_cast<int32_t>(nIdx);
                if (std::find(visited.begin(), visited.end(), neighborID) == visited.end())
                {
                    visited.push_back(neighborID);
                    nextWave.push_back(neighborID);
                }
            }
        }
        queue = nextWave;
    }

    for (const auto &city : this->cities)
    {
        if (std::find(visited.begin(), visited.end(), city.centerTileID) != visited.end())
        {
            return false;
        }
    }

    return true;
}

void GameManager::TransformSettlerToCity(int32_t unitID, int32_t tileID, uint32_t nameID, const std::vector<Tile> &map)
{
    if (!this->CanFoundCity(tileID, map))
    {
        return;
    }

    Unit &settler = this->units[unitID];
    int32_t empireID = settler.ownerEmpireID;

    City newCity;
    newCity.nameID = nameID;
    newCity.centerTileID = tileID;
    newCity.ownerEmpireID = empireID;

    newCity.jurisdictionTiles.push_back(tileID);
    newCity.warehouse[ResourceType::Grain] = 300.0f;
    newCity.warehouse[ResourceType::Fish] = 200.0f;
    newCity.warehouse[ResourceType::Wood] = 150.0f;

    for (std::size_t nIdx : map[tileID].neighbors)
    {
        newCity.jurisdictionTiles.push_back(static_cast<int32_t>(nIdx));
    }

    Empire &empire = const_cast<Empire &>(this->empires[empireID]);
    PopManager &popSys = empire.GetPopManager();
    uint8_t culture = static_cast<uint8_t>(empireID);

    for (int i = 0; i < 7; ++i)
    {
        Pop boundPop;
        boundPop.locationTileID = tileID;
        boundPop.nameSeed = static_cast<uint16_t>(std::rand() % 65535);
        boundPop.cultureID = culture;
        boundPop.religionID = 0;
        boundPop.age = 20 + (std::rand() % 15);
        boundPop.socialClass = SocialClass::Bound;
        boundPop.wealth = WealthLevel::Poor;
        boundPop.literacy = 2;
        boundPop.satisfaction = 180;
        boundPop.reserved = 0;
        boundPop.demographicsFlags = 0x02;
        if (std::rand() % 2 == 0)
            boundPop.demographicsFlags |= 0x01;

        popSys.AddPop(boundPop);
    }

    for (int i = 0; i < 3; ++i)
    {
        Pop laborerPop;
        laborerPop.locationTileID = tileID;
        laborerPop.nameSeed = static_cast<uint16_t>(std::rand() % 65535);
        laborerPop.cultureID = culture;
        laborerPop.religionID = 0;
        laborerPop.age = 20 + (std::rand() % 15);
        laborerPop.socialClass = SocialClass::Laborer;
        laborerPop.wealth = WealthLevel::Poor;
        laborerPop.literacy = 5;
        laborerPop.satisfaction = 180;
        laborerPop.reserved = 0;
        laborerPop.demographicsFlags = 0x02;
        if (std::rand() % 2 == 0)
            laborerPop.demographicsFlags |= 0x01;

        popSys.AddPop(laborerPop);
    }

    this->cities.push_back(newCity);

    int32_t newCityID = static_cast<int32_t>(this->cities.size() - 1);
    this->empires[empireID].AddCity(newCityID);

    this->units.erase(this->units.begin() + unitID);
    for (size_t i = 0; i < this->units.size(); ++i)
    {
        this->units[i].ID = static_cast<int32_t>(i);
    }
}

uint32_t GameManager::RegisterCityName(const std::string &name)
{
    uint32_t assignedID = this->nextCityNameID++;
    this->cityNamesRegistry[assignedID] = name;
    return assignedID;
}

std::string GameManager::GetCityName(uint32_t nameID) const
{
    auto it = this->cityNamesRegistry.find(nameID);
    if (it != this->cityNamesRegistry.end())
    {
        return it->second;
    }
    return "Nieznane Miasto";
}