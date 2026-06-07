#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include "Label.hpp"
#include "Button.hpp"
#include "SidePanel.hpp"
class GameInterface
{
public:
    GameInterface(sf::RenderWindow *window);
    ~GameInterface();

    void Update(float dt, const sf::Vector2i &mousePos, bool mouseClicked);
    void Draw(sf::RenderWindow *window);
    bool IsMouseOverUI(const sf::Vector2i &mousePos) const;
    int GetTurnCount() const { return this->turnCount; }
    void UpdateSelection(const Tile *tile);

private:
    void NextTurn();

    static void OnNextTurnClick();
    static GameInterface *instance;

    sf::Font font;
    std::vector<std::unique_ptr<Label>> statLabels;
    std::unique_ptr<Label> turnCounterLabel;
    std::unique_ptr<Button> nextTurnButton;
    std::unique_ptr<SidePanel> sidePanel;
    int turnCount;
};