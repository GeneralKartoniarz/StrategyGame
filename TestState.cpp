#include "TestState.hpp"
#include "MapGenerator.hpp"
#include <cstdlib>
#include <ctime>
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
    this->camera = windowPtr->getDefaultView();
    this->camera.zoom(0.3f);
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    MapGenerator mg;
    this->map = mg.GetMap(1920, 1080, 3, 1);

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
            if (pointCount < 3)
                continue;

            sf::Vector2f p0 = poly[0];
            for (size_t i = 1; i < pointCount - 1; ++i)
            {
                sf::Vector2f p1 = poly[i];
                sf::Vector2f p2 = poly[i + 1];

                this->terrainMesh.append(sf::Vertex{p0, finalColor});
                this->terrainMesh.append(sf::Vertex{p1, finalColor});
                this->terrainMesh.append(sf::Vertex{p2, finalColor});
            }
        }
    }
}

void TestState::Update(float dt)
{
    if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Right))
    {
        sf::Vector2i currentMousePos = sf::Mouse::getPosition(*this->windowPtr);

        if (this->isDragging)
        {
            sf::Vector2f oldWorldPos = this->windowPtr->mapPixelToCoords(this->lastMousePos, this->camera);
            sf::Vector2f newWorldPos = this->windowPtr->mapPixelToCoords(currentMousePos, this->camera);

            sf::Vector2f delta = oldWorldPos - newWorldPos;
            this->camera.move(delta);
        }

        this->isDragging = true;
        this->lastMousePos = currentMousePos;
    }
    else
    {
        this->isDragging = false;
    }
    sf::Vector2f viewSize = this->camera.getSize();
    sf::Vector2f viewCenter = this->camera.getCenter();

    float minX = viewSize.x / 2.0f;
    float maxX = 1920.0f - minX;
    
    float minY = viewSize.y / 2.0f;
    float maxY = 1080.0f - minY;

    viewCenter.x = std::clamp(viewCenter.x, minX, maxX);
    viewCenter.y = std::clamp(viewCenter.y, minY, maxY);

    this->camera.setCenter(viewCenter);
}

void TestState::Render(sf::RenderWindow *windowPtr)
{
    windowPtr->setView(this->camera);

    windowPtr->draw(this->terrainMesh);
    windowPtr->draw(this->borderMesh);
    
    windowPtr->setView(windowPtr->getDefaultView());
}

void TestState::HandleEvent(const sf::Event& event)
{
    if (const auto* scrollEvent = event.getIf<sf::Event::MouseWheelScrolled>())
    {
        if (scrollEvent->wheel == sf::Mouse::Wheel::Vertical)
        {
            if (scrollEvent->delta > 0.0f)
            {
                this->camera.zoom(0.9f);
                if (this->camera.getSize().x < 50.0f)
                    this->camera.zoom(1.0f / 0.9f);
            }
            else
            {
                this->camera.zoom(1.1f);
                if (this->camera.getSize().x > 350.0f)
                    this->camera.zoom(1.0f / 1.1f);
            }
        }
    }
}