#pragma once
#include "States.hpp"
#include <vector>
#include "Tile.hpp"
#include <SFML/Graphics.hpp>

class TestState : public States {
public:
    TestState(sf::RenderWindow* windowPtr);
    ~TestState() override = default;

    sf::VertexArray terrainMesh;
    sf::VertexArray borderMesh;
    std::vector<Tile> map;
    
    sf::View camera;
    bool isDragging = false;
    sf::Vector2i lastMousePos;
    
    void Update(float dt) override;
    void Render(sf::RenderWindow* windowPtr) override;
    void HandleEvent(const sf::Event& event) override;
};