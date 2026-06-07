#include "States/TestState.hpp"
#include "Map/MapGenerator.hpp"
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <SFML/Graphics.hpp>

sf::Color GetBiomeColor(BiomeType biome)
{
    switch (biome)
    {
    case BiomeType::Ocean:        return sf::Color(28, 81, 141);
    case BiomeType::IceSheet:     return sf::Color(230, 245, 250);
    case BiomeType::Tundra:       return sf::Color(145, 165, 140);
    case BiomeType::Desert:       return sf::Color(225, 190, 110);
    case BiomeType::Plains:       return sf::Color(120, 175, 90);
    case BiomeType::Forest:       return sf::Color(55, 120, 65);
    case BiomeType::Taiga:        return sf::Color(40, 95, 70);
    case BiomeType::Rainforest:   return sf::Color(15, 85, 45);
    case BiomeType::MountainPeak: return sf::Color(110, 115, 120);
    default:                      return sf::Color(100, 100, 100);
    }
}

TestState::TestState(sf::RenderWindow *windowPtr) : States(windowPtr)
{
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    ClimateConfig normalWorld;
    normalWorld.waterThreshold = -0.5f;
    normalWorld.plainsThreshold = 0.80f;
    normalWorld.hillsThreshold = 0.80f;
    normalWorld.coldThreshold = -0.70f;
    normalWorld.temperateThreshold = 0.5f;
    normalWorld.dryThreshold = -0.20f;
    normalWorld.normalThreshold = 1.35f;

    MapGenerator mg;
    this->map = mg.GetMap(1920, 1080, 5, 1, normalWorld);

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

        if (region.terrain.biome == BiomeType::Ocean)
        {
            finalColor.r = std::max(0, std::min(255, baseColor.r + brightnessAdjustment / 2));
            finalColor.g = std::max(0, std::min(255, baseColor.g + brightnessAdjustment / 2));
            finalColor.b = std::max(0, std::min(255, baseColor.b + brightnessAdjustment));
        }
        else
        {
            finalColor.r = std::max(0, std::min(255, baseColor.r + brightnessAdjustment));
            finalColor.g = std::max(0, std::min(255, baseColor.g + brightnessAdjustment));
            finalColor.b = std::max(0, std::min(255, baseColor.b + brightnessAdjustment));
        }

        for (const auto &poly : region.subPolygons)
        {
            size_t pointCount = poly.size();
            if (pointCount < 3) continue;

            sf::Vector2f p0 = poly[0];
            for (size_t i = 1; i < pointCount - 1; ++i)
            {
                this->terrainMesh.append(sf::Vertex{p0, finalColor});
                this->terrainMesh.append(sf::Vertex{poly[i], finalColor});
                this->terrainMesh.append(sf::Vertex{poly[i + 1], finalColor});
            }
        }
    }

    this->inputCtrl = std::make_unique<InputController>(windowPtr, this->map);
    this->gui = std::make_unique<GameInterface>(windowPtr);
}

void TestState::Update(float dt)
{
    sf::Vector2i mousePos = sf::Mouse::getPosition(*this->windowPtr);
    bool mouseClicked = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);
    int selectedID = this->inputCtrl->GetSelectedTileID();
    const Tile* selectedTile = nullptr;
    if (selectedID != -1 && static_cast<size_t>(selectedID) < this->map.size())
    {
        selectedTile = &this->map[selectedID];
    }
    this->gui->UpdateSelection(selectedTile);
    this->gui->Update(dt, mousePos, mouseClicked);

    if (!this->gui->IsMouseOverUI(mousePos))
    {
        this->inputCtrl->Update(dt);
    }
}

void TestState::HandleEvent(const sf::Event &event)
{
    if (const auto* mouseBtnDown = event.getIf<sf::Event::MouseButtonPressed>())
    {
        if (this->gui->IsMouseOverUI(mouseBtnDown->position)) return;
    }
    
    if (const auto* mouseBtnUp = event.getIf<sf::Event::MouseButtonReleased>())
    {
        if (this->gui->IsMouseOverUI(mouseBtnUp->position)) return;
    }
    
    if (const auto* mouseScroll = event.getIf<sf::Event::MouseWheelScrolled>())
    {
        if (this->gui->IsMouseOverUI(mouseScroll->position)) return;
    }

    this->inputCtrl->HandleEvent(event);
}

void TestState::Render(sf::RenderWindow *windowPtr)
{
    windowPtr->setView(this->inputCtrl->GetCamera());
    windowPtr->draw(this->terrainMesh);

    int selectedID = this->inputCtrl->GetSelectedTileID();
    if (selectedID != -1 && static_cast<size_t>(selectedID) < this->map.size())
    {
        const auto &selectedRegion = this->map[selectedID];
        sf::VertexArray highlightMesh(sf::PrimitiveType::Triangles);
        sf::Color highlightColor(255, 255, 255, 80);

        for (const auto &poly : selectedRegion.subPolygons)
        {
            size_t pointCount = poly.size();
            if (pointCount < 3) continue;

            sf::Vector2f p0 = poly[0];
            for (size_t i = 1; i < pointCount - 1; ++i)
            {
                highlightMesh.append(sf::Vertex{p0, highlightColor});
                highlightMesh.append(sf::Vertex{poly[i], highlightColor});
                highlightMesh.append(sf::Vertex{poly[i + 1], highlightColor});
            }
        }
        windowPtr->draw(highlightMesh);
    }

    windowPtr->draw(this->borderMesh);
    windowPtr->setView(windowPtr->getDefaultView());
    this->gui->Draw(windowPtr);
}