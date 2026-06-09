#include "GameManager.hpp"
void GameManager::AddCity(const City& city)
{
    this->cities.push_back(city);
}

void GameManager::AddEmpire(const Empire& empire)
{
    this->empires.push_back(empire);
}

City& GameManager::GetCity(int32_t id)
{
    return this->cities[id];
}

Empire& GameManager::GetEmpire(int32_t id)
{
    return this->empires[id];
}