#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "Empires/City.hpp"

class AnalyticsPanel
{
public:
    AnalyticsPanel(sf::Vector2f position, sf::Vector2f size, const sf::Font &font);
    ~AnalyticsPanel() = default;

    void Update(const City *activeCity, const sf::Vector2i &mousePos, bool leftMouseDown, bool leftMouseReleased);
    void Draw(sf::RenderWindow *window);
    void SetPosition(sf::Vector2f position);
    bool Contains(const sf::Vector2f &point) const;
    bool isVisible = true;

private:
    void RenderLine(sf::RenderWindow *window, const std::vector<float> &data, sf::Color color, float maxVal);

    sf::RectangleShape background;
    sf::View panelView;
    const City *cityContext = nullptr;
    sf::Font font;
    sf::RectangleShape titleBar;
    sf::RectangleShape closeButton;

    bool isDragging = false;
    sf::Vector2f dragOffset;
    sf::Text titlePopulation;
    sf::Text titleFood;
};