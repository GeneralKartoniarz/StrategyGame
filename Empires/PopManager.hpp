#pragma once
#include <vector>
#include <cstdint>
#include <map>
#include "Pop.hpp"
#include "Economy.hpp"

class PopManager 
{
public:
    PopManager() = default;
    ~PopManager() = default;

    void AddPop(const Pop& newPop); 
    void ProgressAgeAndMortality();
    const std::vector<Pop>& GetAllPops() const { return population; }
    std::vector<Pop>& GetPopulationRef() { return population; }

    size_t GetTotalPopulation() const { return population.size(); }
    int32_t GetGroupCount(SocialClass targetClass) const;

    void ProcessMarketAndSatisfaction(std::map<ResourceType, float>& marketSupplies, std::map<ResourceType, float>& marketPrices);
    float GetAverageSatisfaction(const std::vector<const Pop*>& subGroup) const;

    int32_t CalculateGrowthPotential() const;
    void GrowPopulation(int32_t growthAmount, uint16_t cultureID, uint8_t religionID, int32_t tileID);
    void StarvePopulation(int32_t deathAmount);

    void UpdateTurn(std::map<ResourceType, float>& marketSupplies, std::map<ResourceType, float>& marketPrices, uint16_t cultureID, uint8_t religionID, int32_t tileID);

private:
    std::vector<Pop> population;
};