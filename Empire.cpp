#include "Empire.hpp"
#include <algorithm>


Empire::Empire(int32_t id, const std::string& name, sf::Color color)
    : empireID(id), name(name), mapColor(color)
{
}

void Empire::AddCity(int32_t cityID)
{
    this->controlledCitiesIDs.push_back(cityID);
}

void Empire::RemoveCity(int32_t cityID)
{
    auto it = std::remove(this->controlledCitiesIDs.begin(), this->controlledCitiesIDs.end(), cityID);
    if (it != this->controlledCitiesIDs.end())
    {
        this->controlledCitiesIDs.erase(it, this->controlledCitiesIDs.end());
    }
}