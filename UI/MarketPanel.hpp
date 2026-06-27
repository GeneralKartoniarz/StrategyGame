#pragma once
#include <SFML/Graphics.hpp>
#include <map>
#include <vector>
#include <string>
#include "../Economy.hpp"
#include "../Empires/City.hpp"

class MarketPanel
{
public:
    MarketPanel(sf::Vector2f position, sf::Vector2f size, const sf::Font& font);
    ~MarketPanel() = default;

    void Update(const sf::Vector2i& mousePos, bool leftMouseDown, bool leftMouseReleased, float scrollDelta, const std::map<ResourceType, MarketCommodity>& market, const std::map<ResourceType, float>& warehouse);
    void Draw(sf::RenderWindow* window);
    void SetPosition(sf::Vector2f newPos);
    bool Contains(const sf::Vector2f& point) const;

    bool isVisible = false;

private:
    sf::RectangleShape background;
    sf::RectangleShape titleBar;
    sf::RectangleShape closeButton;
    sf::RectangleShape scrollbarThumb;
    
    sf::Font font;
    
    bool isDragging = false;
    sf::Vector2f dragOffset;
    
    float scrollOffset = 0.0f;
    float maxScroll = 0.0f;
    bool isDraggingScrollbar = false; 
    float scrollDragOffsetY = 0.0f;
    std::map<ResourceType, MarketCommodity> currentMarket;
    std::map<ResourceType, float> currentWarehouse;
    std::vector<ResourceType> allResources;
};