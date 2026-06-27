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
    void UpdateCityData(const City &city, const PopManager &popMgr);
    bool isVisible;
    void Update(const sf::Vector2i &mousePos, bool leftMouseDown, bool leftMouseReleased);

private:
    sf::RectangleShape background;
    std::unique_ptr<Label> cityNameLabel;
    std::unique_ptr<Label> ownerLabel;
    std::unique_ptr<Label> tilesCountLabel;
    std::unique_ptr<Label> treasuryLabel;
    std::unique_ptr<Label> unemploymentLabel;
    std::unique_ptr<Label> totalPopLabel;
    std::unique_ptr<Label> satisfactionLabel;
    std::unique_ptr<Label> ClassDistributionLabel;
    sf::RectangleShape titleBar;
    sf::RectangleShape closeButton;

    bool isDragging = false;
    sf::Vector2f dragOffset;
};