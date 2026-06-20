#include "PopManager.hpp"
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <string>
#include <iostream>
#include "../Demographics.hpp"
int32_t PopManager::GetGroupCount(SocialClass targetClass) const
{
    int32_t count = 0;
    for (const auto &pop : this->population)
    {
        if (pop.socialClass == targetClass)
            count++;
    }
    return count;
}

void PopManager::AddPop(const Pop &newPop)
{
    this->population.push_back(newPop);
}

float PopManager::GetAverageSatisfaction(const std::vector<const Pop *> &subGroup) const
{
    if (subGroup.empty())
        return 0.0f;

    float totalSat = 0.0f;
    for (const auto *popPtr : subGroup)
    {
        totalSat += static_cast<float>(popPtr->satisfaction);
    }
    return totalSat / static_cast<float>(subGroup.size());
}

int32_t PopManager::CalculateGrowthPotential() const
{
    if (this->population.empty())
        return 0;

    int32_t totalNewborns = 0;

    for (const auto &pop : this->population)
    {
        if (pop.IsFemale() && pop.age >= 16 && pop.age <= 45)
        {

            float satisfactionRatio = static_cast<float>(pop.satisfaction) / 255.0f;
            float currentBirthChance = 0.40f + (satisfactionRatio * 0.08f);

            float roll = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
            if (roll < currentBirthChance)
            {
                totalNewborns++;
            }
        }
    }

    return totalNewborns;
}
void PopManager::ProgressAgeAndMortality()
{
    this->population.erase(
        std::remove_if(this->population.begin(), this->population.end(), [](Pop &pop)
                       {
            pop.age++;

            if (pop.age >= 60)
            {
                int32_t deathChance = 5 + (pop.age - 60) * 2;
                
                if (pop.age > 85 || (std::rand() % 100) < deathChance)
                {
                    return true; 
                }
            }
            return false; }),
        this->population.end());
}

void PopManager::ProcessMarketAndSatisfaction(std::map<ResourceType, float>& marketSupplies, std::map<ResourceType, float>& marketPrices)
{
    for (auto& pop : this->population)
    {
        auto personalNeeds = DemographicsConfig::GetNeedsForClass(pop.socialClass);
        float totalSatisfactionImpact = 0.0f;
        int32_t activeNeedsCount = 0;

        for (auto const& [category, marketNeed] : personalNeeds)
        {
            float targetDemand = marketNeed.baseDemandPerCapita;
            
            if (category == NeedCategory::Calories && pop.age < 15)
            {
                targetDemand = 0.1f + (static_cast<float>(pop.age) * 0.06f);
            }

            float satisfiedDemand = 0.0f;
            activeNeedsCount++;

            auto substitutes = MarketRegistry::GetSubstitutes(category);

            std::sort(substitutes.begin(), substitutes.end(), [&marketPrices](const auto& a, const auto& b) {
                float priceA = marketPrices.count(a.first) ? marketPrices[a.first] : 999.0f;
                float priceB = marketPrices.count(b.first) ? marketPrices[b.first] : 999.0f;
                return (priceA / a.second) < (priceB / b.second);
            });

            for (auto const& [resource, efficiency] : substitutes)
            {
                if (satisfiedDemand >= targetDemand) break;

                float neededAmount = (targetDemand - satisfiedDemand) / efficiency;
                
                float availableInWarehouse = (marketSupplies.count(resource)) ? marketSupplies[resource] : 0.0f;
                float boughtAmount = std::min(neededAmount, availableInWarehouse);

                marketSupplies[resource] -= boughtAmount;
                satisfiedDemand += boughtAmount * efficiency;
            }
            if (satisfiedDemand >= targetDemand)
            {
                totalSatisfactionImpact += 6.0f; 
            }
            else
            {
                if (category == NeedCategory::Calories)
                {
                    totalSatisfactionImpact -= 30.0f;
                }
                else
                {
                    totalSatisfactionImpact -= marketNeed.isCritical ? 4.0f : 1.0f;
                }
            }
        }

        if (activeNeedsCount > 0)
        {
            int32_t newSat = static_cast<int32_t>(pop.satisfaction) + static_cast<int32_t>(totalSatisfactionImpact);
            pop.satisfaction = static_cast<uint8_t>(std::max(0, std::min(255, newSat)));
        }
    }
}

void PopManager::GrowPopulation(int32_t growthAmount, uint16_t cultureID, uint8_t religionID, int32_t tileID)
{
    for (int32_t i = 0; i < growthAmount; ++i)
    {
        Pop baby;
        baby.locationTileID = tileID;
        baby.nameSeed = static_cast<uint16_t>(std::rand() % 65535);
        baby.cultureID = static_cast<uint8_t>(cultureID);
        baby.religionID = religionID;
        baby.age = 0;
        baby.demographicsFlags = 0;
        baby.satisfaction = 150;
        baby.literacy = 0;
        baby.reserved = 0;

        if (std::rand() % 2 == 0)
            baby.SetFemale();
        baby.SetAssimilated(true);

        if (!this->population.empty())
        {
            const Pop &randomParent = this->population[std::rand() % this->population.size()];
            baby.socialClass = randomParent.socialClass;
            baby.wealth = randomParent.wealth;
        }
        else
        {
            baby.socialClass = SocialClass::Laborer;
            baby.wealth = WealthLevel::Poor;
        }

        this->AddPop(baby);
    }
}

void PopManager::StarvePopulation(int32_t deathAmount)
{
    if (this->population.empty() || deathAmount <= 0)
        return;
    std::sort(this->population.begin(), this->population.end(), [](const Pop &a, const Pop &b)
              {
        if (a.wealth != b.wealth)
            return static_cast<uint8_t>(a.wealth) > static_cast<uint8_t>(b.wealth);
        return a.satisfaction > b.satisfaction; });

    size_t safeToKill = std::min(static_cast<size_t>(deathAmount), this->population.size());
    for (size_t i = 0; i < safeToKill; ++i)
    {
        this->population.pop_back();
    }
}

void PopManager::UpdateTurn(std::map<ResourceType, float> &marketSupplies, std::map<ResourceType, float> &marketPrices, uint16_t cultureID, uint8_t religionID, int32_t tileID)
{
    std::cout << "\n=================== RAPORT GOSPODARCZY (Kafel: " << tileID << ") ===================" << std::endl;

    std::cout << "[MAGAZYN START] Zboze: " << marketSupplies[ResourceType::Grain]
              << " j. | Ryby: " << marketSupplies[ResourceType::Fish] << " j." << std::endl;

    size_t popBeforeLines = this->population.size();
    this->ProgressAgeAndMortality();
    size_t naturalDeaths = popBeforeLines - this->population.size();
    if (naturalDeaths > 0)
    {
        std::cout << "[KOSTUCHA] Ze starosci zmarlo: " << naturalDeaths << " popow." << std::endl;
    }

    size_t popCountBeforeMarket = this->population.size();

    float grainBefore = marketSupplies[ResourceType::Grain];
    float fishBefore = marketSupplies[ResourceType::Fish];

    this->ProcessMarketAndSatisfaction(marketSupplies, marketPrices);

    float grainConsumed = grainBefore - marketSupplies[ResourceType::Grain];
    float fishConsumed = fishBefore - marketSupplies[ResourceType::Fish];

    std::cout << "[KONSUMPCJA] Popy zjadly z magazynu: " << grainConsumed << " Zboza i " << fishConsumed << " Ryb." << std::endl;
    std::cout << "[MAGAZYN KONIEC] Pozostalo -> Zboze: " << marketSupplies[ResourceType::Grain]
              << " j. | Ryby: " << marketSupplies[ResourceType::Fish] << " j." << std::endl;

    float avgSat = 0.0f;
    int32_t starvingPopsCount = 0;

    for (const auto &p : this->population)
    {
        avgSat += p.satisfaction;
        if (p.satisfaction < 50)
            starvingPopsCount++;
    }
    avgSat = this->population.empty() ? 0.0f : (avgSat / this->population.size());

    std::cout << "[RYNEK] Srednie zadowolenie: " << static_cast<int>((avgSat / 255.0f) * 100.0f)
              << "% | Popy na skraju glodu: " << starvingPopsCount << "/" << this->population.size() << std::endl;
    if (marketSupplies[ResourceType::Grain] <= 0.0f && marketSupplies[ResourceType::Fish] <= 0.0f && !this->population.empty())
    {
        int32_t starveDeaths = std::max(1, static_cast<int32_t>(this->population.size() * 0.10f));
        size_t sizeBeforeStarve = this->population.size();

        this->StarvePopulation(starveDeaths);

        size_t actualStarveDeaths = sizeBeforeStarve - this->population.size();
        std::cout << "[ALARM - GLOD] Magazyny sa PUSTE! Z glodu umiera: " << actualStarveDeaths << " popow!" << std::endl;
    }
    else if (avgSat > 165.0f && !this->population.empty())
    {
        int32_t potentialGrowth = this->CalculateGrowthPotential();
        if (potentialGrowth > 0)
        {
            this->GrowPopulation(potentialGrowth, cultureID, religionID, tileID);
            std::cout << "[NARODZINY] Spichlerze pelne! Urodzilo sie: " << potentialGrowth << " nowych popow." << std::endl;
        }
    }

    std::map<SocialClass, int32_t> groupCounts;
    for (const auto &pop : this->population)
        groupCounts[pop.socialClass]++;

    std::cout << "\n[SPIS LUDNOSCI - POZOSTALI PRZY ZYCIU]" << std::endl;
    for (auto const &[group, count] : groupCounts)
    {
        switch (group)
        {
        case SocialClass::Bound:
            std::cout << " -> Bound: " << count << std::endl;
            break;
        case SocialClass::Laborer:
            std::cout << " -> Laborer: " << count << std::endl;
            break;
        case SocialClass::Specialist:
            std::cout << " -> Specialist: " << count << std::endl;
            break;
        case SocialClass::Capitalist:
            std::cout << " -> Capitalist: " << count << std::endl;
            break;
        case SocialClass::Elite:
            std::cout << " -> Elite: " << count << std::endl;
            break;
        }
    }
    std::cout << "===================================================================\n"
              << std::endl;
}