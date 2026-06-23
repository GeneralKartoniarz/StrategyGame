#pragma once
#include <cstdint>
#include <map>
#include "Economy.hpp"

/*
 * [PL] STRUKTURA/ENUM: DemographicsConfig / SocialClass / WealthLevel
 * LOGIKA: Definiuje drabinkę społeczną, klasy majątkowe oraz bazowe zapotrzebowanie 
 * kaloryczne i luksusowe dla każdej z warstw.
 * POWIĄZANIA: Używane bezpośrednio przez PopManager do kalkulacji zadowolenia i awansów.
 * * [EN] STRUCTURE/ENUM: DemographicsConfig / SocialClass / WealthLevel
 * LOGIC: Defines the social ladder, wealth classes, and base caloric/luxury needs 
 * for each stratum.
 * DEPENDENCIES: Used directly by PopManager to calculate satisfaction and class promotion.
 */

enum class WealthLevel : uint8_t
{
    Broke,
    Poor,
    Middle,
    Rich,
    FilthyRich
};

enum class SocialClass : uint8_t
{
    Bound,
    Laborer,
    Specialist,
    Capitalist,
    Elite
};

struct MarketNeed
{
    float baseDemandPerCapita;
    bool isCritical;
};

class DemographicsConfig
{
public:
    static float GetIncomeForClass(SocialClass cl)
    {
        switch (cl)
        {
        case SocialClass::Bound:
            return 1.5f;
        case SocialClass::Laborer:
            return 3.5f;
        case SocialClass::Specialist:
            return 8.0f;
        case SocialClass::Capitalist:
            return 20.0f;
        case SocialClass::Elite:
            return 50.0f;
        }
        return 1.0f;
    }
    static std::map<NeedCategory, MarketNeed> GetNeedsForClass(SocialClass cl)
    {
        std::map<NeedCategory, MarketNeed> needs;
        switch (cl)
        {
        // TODO POPY MAJĄ WYMAGANIA INACZEJ SO WKURWIONE
        case SocialClass::Bound:
            needs[NeedCategory::Calories] = {1.0f, true};
            needs[NeedCategory::Protection] = {0.5f, false};
            break;
        case SocialClass::Laborer:
            needs[NeedCategory::Calories] = {1.0f, true};
            needs[NeedCategory::Protection] = {0.6f, false};
            needs[NeedCategory::DietVariety] = {0.4f, false};
            needs[NeedCategory::Stimulants] = {0.3f, false};
            break;
        case SocialClass::Specialist:
            needs[NeedCategory::Calories] = {1.0f, true};
            needs[NeedCategory::Protection] = {0.8f, true};
            needs[NeedCategory::DietVariety] = {0.8f, false};
            needs[NeedCategory::Education] = {0.5f, false};
            break;
        case SocialClass::Capitalist:
            needs[NeedCategory::Calories] = {1.0f, true};
            needs[NeedCategory::DietVariety] = {1.2f, false};
            needs[NeedCategory::Stimulants] = {0.8f, false};
            needs[NeedCategory::StatusGoods] = {1.0f, false};
            break;
        case SocialClass::Elite:
            needs[NeedCategory::Calories] = {1.0f, true};
            needs[NeedCategory::DietVariety] = {1.5f, false};
            needs[NeedCategory::Education] = {1.0f, false};
            needs[NeedCategory::StatusGoods] = {2.0f, false};
            break;
        }
        return needs;
    }
};