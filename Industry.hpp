#pragma once
#include <cstdint>
#include "Economy.hpp"

enum class BuildingType : uint8_t
{
    None,
    Farm,
    Fishery,
    Mine,
    Distillery,
    PaperMill
};

enum class SocialClass : uint8_t;

struct Manufacture
{
    BuildingType type = BuildingType::None;
    uint8_t level = 0;

    int32_t GetMaxJobs() const
    {
        switch (type)
        {
            case BuildingType::Farm:       return level * 100;
            case BuildingType::Fishery:    return level * 60;
            case BuildingType::Mine:       return level * 80;
            case BuildingType::Distillery: return level * 25;
            case BuildingType::PaperMill:  return level * 15;
            default: return 0;
        }
    }

    uint8_t GetRequiredClassAsUint8() const;

    ResourceType GetOutputResource() const
    {
        switch (type)
        {
            case BuildingType::Farm:       return ResourceType::Grain;
            case BuildingType::Fishery:    return ResourceType::Fish;
            case BuildingType::Mine:       return ResourceType::Coal; 
            case BuildingType::Distillery: return ResourceType::LuxuryAlcohol;
            case BuildingType::PaperMill:  return ResourceType::Paper;
            default:                       return ResourceType::Grain;
        }
    }

    void GetInputRequirements(ResourceType& outType, float& outAmountPerWorker) const
    {
        switch (type)
        {
            case BuildingType::Distillery:
                outType = ResourceType::Grain; outAmountPerWorker = 0.5f; break;
            case BuildingType::PaperMill:
                outType = ResourceType::Wood; outAmountPerWorker = 1.0f; break;
            default:
                outType = ResourceType::Grain; outAmountPerWorker = 0.0f; break;
        }
    }
};