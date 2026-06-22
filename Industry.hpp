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
struct ConstructionTask
{
    BuildingType type;
    int32_t targetTileID;
    int32_t turnsLeft;
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
            case BuildingType::Farm:       return level * 3;
            case BuildingType::Fishery:    return level * 3;
            case BuildingType::Mine:       return level * 5;
            case BuildingType::Distillery: return level * 1;
            case BuildingType::PaperMill:  return level * 1;
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
enum class BiomeType; 

class BuildingRegistry
{
public:
    static bool IsBiomeAllowed(BuildingType bType, BiomeType biome);

    static std::string GetBuildingName(BuildingType bType);
};