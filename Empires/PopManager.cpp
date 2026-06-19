#include "PopManager.hpp"
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <string>
#include <map>
#include <iostream>

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

void PopManager::UpdateSatisfaction(float townFoodSurplus)
{
    bool isStarving = (townFoodSurplus < 0.0f);

    for (auto &pop : this->population)
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
            if (isStarving)
                change -= 15;
        }
        else if (pop.wealth == WealthLevel::Broke)
        {
            if (!isStarving)
                change += 2;
        }

        int32_t newSat = static_cast<int32_t>(pop.satisfaction) + change;
        pop.satisfaction = static_cast<uint8_t>(std::max(0, std::min(255, newSat)));
    }
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

void PopManager::ConsumeSupplies(float availableFood, float &outRemainingFood)
{
    float totalRequired = static_cast<float>(this->population.size()) * 0.1f;
    outRemainingFood = availableFood - totalRequired;
}

int32_t PopManager::CalculateGrowthPotential(float foodSurplus) const
{
    if (foodSurplus <= 0.0f || this->population.empty())
        return 0;

    int32_t femaleCount = 0;
    for (const auto &pop : this->population)
    {
        if (pop.IsFemale() && pop.age >= 16 && pop.age <= 45)
            femaleCount++;
    }

    int32_t foodCap = static_cast<int32_t>(foodSurplus * 5.0f);
    int32_t birthCap = femaleCount / 4;

    return std::max(0, std::min(foodCap, birthCap));
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

        if (std::rand() % 2 == 0) baby.SetFemale();
        baby.SetAssimilated(true);

        if (!this->population.empty())
        {
            const Pop& randomParent = this->population[std::rand() % this->population.size()];
            baby.socialClass = randomParent.socialClass;
                     
            if (randomParent.wealth == WealthLevel::Broke) baby.wealth = WealthLevel::Broke;
            else baby.wealth = static_cast<WealthLevel>(static_cast<uint8_t>(randomParent.wealth) - 1);
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
    if (this->population.empty() || deathAmount <= 0) return;
    std::sort(this->population.begin(), this->population.end(), [](const Pop& a, const Pop& b) {
        if (a.wealth != b.wealth)
            return static_cast<uint8_t>(a.wealth) > static_cast<uint8_t>(b.wealth);
        return a.satisfaction > b.satisfaction;
    });

    size_t safeToKill = std::min(static_cast<size_t>(deathAmount), this->population.size());
    for (size_t i = 0; i < safeToKill; ++i)
    {
        this->population.pop_back();
    }
}

void PopManager::UpdateTurn(float availableFood, float& outRemainingFood, uint16_t cultureID, uint8_t religionID, int32_t tileID)
{
    std::cout << "\n=================== RAPORT TURY (Kafel: " << tileID << ") ===================" << std::endl;

    size_t popBeforeLines = this->population.size();
    this->ProgressAgeAndMortality();
    size_t naturalDeaths = popBeforeLines - this->population.size();
    
    if (naturalDeaths > 0)
    {
        std::cout << "[KOSTUCHA] Ze starosci zmarlo: " << naturalDeaths << " popow." << std::endl;
    }

    size_t totalPops = this->population.size();
    this->ConsumeSupplies(availableFood, outRemainingFood);

    float surplus = outRemainingFood;
    this->UpdateSatisfaction(surplus);

    std::cout << "[ZASOBY] Populacja (" << totalPops << " popow) zjadla: " 
              << (totalPops * 0.1f) << " j. zywnosci. Bilans: " << surplus << std::endl;

    if (surplus < 0.0f)
    {
        int32_t deaths = static_cast<int32_t>(std::abs(surplus) * 10.0f);
        size_t popBeforeStarve = this->population.size();
        
        this->StarvePopulation(deaths);
        
        size_t actualStarveDeaths = popBeforeStarve - this->population.size();
        std::cout << "[GLOD] Skrajne niedozywienie! Z glodu zmarlo: " << actualStarveDeaths << " popow." << std::endl;
        outRemainingFood = 0.0f; 
    }
    else
    {
        int32_t potentialGrowth = this->CalculateGrowthPotential(surplus);
        if (potentialGrowth > 0)
        {
            this->GrowPopulation(potentialGrowth, cultureID, religionID, tileID);
            std::cout << "[NARODZINY] W miescie urodzilo sie: " << potentialGrowth << " nowych popow." << std::endl;
        }
    }

    std::map<SocialClass, int32_t> groupCounts;
    std::map<WealthLevel, int32_t> wealthCounts;

    for (const auto& pop : this->population)
    {
        groupCounts[pop.socialClass]++;
        wealthCounts[pop.wealth]++;
    }

    auto getClassName = [](SocialClass g) -> std::string {
        switch(g) {
            case SocialClass::Bound:      return "Bound (Zwiazani)";
            case SocialClass::Laborer:    return "Laborer (Robotnicy)";
            case SocialClass::Specialist: return "Specialist (Fachowcy)";
            case SocialClass::Capitalist: return "Capitalist (Posiadacze)";
            case SocialClass::Elite:      return "Elite (Elita)";
            default:                      return "Inni";
        }
    };

    auto getWealthName = [](WealthLevel w) -> std::string {
        switch(w) {
            case WealthLevel::Broke:       return "Bankruci";
            case WealthLevel::Poor:        return "Ubodzy";
            case WealthLevel::Middle:      return "Klasa Srednia";
            case WealthLevel::Rich:        return "Bogaci";
            case WealthLevel::FilthyRich:  return "Obrzydliwie Bogaci";
            default:                       return "Nieznany";
        }
    };

    std::cout << "\n[SPIS LUDNOSCI - STRUKTURA KLASOWA]" << std::endl;
    for (auto const& [group, count] : groupCounts)
    {
        std::cout << " -> " << getClassName(group) << ": " << count << std::endl;
    }

    std::cout << "[SPIS LUDNOSCI - STRUKTURA MAJĄTKOWA]" << std::endl;
    for (auto const& [wealth, count] : wealthCounts)
    {
        std::cout << " -> " << getWealthName(wealth) << ": " << count << std::endl;
    }

    std::cout << "\n[STATUS KOŃCOWY] Łaczna liczba ludnosci: " << this->population.size() << " popow." << std::endl;
    std::cout << "===================================================================\n" << std::endl;
}