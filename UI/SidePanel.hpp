#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include "Tile.hpp"

class SidePanel
{
public:
    SidePanel(const sf::Vector2f& position, const sf::Vector2f& size, const sf::Font& font);
    ~SidePanel() = default;

    void UpdateSelection(const Tile* tile);
    void Draw(sf::RenderWindow* window);
    bool Contains(const sf::Vector2f& point) const;

private:
    sf::RectangleShape background;
    sf::Text nameText;
    sf::Text idText;
    sf::Text biomeText;

    std::string BiomeToString(BiomeType biome);
};