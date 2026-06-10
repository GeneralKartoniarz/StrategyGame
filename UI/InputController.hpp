#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "Map/Tile.hpp"
#include "GameManager.hpp"
#include "GameInterface.hpp"
#include "Pathfinder.hpp"
class InputController
{
public:
    InputController(sf::RenderWindow *window, std::vector<Tile> &mapRef, GameManager &gmRef, GameInterface *guiPtr);
    ~InputController() = default;

    void Update(float dt);
    void HandleEvent(const sf::Event &event);

    const sf::View &GetCamera() const { return this->camera; }
    int GetSelectedTileID() const { return this->selectedTileID; }

private:
    bool IsPointInTriangle(const sf::Vector2f &p, const sf::Vector2f &a, const sf::Vector2f &b, const sf::Vector2f &c);
    void ClampCamera();

    sf::RenderWindow *windowPtr;
    std::vector<Tile> &map;

    sf::View camera;
    bool isDragging;
    sf::Vector2i lastMousePos;
    int selectedTileID;

    GameManager &gm;
    GameInterface *gui;
    int32_t selectedUnitID;
};