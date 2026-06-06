#include "TestState.hpp"
#include "Tile.hpp"
#include "vector"
using namespace std;

TestState::TestState(sf::RenderWindow *windowPtr) : States(windowPtr)
{
    this->CreateTestGraph();
}

void TestState::Update(float dt)
{
}
void TestState::CreateTestGraph()
{
    for (int i = 0; i < 10; i++)
    {
        Tile t;
        t.ID = i;
        t.position = sf::Vector2f(100.f + i * 150.f, 500.f);
        this->map.push_back(t);
    }
    for (int i = 1; i < 9; i++)
    {
        Tile &t = this->map[i];
        t.neighbors.push_back(i + 1);
        t.neighbors.push_back(i - 1);
    }
    this->map[0].neighbors.push_back(1);
    this->map[9].neighbors.push_back(8);
}
void TestState::Render(sf::RenderWindow *windowPtr)
{
    for (int i = 0; i < this->map.size(); i++)
    {
        sf::RectangleShape node(sf::Vector2f(30.f, 30.f));
        node.setOrigin(sf::Vector2f(15.f, 15.f));
        node.setPosition(this->map[i].position);
        node.setFillColor(sf::Color::Red);
        windowPtr->draw(node);
    }
    for (int i = 0; i < this->map.size(); i++)
    {
        for (int j = 0; j < this->map[i].neighbors.size(); j++)
        {
            sf::Vertex line[] = {
                sf::Vertex{this->map[i].position, sf::Color::White},
                sf::Vertex{this->map[this->map[i].neighbors[j]].position, sf::Color::White}};
            windowPtr->draw(line, 2, sf::PrimitiveType::Lines);
        }
    }
}