#include "TestState.hpp"
#include "MapGenerator.hpp"
#include <cstdlib>
#include <ctime>

TestState::TestState(sf::RenderWindow *windowPtr) : States(windowPtr)
{
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    MapGenerator mg;
    this->map = mg.GetMap(1920, 1080, 15, 1);

    for (auto& region : this->map)
    {
        sf::Color regionColor(std::rand() % 150 + 50, 
                              std::rand() % 150 + 50, 
                              std::rand() % 150 + 50);
        for (auto& shape : region.subShapes)
        {
            shape.setFillColor(regionColor);
            shape.setOutlineThickness(0); 
        }
    }
}

void TestState::Update(float dt)
{
}

void TestState::Render(sf::RenderWindow *windowPtr)
{
    for (const auto& region : this->map)
    {
        for (const auto& shape : region.subShapes)
        {
            windowPtr->draw(shape);
        }
    }
}