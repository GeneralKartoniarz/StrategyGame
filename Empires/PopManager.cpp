#include "PopManager.hpp"
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <string>
#include <iostream>
#include "../Demographics.hpp"
#include "../GameManager.hpp"

#include <string>
/*
 * [PL] METODA: GetGroupCount
 * LOGIKA: Pomocnicza metoda zwracająca liczebność popów w danej klasie społecznej.
 * [EN] METHOD: GetGroupCount
 * LOGIC: Helper method returning the count of pops belonging to a specific social class.
 */
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
/*
 * [PL] METODA: AddPop
 * LOGIKA: Rejestruje nowego obywatela w strukturach państwa.
 * [EN] METHOD: AddPop
 * LOGIC: Registers a newly created citizen in the state structures.
 */
void PopManager::AddPop(const Pop &newPop)
{
    this->population.push_back(newPop);
}
/*
 * [PL] METODA: GetAverageSatisfaction
 * LOGIKA: Zwraca średnią satysfakcję dla podzbioru populacji (wykorzystuje wskaźniki).
 * [EN] METHOD: GetAverageSatisfaction
 * LOGIC: Returns the average satisfaction for a population subset using pointers.
 */
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
/*
 * [PL] METODA: CalculateGrowthPotential
 * LOGIKA: Szacuje przyrost naturalny na podstawie zadowolenia, płci i wieku (16-45).
 * [EN] METHOD: CalculateGrowthPotential
 * LOGIC: Estimates natural growth based on satisfaction, gender, and reproductive age (16-45).
 */
int32_t PopManager::CalculateGrowthPotential(City &city) const
{
    if (this->population.empty())
        return 0;

    int32_t currentPopulation = static_cast<int32_t>(this->population.size());

    int32_t maxSustainablePop = static_cast<int32_t>(city.producedFood);

    if (currentPopulation >= maxSustainablePop)
    {
        return 0;
    }

    int32_t totalNewborns = 0;

    for (const auto &pop : this->population)
    {
        if (pop.IsFemale() && pop.age >= 16 && pop.age <= 45)
        {
            float satisfactionRatio = static_cast<float>(pop.satisfaction) / 255.0f;
            float currentBirthChance = 0.06f + (satisfactionRatio * 0.08f);

            float roll = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
            if (roll < currentBirthChance)
            {
                totalNewborns++;
            }
        }
    }

    if (currentPopulation + totalNewborns > maxSustainablePop)
    {
        totalNewborns = maxSustainablePop - currentPopulation;
    }

    return totalNewborns;
}
/*
 * [PL] METODA: ProgressAgeAndMortality
 * LOGIKA: Zwiększa wiek popów (bufor 12 miesięcy) i realizuje śmierć ze starości.
 * [EN] METHOD: ProgressAgeAndMortality
 * LOGIC: Increments pop age (using a 12-month buffer) and executes old-age death mechanics.
 */
void PopManager::ProgressAgeAndMortality()
{
    this->population.erase(
        std::remove_if(this->population.begin(), this->population.end(), [](Pop &pop)
                       {
            pop.reserved++;
            if (pop.reserved >= 12)
            {
                pop.age++;
                pop.reserved = 0;
            }

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
/*
 * [PL] METODA: ProcessMarketAndSatisfaction
 * LOGIKA: Przetwarza fazę konsumpcji na podstawie rynkowych zarobków (averageClassWages).
 * Pop szuka substytutów na giełdzie, zdejmuje towar z magazynu miasta i zostawia tam pieniądz.
 * Jeśli pop jest bezrobotny (lub to dziecko/kobieta), utrzymuje się z UBI (childSupportFund) opłacanego z podatków firm.
 * Satysfakcja i flagi głodu opierają się na faktycznych 16-bajtowych propercjach struktury Pop.
 * POWIĄZANIA: Demographics.hpp (GetNeedsForClass z dodanym wymogiem WealthLevel).
 */
void PopManager::ProcessMarketAndSatisfaction(std::map<ResourceType, float> &marketSupplies, std::map<ResourceType, MarketCommodity> &market, City &city, const std::map<SocialClass, float> &averageClassWages)
{
    float totalSalesValue = 0.0f;

    int32_t welfareRecipients = 0;
    for (const auto &pop : this->population)
    {
        if (std::find(city.jurisdictionTiles.begin(), city.jurisdictionTiles.end(), pop.locationTileID) != city.jurisdictionTiles.end())
        {
            if (!pop.IsEmployed())
            {
                welfareRecipients++;
            }
        }
    }

    float welfarePerCapita = welfareRecipients > 0 ? (city.childSupportFund / welfareRecipients) : 0.0f;
    if (welfareRecipients > 0)
    {
        city.childSupportFund = 0.0f;
    }
    else
    {
        totalSalesValue += city.childSupportFund;
        city.childSupportFund = 0.0f;
    }

    std::vector<size_t> localPopIndices;
    localPopIndices.reserve(this->population.size());

    for (size_t i = 0; i < this->population.size(); ++i)
    {
        if (std::find(city.jurisdictionTiles.begin(), city.jurisdictionTiles.end(), this->population[i].locationTileID) != city.jurisdictionTiles.end())
        {
            localPopIndices.push_back(i);
        }
    }
    std::sort(localPopIndices.begin(), localPopIndices.end(), [this](size_t a, size_t b)
              {
        const auto& popA = this->population[a];
        const auto& popB = this->population[b];

        if (popA.IsEmployed() != popB.IsEmployed())
        {
            return popA.IsEmployed() > popB.IsEmployed();
        }

        return popA.age < popB.age; });

    for (size_t popIndex : localPopIndices)
    {
        auto &pop = this->population[popIndex];

        float popBudget = 0.0f;

        if (pop.IsEmployed())
        {
            if (averageClassWages.count(pop.socialClass))
            {
                popBudget = averageClassWages.at(pop.socialClass);
            }
        }
        else
        {
            popBudget = welfarePerCapita;
        }

        auto personalNeeds = DemographicsConfig::GetNeedsForClass(pop.socialClass, pop.wealth);
        float totalSatisfactionImpact = 0.0f;
        int32_t activeNeedsCount = 0;

        for (auto const &[category, marketNeed] : personalNeeds)
        {
            float targetDemand = marketNeed.baseDemandPerCapita;
            if (category == NeedCategory::Calories && pop.age < 15)
            {
                targetDemand = 0.1f + (static_cast<float>(pop.age) * 0.06f);
            }

            float satisfiedDemand = 0.0f;
            activeNeedsCount++;

            auto substitutes = MarketRegistry::GetSubstitutes(category);

            std::sort(substitutes.begin(), substitutes.end(), [&market](const auto &a, const auto &b)
                      {
                float priceA = market.count(a.first) ? market.at(a.first).currentPrice : 999.0f;
                float priceB = market.count(b.first) ? market.at(b.first).currentPrice : 999.0f;
                return (priceA / a.second) < (priceB / b.second); });

            for (auto const &[resource, efficiency] : substitutes)
            {
                if (satisfiedDemand >= targetDemand)
                    break;

                float price = market.count(resource) ? market.at(resource).currentPrice : 999.0f;
                if (price <= 0.01f)
                    price = 0.01f;

                float neededAmount = (targetDemand - satisfiedDemand) / efficiency;
                market[resource].demandLastTurn += neededAmount;

                float availableInWarehouse = (marketSupplies.count(resource)) ? marketSupplies[resource] : 0.0f;

                float maxAffordable = popBudget / price;
                float boughtAmount = std::min({neededAmount, availableInWarehouse, maxAffordable});

                if (boughtAmount > 0.0f)
                {
                    marketSupplies[resource] -= boughtAmount;
                    float cost = boughtAmount * price;
                    popBudget -= cost;
                    totalSalesValue += cost;
                    satisfiedDemand += boughtAmount * efficiency;
                }
            }

            if (satisfiedDemand + 0.01f >= targetDemand)
            {
                totalSatisfactionImpact += marketNeed.isCritical ? 6.0f : 12.0f;
                if (category == NeedCategory::Calories && pop.HasStarvingWarning())
                {
                    pop.SetStarvingWarning(false);
                }
            }
            else
            {
                if (category == NeedCategory::Calories)
                {
                    if (satisfiedDemand + 0.01f >= targetDemand * 0.5f)
                    {
                        totalSatisfactionImpact -= 5.0f;
                    }
                    else
                    {
                        if (!pop.HasStarvingWarning())
                        {
                            pop.SetStarvingWarning(true);
                            totalSatisfactionImpact -= 30.0f;
                        }
                        else
                        {
                            totalSatisfactionImpact -= 100.0f;
                        }
                    }
                }
                else
                {
                    if (marketNeed.isCritical)
                    {
                        totalSatisfactionImpact -= 4.0f;
                    }
                }
            }
        }

        if (activeNeedsCount > 0)
        {
            int32_t newSat = static_cast<int32_t>(pop.satisfaction) + static_cast<int32_t>(totalSatisfactionImpact);
            pop.satisfaction = static_cast<uint8_t>(std::max(0, std::min(255, newSat)));
        }

        if (popBudget > 2.0f && totalSatisfactionImpact >= 0.0f)
        {
            if (pop.wealth != WealthLevel::FilthyRich && (std::rand() % 100 < 5))
            {
                pop.wealth = static_cast<WealthLevel>(static_cast<uint8_t>(pop.wealth) + 1);
            }
        }
        else if (popBudget <= 0.0f || totalSatisfactionImpact < 0.0f)
        {
            if (pop.wealth != WealthLevel::Broke && (std::rand() % 100 < 15))
            {
                pop.wealth = static_cast<WealthLevel>(static_cast<uint8_t>(pop.wealth) - 1);
            }
        }
    }

    city.money += totalSalesValue;
}
/*
 * [PL] METODA: GrowPopulation
 * LOGIKA: Wprowadza na planszę nowe jednostki z puli urodzeń (imiona przez dasmig::ng).
 * POWIĄZANIA: GameManager.
 * [EN] METHOD: GrowPopulation
 * LOGIC: Spawns new demographic units assigning cultural names via dasmig::ng.
 * DEPENDENCIES: GameManager.
 */
void PopManager::GrowPopulation(int32_t growthAmount, uint16_t cultureID, uint8_t religionID, int32_t tileID, GameManager &gm, uint8_t empireCultureRaw)
{
    for (int32_t i = 0; i < growthAmount; ++i)
    {
        Pop baby;
        baby.locationTileID = tileID;
        baby.cultureID = static_cast<uint8_t>(cultureID);
        baby.religionID = religionID;
        baby.age = 0;
        baby.demographicsFlags = 0;
        baby.satisfaction = 150;
        baby.literacy = 0;
        baby.reserved = 0;

        if (std::rand() % 2 == 0)
        {
            baby.SetFemale();
        }

        baby.SetAssimilated(true);

        dasmig::gender popGender = baby.IsFemale() ? dasmig::gender::f : dasmig::gender::m;
        dasmig::culture popCulture = static_cast<dasmig::culture>(empireCultureRaw);

        std::wstring fullName = L"Anonim";
        try
        {
            fullName = gm.GetNameGenerator().get_name(popGender, popCulture).append_surname(popCulture);
        }
        catch (const std::exception &e)
        {
            std::cout << "[OSTRZEZENIE] Blad generatora: " << e.what() << " -> Nadano imie domyslne.\n";
        }

        uint32_t assignedNameID = gm.RegisterPopName(fullName);
        baby.nameSeed = static_cast<uint16_t>(assignedNameID > 65535 ? 65535 : assignedNameID);

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
/*
 * [PL] METODA: StarvePopulation
 * LOGIKA: Realizuje klęskę głodu – usuwa najbiedniejszych.
 * [EN] METHOD: StarvePopulation
 * LOGIC: Executes famine – removes the poorest from the population vector.
 */
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
/*
 * [PL] METODA: UpdateTurn
 * LOGIKA: Orkiestracja cyklu demograficznego (starzenie, rynek, głód, przyrost).
 * POWIĄZANIA: Spina działanie PopManager. Zależna od City.
 * [EN] METHOD: UpdateTurn
 * LOGIC: Orchestrates the demographic cycle (aging, market, famine, growth).
 * DEPENDENCIES: Ties the PopManager together. Relies on City.
 */
std::pair<float, float> PopManager::UpdateTurn(std::map<ResourceType, float> &marketSupplies, std::map<ResourceType, MarketCommodity> &market, uint16_t cultureID, uint8_t religionID, int32_t tileID, GameManager &gm, uint8_t empireCultureRaw, City &city, const std::map<SocialClass, float> &averageClassWages)
{
    std::cout << "\n=================== RAPORT GOSPODARCZY (Kafel: " << tileID << ") ===================" << std::endl;

    std::cout << "[MAGAZYN START] Zboze: " << marketSupplies[ResourceType::Grain]
              << " j. | Zloto: " << marketSupplies[ResourceType::Gold] << " j." << std::endl;

    size_t popBeforeLines = this->population.size();
    this->ProgressAgeAndMortality();
    size_t naturalDeaths = popBeforeLines - this->population.size();
    if (naturalDeaths > 0)
    {
        std::cout << "[KOSTUCHA] Ze starosci zmarlo: " << naturalDeaths << " popow." << std::endl;
    }

    float grainBefore = marketSupplies[ResourceType::Grain];
    float fishBefore = marketSupplies[ResourceType::Fish];
    this->ProcessMarketAndSatisfaction(marketSupplies, market, city, averageClassWages);
    float grainConsumed = grainBefore - marketSupplies[ResourceType::Grain];
    float fishConsumed = fishBefore - marketSupplies[ResourceType::Fish];

    std::cout << "[KONSUMPCJA] Popy kupily: " << grainConsumed << " Zboza i " << fishConsumed << " Ryb." << std::endl;
    std::cout << "[MAGAZYN KONIEC] Pozostalo -> Zboze: " << marketSupplies[ResourceType::Grain]
              << " j. | Ryby: " << marketSupplies[ResourceType::Fish] << " j." << std::endl;

    float avgSat = 0.0f;
    int32_t starvingPopsCount = 0;

    for (const auto &p : this->population)
    {
        avgSat += p.satisfaction;
        if (p.satisfaction < 50 || p.HasStarvingWarning())
            starvingPopsCount++;
    }
    avgSat = this->population.empty() ? 0.0f : (avgSat / this->population.size());

    std::cout << "[RYNEK] Srednie zadowolenie: " << static_cast<int>((avgSat / 255.0f) * 100.0f)
              << "% | Popy na skraju glodu/biedy: " << starvingPopsCount << "/" << this->population.size() << std::endl;

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
        int32_t potentialGrowth = this->CalculateGrowthPotential(city);
        if (potentialGrowth > 0)
        {
            size_t popBeforeGrowth = this->population.size();

            this->GrowPopulation(potentialGrowth, cultureID, religionID, tileID, gm, empireCultureRaw);
            std::cout << "[NARODZINY] Spoleczenstwo kwitnie! Urodzilo sie: " << potentialGrowth << " nowych popow." << std::endl;

            for (size_t i = popBeforeGrowth; i < this->population.size(); ++i)
            {
                const Pop &baby = this->population[i];
                std::cout << "  [+] Witamy na swiecie: ";
                std::wcout << gm.GetPopName(baby.nameSeed);
                std::cout << "\n";
            }
        }
    }

    std::map<SocialClass, std::map<WealthLevel, int32_t>> stratification;
    for (const auto &pop : this->population)
    {
        stratification[pop.socialClass][pop.wealth]++;
    }

    std::vector<std::pair<SocialClass, std::string>> classNames = {
        {SocialClass::Bound, "Bound"},
        {SocialClass::Laborer, "Laborer"},
        {SocialClass::Specialist, "Specialist"},
        {SocialClass::Capitalist, "Capitalist"},
        {SocialClass::Elite, "Elite"}};

    std::vector<std::pair<WealthLevel, std::string>> wealthNames = {
        {WealthLevel::Broke, "Broke"},
        {WealthLevel::Poor, "Poor"},
        {WealthLevel::Middle, "Middle"},
        {WealthLevel::Rich, "Rich"},
        {WealthLevel::FilthyRich, "FilthyRich"}};

    std::cout << "\n[SPIS LUDNOSCI - STRATYFIKACJA MAJATKOWA]" << std::endl;
    for (const auto &[sc, cName] : classNames)
    {
        int totalInClass = 0;
        for (const auto &[wl, wName] : wealthNames)
        {
            totalInClass += stratification[sc][wl];
        }

        if (totalInClass > 0)
        {
            std::cout << " -> " << cName << " (" << totalInClass << "): ";
            for (const auto &[wl, wName] : wealthNames)
            {
                if (stratification[sc][wl] > 0)
                {
                    std::cout << "[" << wName << ": " << stratification[sc][wl] << "] ";
                }
            }
            std::cout << std::endl;
        }
    }
    std::cout << "===================================================================\n"
              << std::endl;
    float totalPeopleCount = static_cast<float>(this->population.size());
    return {totalPeopleCount, avgSat};
}