#include "TestState.hpp"
#include "MapGenerator.hpp"

TestState::TestState(sf::RenderWindow *windowPtr) : States(windowPtr)
{
    MapGenerator mg;
    this->map = mg.GenerateSeeds(1920, 1080, 120);
}

void TestState::Update(float dt)
{
}

void TestState::Render(sf::RenderWindow *windowPtr)
{
    for (size_t i = 0; i < this->map.size(); i++)
    {
        sf::ConvexShape polygon;
        polygon.setPointCount(this->map[i].vertices.size());
        for (int j = 0; j < this->map[i].vertices.size(); j++)
        {
            polygon.setPoint(j, this->map[i].vertices[j]);
        }
        polygon.setFillColor(sf::Color::Transparent);
        polygon.setOutlineColor(sf::Color::Red);
        polygon.setOutlineThickness(3);
        windowPtr->draw(polygon);
    }
}