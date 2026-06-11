#include "GameManager.hpp"
#include <cmath>
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