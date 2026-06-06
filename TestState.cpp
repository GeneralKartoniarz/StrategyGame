#include "TestState.hpp"
#include "MapGenerator.hpp"
#include <cstdlib>
#include <ctime>
#include <SFML/Graphics.hpp>
TestState::TestState(sf::RenderWindow *windowPtr) : States(windowPtr)
{
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    MapGenerator mg;
    this->map = mg.GetMap(1920, 1080, 20, 1);
    this->borderMesh.setPrimitiveType(sf::PrimitiveType::Lines);
    this->terrainMesh.setPrimitiveType(sf::PrimitiveType::Triangles);
    for (const auto &region : this->map)
    {
        for (const auto &borderVertex : region.provinceBorders)
        {
            this->borderMesh.append(borderVertex);
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
                this->terrainMesh.append(sf::Vertex{p0, sf::Color(150,0,20)});
                this->terrainMesh.append(sf::Vertex{p1, sf::Color(150,0,20)});
                this->terrainMesh.append(sf::Vertex{p2, sf::Color(150,0,20)});
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