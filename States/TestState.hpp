#pragma once
#include "States.hpp"
#include <vector>
#include <memory>
#include "Map/Tile.hpp"
#include "UI/InputController.hpp"
#include "UI/GameInterface.hpp"
#include <SFML/Graphics.hpp>

class TestState : public States
{
public:
    TestState(sf::RenderWindow *windowPtr);
    ~TestState() override = default;

    void Update(float dt) override;
    void Render(sf::RenderWindow *windowPtr) override;
    void HandleEvent(const sf::Event &event) override;

private:
    sf::VertexArray terrainMesh;
    sf::VertexArray borderMesh;
    std::vector<Tile> map;
    std::unique_ptr<InputController> inputCtrl;
    std::unique_ptr<GameInterface> gui;
};