#pragma once
#include <SFML/Graphics.hpp>
#include <memory>

class States {
protected:
    sf::RenderWindow* windowPtr;
    bool quit;

public:
    std::unique_ptr<States> nextState;

    States(sf::RenderWindow* windowPtr); // Tutaj ucinamy sprawę średnikiem
    virtual ~States() = default;

    virtual void Update(float dt) = 0;
    virtual void Render(sf::RenderWindow* windowPtr) = 0;

    bool GetQuit() const { return quit; }
    void EndState() { quit = true; }
};