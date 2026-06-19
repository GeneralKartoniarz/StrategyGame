#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <utility>

enum class ResourceType : uint8_t
{
    Grain,
    Fish,
    Meat,
    Wood,
    Coal,
    IronOre,
    Gold,
    Fur,
    Cocoa,
    Silk,
    Paper,
    LuxuryAlcohol 
};

enum class NeedCategory : uint8_t
{
    Calories,       // Podstawowe kalorie
    Protection,     // Ochrona przed naturą (ogrzewanie)
    DietVariety,    // Zróżnicowana dieta
    Stimulants,     // Używki
    Education,      // Edukacja
    StatusGoods     // Dobra luksusowe
};

struct MarketRegistry
{
    static std::vector<std::pair<ResourceType, float>> GetSubstitutes(NeedCategory category)
    {
        switch (category)
        {
            case NeedCategory::Calories:
                return { {ResourceType::Grain, 1.0f}, {ResourceType::Fish, 0.8f} };
            case NeedCategory::Protection:
                return { {ResourceType::Wood, 1.0f}, {ResourceType::Coal, 2.0f} };
            case NeedCategory::DietVariety:
                return { {ResourceType::Meat, 1.0f}, {ResourceType::Cocoa, 0.5f} };
            case NeedCategory::Stimulants:
                return { {ResourceType::LuxuryAlcohol, 1.0f} };
            case NeedCategory::Education:
                return { {ResourceType::Paper, 1.0f} };
            case NeedCategory::StatusGoods:
                return { {ResourceType::Gold, 1.0f}, {ResourceType::Silk, 1.5f}, {ResourceType::Fur, 0.8f} };
        }
        return {};
    }

    static std::string GetResourceName(ResourceType type)
    {
        switch (type)
        {
            case ResourceType::Grain:         return "Zboze";
            case ResourceType::Fish:          return "Ryby";
            case ResourceType::Meat:          return "Mieso";
            case ResourceType::Wood:          return "Drewno";
            case ResourceType::Coal:          return "Wegiel";
            case ResourceType::IronOre:       return "Ruda Zelaza";
            case ResourceType::Gold:          return "Zloto";
            case ResourceType::Fur:           return "Futra";
            case ResourceType::Cocoa:         return "Kakao";
            case ResourceType::Silk:          return "Jedwab";
            case ResourceType::Paper:         return "Papier";
            case ResourceType::LuxuryAlcohol: return "Drogi Alkohol";
        }
        return "Nieznany";
    }
};