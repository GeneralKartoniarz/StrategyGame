#include "TestState.hpp"
#include "MapGenerator.hpp"
#include <cstdlib>
#include <ctime>
#include <SFML/Graphics.hpp>

sf::Color GetBiomeColor(BiomeType biome)
{
    switch (biome)
    {
        case BiomeType::Ocean:        return sf::Color(28, 81, 141);    // Głęboki niebieski
        case BiomeType::IceSheet:     return sf::Color(230, 245, 250);  // Mroźny biały
        case BiomeType::Tundra:       return sf::Color(145, 165, 140);  // Surowa, szara zieleń
        case BiomeType::Desert:       return sf::Color(225, 190, 110);  // Piaskowy żółty
        case BiomeType::Plains:       return sf::Color(120, 175, 90);   // Soczysta zieleń równin
        case BiomeType::Forest:       return sf::Color(55, 120, 65);    // Klasyczny las umiarkowany
        case BiomeType::Taiga:        return sf::Color(40, 95, 70);     // Chłodny, iglasty las
        case BiomeType::Rainforest:   return sf::Color(15, 85, 45);     // Gęsta dżungla
        case BiomeType::MountainPeak: return sf::Color(110, 115, 120);  // Skalisty szary
        default:                      return sf::Color(100, 100, 100);
    }
}

TestState::TestState(sf::RenderWindow *windowPtr) : States(windowPtr)
{
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    MapGenerator mg;
    this->map = mg.GetMap(1920, 1080, 6, 1);
    
    this->borderMesh.setPrimitiveType(sf::PrimitiveType::Lines);
    this->terrainMesh.setPrimitiveType(sf::PrimitiveType::Triangles);
    
    for (const auto &region : this->map)
    {
        for (const auto &borderVertex : region.provinceBorders)
        {
            this->borderMesh.append(borderVertex);
        }

        sf::Color baseColor = GetBiomeColor(region.terrain.biome);


        float heightFactor = region.terrain.elevationNoise; 
        
        int brightnessAdjustment = static_cast<int>(heightFactor * 30.0f);
        
        sf::Color finalColor;
        if (region.terrain.biome == BiomeType::Ocean) {
            finalColor.r = std::max(0, std::min(255, baseColor.r + brightnessAdjustment / 2));
            finalColor.g = std::max(0, std::min(255, baseColor.g + brightnessAdjustment / 2));
            finalColor.b = std::max(0, std::min(255, baseColor.b + brightnessAdjustment));
        } else {
            finalColor.r = std::max(0, std::min(255, baseColor.r + brightnessAdjustment));
            finalColor.g = std::max(0, std::min(255, baseColor.g + brightnessAdjustment));
            finalColor.b = std::max(0, std::min(255, baseColor.b + brightnessAdjustment));
        }

        for (const auto &shape : region.subShapes)
        {
            size_t pointCount = shape.getPointCount();
            if (pointCount < 3)
                continue;

            sf::Vector2f p0 = shape.getPoint(0);

            for (size_t i = 1; i < pointCount - 1; ++i)
            {
                sf::Vector2f p1 = shape.getPoint(i);
                sf::Vector2f p2 = shape.getPoint(i + 1);
                
                this->terrainMesh.append(sf::Vertex{p0, finalColor});
                this->terrainMesh.append(sf::Vertex{p1, finalColor});
                this->terrainMesh.append(sf::Vertex{p2, finalColor});
            }
        }
    }
}

void TestState::Update(float dt)
{
}

void TestState::Render(sf::RenderWindow *windowPtr)
{
    windowPtr->draw(this->terrainMesh);
    windowPtr->draw(this->borderMesh);
}