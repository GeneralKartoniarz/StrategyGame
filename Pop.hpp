#pragma once
#include <cstdint>

enum class WealthLevel : uint8_t 
{
    Broke,
    Poor,
    Middle,
    Rich,
    FilthyRich
};

enum class PopGroup : uint8_t 
{
    None,
    Farmer,
    Worker,
    Intellectual,
    Noble
};

/*
 * Pamięciowy układ struktury (Data Alignment):
 * int32_t  (4 bajty) - ID kafelka
 * uint16_t (2 bajty) - ID imienia
 * uint16_t (2 bajty) - ID nazwiska
 * uint16_t (2 bajty) - ID kultury
 * uint16_t (2 bajty) - wiek
 * uint8_t  (1 bajt)  - zawód
 * uint8_t  (1 bajt)  - majątek
 * -----------------------------------------
 * Suma: 14 bajtów twardych danych + 2 bajty wyrównania.
 */
struct Pop 
{
    int32_t locationTileID;
    uint16_t firstNameID;
    uint16_t lastNameID;
    uint16_t cultureID;
    uint16_t age;
    PopGroup group;
    WealthLevel wealth;
};