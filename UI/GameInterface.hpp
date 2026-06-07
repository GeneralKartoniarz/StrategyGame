#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include "Label.hpp"
#include "Button.hpp"

class GameInterface
{
public:
    GameInterface(sf::RenderWindow* window);
    ~GameInterface();

    void Update(float dt, const sf::Vector2i& mousePos, bool mouseClicked);
    void Draw(sf::RenderWindow* window);

    int GetTurnCount() const { return this->turnCount; }

private:
    void NextTurn();

    static void OnNextTurnClick();
    static GameInterface* instance;

    sf::Font font;
    std::vector<std::unique_ptr<Label>> statLabels;
    std::unique_ptr<Label> turnCounterLabel;
    std::unique_ptr<Button> nextTurnButton;

    int turnCount;
};