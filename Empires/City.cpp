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
    if (type == BuildingType::SawMill)
        return static_cast<uint8_t>(SocialClass::Laborer);
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
 * POWIĄZANIA: PopManager (pobiera referencje do obywateli).
 *
 * * [EN] METHOD: PerformEmploymentRegistry
 * LOGIC: Central employment office. Assigns citizens to available jobs.
 * DEPENDENCIES: PopManager (fetches and modifies citizen references).
 */
void City::PerformEmploymentRegistry(PopManager &popManager)
{
    for (auto &job : this->workplaces)
    {
        job.currentEmployees = 0;
    }

    std::map<SocialClass, std::vector<Pop *>> availableWorkers;

    for (auto &pop : popManager.GetPopulationRef())
    {
        auto it = std::find(this->jurisdictionTiles.begin(), this->jurisdictionTiles.end(), pop.locationTileID);
        if (it != this->jurisdictionTiles.end())
        {
            if (pop.age >= 16 && !pop.IsFemale())
            {
                pop.SetEmployed(false);
                availableWorkers[pop.socialClass].push_back(&pop);
            }
        }
    }
    for (auto &job : this->workplaces)
    {
        SocialClass reqClass = static_cast<SocialClass>(job.requiredClassRaw);

        while (job.currentEmployees < job.maxEmployees)
        {
            Pop *hiredWorker = nullptr;

            if (!availableWorkers[reqClass].empty())
            {
                hiredWorker = availableWorkers[reqClass].back();
                availableWorkers[reqClass].pop_back();
            }
            else if (reqClass == SocialClass::Bound && !availableWorkers[SocialClass::Laborer].empty())
            {
                hiredWorker = availableWorkers[SocialClass::Laborer].back();
                availableWorkers[SocialClass::Laborer].pop_back();
            }
            else
            {
                break;
            }

            hiredWorker->SetEmployed(true);
            job.currentEmployees++;
        }
    }
}
void City::RecordTurnStatistics(float totalPop, float avgSat)
{
    EconomySample sample;
    sample.population = totalPop;
    sample.treasury = this->money;
    sample.averageSatisfaction = avgSat;
    sample.foodSupply = this->warehouse[ResourceType::Grain] + this->warehouse[ResourceType::Fish];

    if (this->economyHistory.size() >= MAX_HISTORY_SAMPLES)
    {
        this->economyHistory.erase(this->economyHistory.begin());
    }
    this->economyHistory.push_back(sample);
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
void City::SimulateProduction(const std::vector<Tile> &map, std::map<ResourceType, MarketCommodity> &market)
{
    std::cout << "--- BILANS PRODUKCJI MIASTA (KAPITALIZM) ---" << std::endl;

    float totalLocalWorkers = 0.0f;
    for (const auto &job : this->workplaces)
    {
        if (job.producedResource == ResourceType::Grain || job.producedResource == ResourceType::Fish)
        {
            totalLocalWorkers += static_cast<float>(job.currentEmployees);
        }
    }

    float grainSafetyBuffer = totalLocalWorkers * 1.5f;
    if (grainSafetyBuffer <= 0.0f)
        grainSafetyBuffer = 20.0f;

    float availableGrainForIndustry = std::max(0.0f, this->warehouse[ResourceType::Grain] - grainSafetyBuffer);

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

            float totalGenerated = 0.0f;

            if (inputAmountPerWorker == 0.0f)
            {
                totalGenerated = static_cast<float>(job.currentEmployees) * 5.0f;

                if ((manufacture.type == BuildingType::Farm || manufacture.type == BuildingType::SawMill) && tile.terrain.resourceName == "Żyzna Gleba")
                {
                    totalGenerated *= 2.0f;
                }
                // TODO DO ZMIANY
                if (manufacture.type == BuildingType::SawMill)
                {
                    totalGenerated *= 5;
                }
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

                totalGenerated = static_cast<float>(job.currentEmployees) * 0.5f * efficiency;
            }

            if (totalGenerated > 0.0f)
            {
                this->warehouse[job.producedResource] += totalGenerated;
                market[job.producedResource].productionLastTurn += totalGenerated;
                float unitPrice = market[job.producedResource].currentPrice;
                float batchValue = totalGenerated * unitPrice;

                job.revenuePool += batchValue;

                std::cout << " -> [" << BuildingRegistry::GetBuildingName(manufacture.type)
                          << "] Wyprodukowano: " << totalGenerated << " szt. Wartość: "
                          << batchValue << " (Cena: " << unitPrice << ")" << std::endl;
            }
        }
    }
}
std::map<SocialClass, float> City::DistributeWages()
{
    constexpr float taxRate = 0.15f;
    constexpr float maintenanceRate = 0.10f;

    std::map<SocialClass, float> accumulatedWagePools;
    std::map<SocialClass, int32_t> totalWorkersPerClass;

    for (auto &job : this->workplaces)
    {
        if (job.currentEmployees <= 0 || job.revenuePool <= 0.0f)
            continue;

        float totalTax = job.revenuePool * taxRate;
        float totalMaintenance = job.revenuePool * maintenanceRate;
        float netWagePool = job.revenuePool - totalTax - totalMaintenance;

        this->money += totalTax;

        SocialClass workerClass = static_cast<SocialClass>(job.requiredClassRaw);
        accumulatedWagePools[workerClass] += netWagePool;
        totalWorkersPerClass[workerClass] += job.currentEmployees;

        job.revenuePool = 0.0f;
    }

    std::map<SocialClass, float> averageClassWages;
    for (const auto &[sClass, pool] : accumulatedWagePools)
    {
        int32_t workers = totalWorkersPerClass[sClass];
        if (workers > 0)
        {
            averageClassWages[sClass] = pool / static_cast<float>(workers);
        }
    }

    return averageClassWages;
}