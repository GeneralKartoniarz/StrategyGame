#pragma once
#include "States.hpp"
#include <vector>
#include "Tile.hpp"

class TestState : public States {
public:
    TestState(sf::RenderWindow* windowPtr);
    ~TestState() override = default;
    
    std::vector<Tile> map;
    
    void Update(float dt) override;
    void Render(sf::RenderWindow* windowPtr) override;
};