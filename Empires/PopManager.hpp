#pragma once
#include <vector>
#include <cstdint>
#include <map>
#include "Pop.hpp"
#include "Economy.hpp"
#include "namegen.hpp"
class GameManager;
struct City;
class PopManager
{
public:
    PopManager() = default;
    ~PopManager() = default;

    void AddPop(const Pop &newPop);
    void ProgressAgeAndMortality();
    const std::vector<Pop> &GetAllPops() const { return population; }
    std::vector<Pop> &GetPopulationRef() { return population; }

    size_t GetTotalPopulation() const { return population.size(); }
    int32_t GetGroupCount(SocialClass targetClass) const;

    void ProcessMarketAndSatisfaction(std::map<ResourceType, float> &marketSupplies, std::map<ResourceType, MarketCommodity> &market, City &city, const std::map<SocialClass, float> &averageClassWages);

    void UpdateTurn(std::map<ResourceType, float> &marketSupplies, std::map<ResourceType, MarketCommodity> &market, uint16_t cultureID, uint8_t religionID, int32_t tileID, GameManager &gm, uint8_t empireCultureRaw, City &city, const std::map<SocialClass, float> &averageClassWages);

    void GrowPopulation(int32_t growthAmount, uint16_t cultureID, uint8_t religionID, int32_t tileID, GameManager &gm, uint8_t empireCultureRaw);
    float GetAverageSatisfaction(const std::vector<const Pop *> &subGroup) const;

    int32_t CalculateGrowthPotential() const;
    void StarvePopulation(int32_t deathAmount);

private:
    std::vector<Pop> population;
};