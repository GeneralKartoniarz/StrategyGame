#pragma once
#include <SFML/Graphics.hpp>
#include "Empires/City.hpp"
#include "Label.hpp"
#include <memory>
#include "../GameManager.hpp"
class CityPanel
{
public:
    CityPanel(sf::Vector2f position, sf::Vector2f size, const sf::Font &font);

    void UpdateSelection(const City *city, const std::string &empireName, const GameManager &gm);
    void Draw(sf::RenderWindow *window);
    bool Contains(sf::Vector2f point) const;
    void SetPosition(sf::Vector2f position);

private:
    sf::RectangleShape background;
    std::unique_ptr<Label> cityNameLabel;
    std::unique_ptr<Label> ownerLabel;
    std::unique_ptr<Label> tilesCountLabel;
    bool isVisible;
};