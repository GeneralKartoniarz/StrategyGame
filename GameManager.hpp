#pragma once
#include <vector>
#include <cstdint>
#include "City.hpp"
#include "Empire.hpp"

class GameManager 
{
public:
    GameManager() = default;
    ~GameManager() = default;

    void AddCity(const City& city);
    void AddEmpire(const Empire& empire);

    City& GetCity(int32_t id);
    Empire& GetEmpire(int32_t id);

    const std::vector<City>& GetAllCities() const { return cities; }
    const std::vector<Empire>& GetAllEmpires() const { return empires; }

private:
    std::vector<City> cities;
    std::vector<Empire> empires;
};