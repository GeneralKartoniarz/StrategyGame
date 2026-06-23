#include "City.hpp"
#include "Industry.hpp"
#include "Demographics.hpp"
#include "Pop.hpp"
#include "../Map/Tile.hpp"
#include <algorithm>
#include <iostream>
#include "PopManager.hpp"
/*
 * [PL] METODA: GetRequiredClassAsUint8
 * LOGIKA: Mapuje typ budynku produkcyjnego na docelową klasę społeczną wymaganą do pracy.
 * POWIĄZANIA: Industry.hpp (BuildingType), Demographics.hpp (SocialClass).
 * 
 * * [EN] METHOD: GetRequiredClassAsUint8
 * LOGIC: Maps the production building type to the target social class required for employment.
 * DEPENDENCIES: Industry.hpp (BuildingType), Demographics.hpp (SocialClass).
 */
uint8_t Manufacture::GetRequiredClassAsUint8() const
{
    if (type == BuildingType::Farm)
        return static_cast<uint8_t>(SocialClass::Bound);
    if (type == BuildingType::PaperMill)
        return static_cast<uint8_t>(SocialClass::Specialist);
    return static_cast<uint8_t>(SocialClass::Laborer);
}
/*
 * [PL] METODA: ProcessConstructionQueue
 * LOGIKA: Dekrementuje liczniki tur dla budynków i fizycznie stawia je na planszy.
 * POWIĄZANIA: Map/Tile.hpp (modyfikuje wektor manufactures docelowego kafelka).
 * 
 * * [EN] METHOD: ProcessConstructionQueue
 * LOGIC: Decrements turn counters for queued buildings and physically spawns/upgrades them.
 * DEPENDENCIES: Map/Tile.hpp (modifies the 'manufactures' vector of the target tile).
 */
void City::ProcessConstructionQueue(std::vector<Tile> &map)
{
    for (auto it = this->buildQueue.begin(); it != this->buildQueue.end();)
    {
        it->turnsLeft--;
        std::cout << "[KOLEJKA] Budynek na kafelku " << it->targetTileID
                  << " | Pozostalo tur: " << it->turnsLeft << std::endl;

        if (it->turnsLeft <= 0)
        {
            Tile &targetTile = map[it->targetTileID];
            bool upgraded = false;

            for (auto &m : targetTile.manufactures)
            {
                if (m.type == it->type)
                {
                    m.level++;
                    upgraded = true;
                    break;
                }
            }

            if (!upgraded)
            {
                Manufacture newBuilding;
                newBuilding.type = it->type;
                newBuilding.level = 1;
                targetTile.manufactures.push_back(newBuilding);
            }

            std::cout << "[BUDOWNICTWO] Ukonczono wznoszenie manufaktury na kafelku: "
                      << it->targetTileID << "!" << std::endl;

            it = this->buildQueue.erase(it);
        }
        else
        {
            ++it;
        }
    }
}
/*
 * [PL] METODA: CollectWorkplacesFromTerritory
 * LOGIKA: Agreguje manufaktury z terytorium miasta i konwertuje je na listę miejsc pracy.
 * POWIĄZANIA: Map/Tile.hpp (odczytuje manufactures z kafelków jurysdykcji).
 * 
 * * [EN] METHOD: CollectWorkplacesFromTerritory
 * LOGIC: Aggregates manufactures from the city's territory into a list of available Workplaces.
 * DEPENDENCIES: Map/Tile.hpp (reads 'manufactures' from jurisdiction tiles).
 */
void City::CollectWorkplacesFromTerritory(const std::vector<Tile> &map)
{
    this->workplaces.clear();
    for (int32_t tileID : this->jurisdictionTiles)
    {
        const Tile &tile = map[tileID];
        for (const auto &manufacture : tile.manufactures)
        {
            Workplace wp;
            wp.producedResource = manufacture.GetOutputResource();
            wp.requiredClassRaw = manufacture.GetRequiredClassAsUint8();
            wp.maxEmployees = manufacture.GetMaxJobs();
            wp.currentEmployees = 0;
            this->workplaces.push_back(wp);
        }
    }
}
/*
 * [PL] METODA: PerformEmploymentRegistry
 * LOGIKA: Centralny urząd pracy. Przypisuje obywateli do miejsc pracy.
 * [DO ZMIANY]: Kalkulacja państwowych płac (promisedWages) wyleci – fabryki będą 
 * płacić z dywidend.
 * POWIĄZANIA: PopManager (pobiera referencje do obywateli).
 * 
 * * [EN] METHOD: PerformEmploymentRegistry
 * LOGIC: Central employment office. Assigns citizens to available jobs.
 * [TO CHANGE]: State-funded wage calculation (promisedWages) will be removed. 
 * Factories will pay from market dividends.
 * DEPENDENCIES: PopManager (fetches and modifies citizen references).
 */
void City::PerformEmploymentRegistry(PopManager &popManager)
{
    std::vector<Pop>& allPops = popManager.GetPopulationRef();
    std::map<uint8_t, std::vector<Pop*>> availableWorkers;

    int32_t childrenCount = 0;
    int32_t mothersOnLeave = 0;

    for (auto& pop : allPops)
    {
        if (std::find(this->jurisdictionTiles.begin(), this->jurisdictionTiles.end(), pop.locationTileID) == this->jurisdictionTiles.end())
            continue;

        pop.SetEmployed(false);

        if (pop.age < 16)
        {
            childrenCount++;
        }
    }

    for (auto& pop : allPops)
    {
        if (std::find(this->jurisdictionTiles.begin(), this->jurisdictionTiles.end(), pop.locationTileID) == this->jurisdictionTiles.end())
            continue;

        if (pop.age < 16) 
            continue;

        if (pop.IsFemale() && mothersOnLeave < childrenCount)
        {
            mothersOnLeave++;
            continue; 
        }

        availableWorkers[static_cast<uint8_t>(pop.socialClass)].push_back(&pop);
    }

    for (auto& [classRaw, pool] : availableWorkers)
    {
        std::sort(pool.begin(), pool.end(), [](const Pop* a, const Pop* b) {
            return a->literacy < b->literacy; 
        });
    }

    this->promisedWages = 0.0f;
    float baseWage = 4.0f; 

    for (auto &job : this->workplaces)
    {
        job.currentEmployees = 0;
        int32_t remainingJobs = job.maxEmployees;

        auto assignWorkers = [&](uint8_t classRaw, bool isFallback) {
            auto& pool = availableWorkers[classRaw];
            int32_t recruits = 0;

            while (remainingJobs > 0 && !pool.empty())
            {
                Pop* p = pool.back();
                pool.pop_back();

                p->SetEmployed(true);

                float classMult = 1.0f;
                if (p->socialClass == SocialClass::Bound) classMult = 0.5f;
                else if (p->socialClass == SocialClass::Specialist) classMult = 3.0f;
                else if (p->socialClass == SocialClass::Capitalist) classMult = 8.0f;
                else if (p->socialClass == SocialClass::Elite) classMult = 15.0f;

                this->promisedWages += baseWage * classMult * (1.0f + (p->literacy / 32.0f));

                recruits++;
                remainingJobs--;
                job.currentEmployees++;
            }

            if (isFallback && recruits > 0)
            {
                std::cout << "[URZĄD PRACY] Przekwalifikowano " << recruits 
                          << " popów z klasy " << static_cast<int>(classRaw) 
                          << " do pracy typu " << MarketRegistry::GetResourceName(job.producedResource) << std::endl;
            }
        };

        assignWorkers(job.requiredClassRaw, false);

        if (remainingJobs > 0)
        {
            std::vector<uint8_t> backupClasses;

            if (job.requiredClassRaw == static_cast<uint8_t>(SocialClass::Bound))
            {
                backupClasses = { static_cast<uint8_t>(SocialClass::Laborer), static_cast<uint8_t>(SocialClass::Specialist) };
            }
            else if (job.requiredClassRaw == static_cast<uint8_t>(SocialClass::Laborer))
            {
                backupClasses = { static_cast<uint8_t>(SocialClass::Bound), static_cast<uint8_t>(SocialClass::Specialist) };
            }

            for (uint8_t backupClassRaw : backupClasses)
            {
                if (remainingJobs <= 0) break;
                assignWorkers(backupClassRaw, true);
            }
        }
    }
}
/*
 * [PL] METODA: SimulateProduction
 * LOGIKA: Przetwarza surowce w fabrykach na podstawie wydajności zatrudnionych popów.
 * [DO ZMIANY]: Będzie obliczać zyski ze sprzedaży by zasilić pulę pensji konkretnego Workplace.
 * POWIĄZANIA: Map/Tile.hpp, Industry.hpp.
 * 
 * * [EN] METHOD: SimulateProduction
 * LOGIC: Transforms input resources into outputs based on the efficiency of employed pops.
 * [TO CHANGE]: Will calculate market sales profits to feed the wage pool of a specific Workplace.
 * DEPENDENCIES: Map/Tile.hpp, Industry.hpp.
 */
void City::SimulateProduction(const std::vector<Tile> &map)
{
    std::cout << "--- BILANS PRODUKCJI MIASTA ---" << std::endl;

    float requiredGrainForPops = 0.0f;
    int32_t totalLocalWorkers = 0;
    for (const auto& job : this->workplaces)
    {
        if (job.producedResource == ResourceType::Grain || job.producedResource == ResourceType::Fish)
        {
            totalLocalWorkers += job.currentEmployees;
        }
    }
    float grainSafetyBuffer = static_cast<float>(totalLocalWorkers) * 1.5f;
    if (grainSafetyBuffer <= 0.0f) grainSafetyBuffer = 20.0f;

    float availableGrainForIndustry = std::max(0.0f, this->warehouse[ResourceType::Grain] - grainSafetyBuffer);
    
    std::cout << "[BEZPIECZNIK] Łącznie zboża w magazynie: " << this->warehouse[ResourceType::Grain] << " j." << std::endl;
    std::cout << "[BEZPIECZNIK] Zablokowano do spichlerza (rezerwa głodowa): " << grainSafetyBuffer << " j." << std::endl;
    std::cout << "[BEZPIECZNIK] Wolne zboże przekazane dla przemysłu: " << availableGrainForIndustry << " j." << std::endl;


    size_t wpIndex = 0;
    for (int32_t tileID : this->jurisdictionTiles)
    {
        const Tile &tile = map[tileID];
        for (const auto &manufacture : tile.manufactures)
        {
            if (wpIndex >= this->workplaces.size())
                break;
            Workplace &job = this->workplaces[wpIndex++];

            if (job.currentEmployees <= 0)
                continue;

            ResourceType inputType;
            float inputAmountPerWorker;
            manufacture.GetInputRequirements(inputType, inputAmountPerWorker);

            if (inputAmountPerWorker == 0.0f)
            {
                float baseOutput = static_cast<float>(job.currentEmployees) * 2.0f;

                if (manufacture.type == BuildingType::Farm && tile.terrain.resourceName == "Żyzna Gleba")
                {
                    baseOutput *= 2.0f;
                }

                this->warehouse[job.producedResource] += baseOutput;
                std::cout << " -> [" << tile.terrain.resourceName << "] Wyprodukowano: " << baseOutput
                          << " szt. " << MarketRegistry::GetResourceName(job.producedResource) << std::endl;
            }
            else
            {
                float requiredInput = static_cast<float>(job.currentEmployees) * inputAmountPerWorker;
                float usableInputPool = this->warehouse[inputType];

                if (inputType == ResourceType::Grain)
                {
                    usableInputPool = availableGrainForIndustry;
                }

                float efficiency = 1.0f;
                if (usableInputPool < requiredInput)
                {
                    efficiency = (requiredInput > 0.0f) ? (usableInputPool / requiredInput) : 0.0f;
                    requiredInput = usableInputPool;
                }

                this->warehouse[inputType] -= requiredInput;
                if (inputType == ResourceType::Grain)
                {
                    availableGrainForIndustry -= requiredInput;
                }

                float totalGenerated = static_cast<float>(job.currentEmployees) * 0.5f * efficiency;
                this->warehouse[job.producedResource] += totalGenerated;

                std::cout << " -> Przetworzono " << requiredInput << " szt. "
                          << MarketRegistry::GetResourceName(inputType) << " na " << totalGenerated
                          << " szt. " << MarketRegistry::GetResourceName(job.producedResource) 
                          << " (Wydajność: " << static_cast<int>(efficiency * 100.0f) << "%)" << std::endl;
            }
        }
    }
}