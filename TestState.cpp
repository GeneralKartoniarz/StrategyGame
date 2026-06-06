#include "TestState.hpp"
#include "MapGenerator.hpp"

TestState::TestState(sf::RenderWindow *windowPtr) : States(windowPtr)
{
    MapGenerator mg;
    this->map = mg.GetMap(1920, 1080, 60, 1);
    this->map[106].shape.setFillColor(sf::Color::Blue);
    for (int i = 0; i < this->map[106].neighbors.size(); i++)
    {
        this->map[this->map[106].neighbors[i]].shape.setFillColor(sf::Color::Red);
    }
    
}

void TestState::Update(float dt)
{
}

void TestState::Render(sf::RenderWindow *windowPtr)
{
    for (size_t i = 0; i < this->map.size(); i++)
    {
        windowPtr->draw(this->map[i].shape);
    }
}