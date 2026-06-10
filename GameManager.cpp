#include "GameManager.hpp"
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

Empire &GameManager::GetEmpire(int32_t id)
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