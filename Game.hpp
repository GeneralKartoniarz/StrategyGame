
#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include "States/States.hpp"

class Game {
private:
    sf::RenderWindow* windowPtr;
    std::vector<std::unique_ptr<States>> states;
    sf::Clock dtClock;
    float dt;

    void InitWindow();
    void InitStates();
    void UpdateEvent();

public:
    Game();
    ~Game();

    void Run();
    void Update(float dt);
    void Render();
};