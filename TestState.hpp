#pragma once
#include "States.hpp"

class TestState : public States {
public:
    TestState(sf::RenderWindow* windowPtr);
    ~TestState() override = default;

    void Update(float dt) override;
    void Render(sf::RenderWindow* windowPtr) override;
};