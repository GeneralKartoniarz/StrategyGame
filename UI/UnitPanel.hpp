#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include "Unit.hpp"
#include "Label.hpp"

class UnitPanel 
{
public:
    UnitPanel(const sf::Vector2f& position, const sf::Vector2f& size, const sf::Font& font);
    void UpdateSelection(const Unit* unit);
    void Draw(sf::RenderWindow* window);
    bool Contains(const sf::Vector2f& point) const;

private:
    sf::RectangleShape background;
    std::unique_ptr<Label> typeLabel;
    std::unique_ptr<Label> movementLabel;
    std::unique_ptr<Label> actionLabel;
    bool isVisible = false;
};