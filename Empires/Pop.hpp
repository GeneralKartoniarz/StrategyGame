#pragma once
#include <cstdint>
#include "../Demographics.hpp"
enum class ReligionGroup : uint8_t
{
    None
};
/*
 * [PL] STRUKTURA: Pop
 * LOGIKA: Reprezentacja pojedynczej grupy ludności (popu) zoptymalizowana do rygorystycznego
 * limitu 16 bajtów. Stanowi fundament systemu demograficznego.
 * POWIĄZANIA: Wykorzystuje typy wyliczeniowe z Demographics.hpp (SocialClass, WealthLevel).
 * Rozszerzona o flagi bitowe do obsługi płci, zatrudnienia i ostrzeżeń o głodzie.
 * 
 * * [EN] STRUCTURE: Pop
 * LOGIC: Representation of a single population group (pop) optimized to a strict 
 * 16-byte limit. Forms the foundation of the demographic system.
 * DEPENDENCIES: Uses enums from Demographics.hpp (SocialClass, WealthLevel). 
 * Extended with bit flags to handle gender (as previously requested, 
 * no RNG needed for namegen), employment, and starvation warnings.
 */
#pragma pack(push, 1)
struct Pop
{
    int32_t locationTileID; // 4 bajty [0-3]

    uint16_t nameSeed;  // 2 bajty [4-5] -> Zastępuje firstNameID i lastNameID. Losujesz z tego imię i nazwisko generatorem pseudolosowym!
    uint8_t cultureID;  // 1 bajt  [6]   -> Max 256 unikalnych kultur w grze
    uint8_t religionID; // 1 bajt  [7]   -> Max 256 unikalnych religii w grze

    uint16_t age;            // 2 bajty [8-9] -> Spokojnie mieści wiek popu
    SocialClass socialClass; // 1 bajt  [10]  -> Nowe klasy społeczne
    WealthLevel wealth;      // 1 bajt  [11]  -> Poziom majątku

    uint8_t literacy;          // 1 bajt  [12]  -> Alfabetyzacja (w skali 0 - 32)
    uint8_t satisfaction;      // 1 bajt  [13]  -> Satysfakcja (0 - 255)
    uint8_t demographicsFlags; // 1 bajt  [14]  -> Flagi bitowe (Płeć, Asymilacja)
    uint8_t reserved;          // 1 bajt  [15]  -> Wolny bajt techniczny

    inline bool IsFemale() const { return (demographicsFlags & 0x01) != 0; }
    inline bool IsMale() const { return (demographicsFlags & 0x01) == 0; }
    inline bool IsAssimilated() const { return (demographicsFlags & 0x02) != 0; }

    inline void SetFemale() { demographicsFlags |= 0x01; }
    inline void SetMale() { demographicsFlags &= ~0x01; }
    inline void SetAssimilated(bool v) { demographicsFlags = v ? (demographicsFlags | 0x02) : (demographicsFlags & ~0x02); }

    inline bool IsEmployed() const { return (demographicsFlags & 0x04) != 0; }
    inline void SetEmployed(bool v) { demographicsFlags = v ? (demographicsFlags | 0x04) : (demographicsFlags & ~0x04); }

    inline bool HasStarvingWarning() const { return (demographicsFlags & 0x08) != 0; }
    inline void SetStarvingWarning(bool v) { demographicsFlags = v ? (demographicsFlags | 0x08) : (demographicsFlags & ~0x08); }
};
#pragma pack(pop)

static_assert(sizeof(Pop) == 16, "Struktura Pop przekroczyła limit 16 bajtów!");