#pragma once
#include <vector>
#include <cstdint>
#include "Pop.hpp"

class PopManager 
{
public:
    PopManager() = default;
    ~PopManager() = default;

    void AddPop(const Pop& newPop); 
    void ProgressAgeAndMortality();
    const std::vector<Pop>& GetAllPops() const { return population; }

    size_t GetTotalPopulation() const { return population.size(); }
    int32_t GetGroupCount(SocialClass targetClass) const;

    void UpdateSatisfaction(float townFoodSurplus);
    float GetAverageSatisfaction(const std::vector<const Pop*>& subGroup) const;

    void ConsumeSupplies(float availableFood, float& outRemainingFood);
    int32_t CalculateGrowthPotential(float foodSurplus) const;
    void GrowPopulation(int32_t growthAmount, uint16_t cultureID, uint8_t religionID, int32_t tileID);
    void StarvePopulation(int32_t deathAmount);

    void UpdateTurn(float availableFood, float& outRemainingFood, uint16_t cultureID, uint8_t religionID, int32_t tileID);

private:
    std::vector<Pop> population;
};