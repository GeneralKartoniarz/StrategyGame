#include "GameManager.hpp"
#include <cmath>
#include "Map/Tile.hpp"
#include <iostream>
#include <algorithm>

void GameManager::AddCity(const City &city)
{
    this->cities.push_back(city);
}

void GameManager::AddEmpire(const Empire &empire)
{
    this->empires.push_back(empire);
}

City &GameManager::GetCity(int32_t id)
{
    return this->cities[id];
}

const Empire& GameManager::GetEmpire(int32_t id) const
{
    return this->empires[id];
}

Unit &GameManager::GetUnit(int32_t id)
{
    return this->units[id];
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

    for (auto& unit : this->units)
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

void GameManager::ResetMovementPoints()
{
    for (auto& unit : this->units)
    {
        unit.currentMovementPoints = unit.maxMovementPoints;
    }
}

bool GameManager::CanFoundCity(int32_t tileID, const std::vector<Tile>& map) const
{
    if (map[tileID].terrain.biome == BiomeType::Ocean) return false;

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

    for (const auto& city : this->cities)
    {
        if (std::find(visited.begin(), visited.end(), city.centerTileID) != visited.end())
        {
            return false; 
        }
    }

    return true;
}

void GameManager::TransformSettlerToCity(int32_t unitID, int32_t tileID, uint32_t nameID, const std::vector<Tile>& map)
{
    if (!this->CanFoundCity(tileID, map))
    {
        return;
    }

    Unit& settler = this->units[unitID];
    int32_t empireID = settler.ownerEmpireID;

    City newCity;
    newCity.nameID = 0; 
    newCity.centerTileID = tileID;
    newCity.ownerEmpireID = empireID;

    newCity.jurisdictionTiles.push_back(tileID);
    //może to zostawie idk
    for (std::size_t nIdx : map[tileID].neighbors)
    {
        newCity.jurisdictionTiles.push_back(static_cast<int32_t>(nIdx));
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
uint32_t GameManager::RegisterCityName(const std::string& name)
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