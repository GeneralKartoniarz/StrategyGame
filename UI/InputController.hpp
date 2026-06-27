#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "Map/Tile.hpp"
#include "GameManager.hpp"
#include "GameInterface.hpp"
#include "Pathfinder.hpp"
#include "MarketPanel.hpp"
#include <functional>
class InputController
{
public:
    InputController(sf::RenderWindow *window, std::vector<Tile> &mapRef, GameManager &gmRef, GameInterface *guiPtr);
    ~InputController() = default;

    void Update(float dt);
    void HandleEvent(const sf::Event &event);

    const sf::View &GetCamera() const { return this->camera; }
    int GetSelectedTileID() const { return this->selectedTileID; }
    int32_t GetSelectedUnitID() const { return this->selectedUnitID; }
    std::function<void()> onMapChanged;
    void RefreshSelectedUnitUI();

    void DrawCityPlanningHighlights(sf::RenderWindow *window);

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

    bool isPlanningCity = false;
    int32_t settlerUnitIDForCity = -1;
    std::vector<int32_t> validCityTiles;

    bool isTypingCityName = false;
    int32_t pendingCityTileID = -1;
    std::string typedCityName = "";
    sf::Font font;
};