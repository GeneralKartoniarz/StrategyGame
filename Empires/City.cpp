#include "City.hpp"
#include "Industry.hpp"
#include "Demographics.hpp"
#include "Pop.hpp"
#include "../Map/Tile.hpp"
#include <algorithm>
#include <iostream>

uint8_t Manufacture::GetRequiredClassAsUint8() const
{
    if (type == BuildingType::Farm)
        return static_cast<uint8_t>(SocialClass::Bound);
    if (type == BuildingType::PaperMill)
        return static_cast<uint8_t>(SocialClass::Specialist);
    return static_cast<uint8_t>(SocialClass::Laborer);
}

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

void City::PerformEmploymentRegistry(const std::vector<Pop> &empirePops)
{
    std::map<uint8_t, int32_t> availableWorkers;
    for (const auto &pop : empirePops)
    {
        auto it = std::find(this->jurisdictionTiles.begin(), this->jurisdictionTiles.end(), pop.locationTileID);
        if (it != this->jurisdictionTiles.end() && pop.age >= 15)
        {
            availableWorkers[static_cast<uint8_t>(pop.socialClass)]++;
        }
    }

    for (auto &job : this->workplaces)
    {
        job.currentEmployees = 0;
        int32_t pool = availableWorkers[job.requiredClassRaw];
        if (pool <= 0)
            continue;

        int32_t recruits = std::min(job.maxEmployees, pool);
        job.currentEmployees = recruits;
        availableWorkers[job.requiredClassRaw] -= recruits;
    }
}

void City::SimulateProduction(const std::vector<Tile> &map)
{
    std::cout << "--- BILANS PRODUKCJI MIASTA ---" << std::endl;
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
                float availableInput = this->warehouse[inputType];
                float efficiency = 1.0f;

                if (availableInput < requiredInput)
                {
                    efficiency = (requiredInput > 0.0f) ? (availableInput / requiredInput) : 0.0f;
                    requiredInput = availableInput;
                }

                this->warehouse[inputType] -= requiredInput;
                float totalGenerated = static_cast<float>(job.currentEmployees) * 0.5f * efficiency;
                this->warehouse[job.producedResource] += totalGenerated;

                std::cout << " -> Przetworzono " << requiredInput << " szt. "
                          << MarketRegistry::GetResourceName(inputType) << " na " << totalGenerated
                          << " szt. " << MarketRegistry::GetResourceName(job.producedResource) << std::endl;
            }
        }
    }
}