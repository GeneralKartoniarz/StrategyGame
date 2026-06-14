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
    Slave,          
    Serf,           
    FreeFarmer,   
    Worker,        
    Intellectual,   
    Noble 
};
enum class ReligionGroup : uint8_t
{
    None
};
struct Pop 
{
    int32_t locationTileID;
    uint16_t firstNameID;    
    uint16_t lastNameID;    
    uint16_t cultureID;     
    uint16_t age;           
    PopGroup group;         
    WealthLevel wealth;      
    ReligionGroup religion;  
    
    uint8_t demographicsFlags; 
    
    uint8_t satisfaction;

    inline bool IsFemale() const { return (demographicsFlags & 0x01) != 0; }
    inline bool IsMale() const { return (demographicsFlags & 0x01) == 0; }
    inline bool IsAssimilated() const { return (demographicsFlags & 0x02) != 0; }

    inline void SetFemale() { demographicsFlags |= 0x01; }
    inline void SetMale() { demographicsFlags &= ~0x01; }
    inline void SetAssimilated(bool v) { v ? demographicsFlags |= 0x02 : demographicsFlags &= ~0x02; }
};