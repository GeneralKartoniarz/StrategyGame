#include "PopManager.hpp"
#include <algorithm>
#include <cmath>
#include <cstdlib>

int32_t PopManager::GetGroupCount(PopGroup targetGroup) const
{
    int32_t count = 0;
    for (const auto& pop : this->population)
    {
        if (pop.group == targetGroup) count++;
    }
    return count;
}

void PopManager::AddPop(const Pop& newPop)
{
    this->population.push_back(newPop);
}

void PopManager::UpdateSatisfaction(float townFoodSurplus)
{
    bool isStarving = (townFoodSurplus < 0.0f);

    for (auto& pop : this->population)
    {
        int32_t change = 0;

        if (isStarving)
        {
            change -= 30; 
        }
        else
        {
            change += 5;
        }

        if (pop.wealth == WealthLevel::Rich || pop.wealth == WealthLevel::FilthyRich)
        {
            if (isStarving) change -= 15; 
        }
        else if (pop.wealth == WealthLevel::Broke)
        {
            if (!isStarving) change += 2;
        }

        int32_t newSat = static_cast<int32_t>(pop.satisfaction) + change;
        pop.satisfaction = static_cast<uint8_t>(std::max(0, std::min(255, newSat)));
    }
}

float PopManager::GetAverageSatisfaction(const std::vector<const Pop*>& subGroup) const
{
    if (subGroup.empty()) return 0.0f;

    float totalSat = 0.0f;
    for (const auto* popPtr : subGroup)
    {
        totalSat += static_cast<float>(popPtr->satisfaction);
    }
    return totalSat / static_cast<float>(subGroup.size());
}

void PopManager::ConsumeSupplies(float availableFood, float& outRemainingFood)
{
    float totalRequired = static_cast<float>(this->population.size()) * 0.1f;
    outRemainingFood = availableFood - totalRequired;
}

int32_t PopManager::CalculateGrowthPotential(float foodSurplus) const
{
    if (foodSurplus <= 0.0f || this->population.empty()) return 0;

    int32_t femaleCount = 0;
    for (const auto& pop : this->population)
    {
        if (pop.IsFemale() && pop.age >= 16 && pop.age <= 45) femaleCount++;
    }

    int32_t foodCap = static_cast<int32_t>(foodSurplus * 5.0f);
    int32_t birthCap = femaleCount / 4;

    return std::max(0, std::min(foodCap, birthCap));
}

void PopManager::GrowPopulation(int32_t growthAmount, uint16_t cultureID, uint8_t religionID, int32_t tileID)
{
    for (int32_t i = 0; i < growthAmount; ++i)
    {
        Pop baby;
        baby.locationTileID = tileID;
        baby.firstNameID = static_cast<uint16_t>(std::rand() % 2000);
        baby.lastNameID = static_cast<uint16_t>(std::rand() % 2000);
        baby.cultureID = cultureID;
        baby.age = 0;
        baby.group = PopGroup::Serf;
        baby.wealth = WealthLevel::Broke;
        baby.religion = static_cast<ReligionGroup>(religionID);
        baby.demographicsFlags = 0;
        baby.satisfaction = 150;

        if (std::rand() % 2 == 0) baby.SetFemale();
        baby.SetAssimilated(true);

        this->AddPop(baby);
    }
}

void PopManager::StarvePopulation(int32_t deathAmount)
{
    size_t safeToKill = std::min(static_cast<size_t>(deathAmount), this->population.size());
    for (size_t i = 0; i < safeToKill; ++i)
    {
        this->population.pop_back();
    }
}

void PopManager::UpdateTurn(float availableFood, float& outRemainingFood, uint16_t cultureID, uint8_t religionID, int32_t tileID)
{
    this->ConsumeSupplies(availableFood, outRemainingFood);

    float surplus = outRemainingFood;
    this->UpdateSatisfaction(surplus);

    if (surplus < 0.0f)
    {
        int32_t deaths = static_cast<int32_t>(std::abs(surplus) * 10.0f);
        this->StarvePopulation(deaths);
        outRemainingFood = 0.0f; 
    }
    else
    {
        int32_t potentialGrowth = this->CalculateGrowthPotential(surplus);
        if (potentialGrowth > 0)
        {
            this->GrowPopulation(potentialGrowth, cultureID, religionID, tileID);
        }
    }
}