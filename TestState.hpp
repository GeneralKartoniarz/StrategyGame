#pragma once
#include "States.hpp"
#include "vector"
#include "Tile.hpp"
using namespace std;
class TestState : public States {
public:
    TestState(sf::RenderWindow* windowPtr);
    ~TestState() override = default;
    vector<Tile> map;
    void CreateTestGraph();
    void Update(float dt) override;
    void Render(sf::RenderWindow* windowPtr) override;
};