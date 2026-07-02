#include "MarketPanel.hpp"
#include <iomanip>
#include <sstream>

MarketPanel::MarketPanel(sf::Vector2f position, sf::Vector2f size, const sf::Font &font)
    : font(font)
{
    this->background.setSize(size);
    this->background.setFillColor(sf::Color(25, 25, 30, 245));
    this->background.setOutlineThickness(1.5f);
    this->background.setOutlineColor(sf::Color(150, 130, 80));

    this->titleBar.setFillColor(sf::Color(40, 40, 45, 240));
    this->titleBar.setSize(sf::Vector2f(size.x, 25.0f));

    this->closeButton.setFillColor(sf::Color(180, 50, 50, 240));
    this->closeButton.setSize(sf::Vector2f(20.0f, 20.0f));

    this->scrollbarThumb.setFillColor(sf::Color(80, 80, 85, 200));
    this->scrollbarThumb.setSize(sf::Vector2f(10.0f, 40.0f));

    this->allResources = {
        ResourceType::Grain, ResourceType::Fish, ResourceType::Meat, ResourceType::Wood,
        ResourceType::Coal, ResourceType::IronOre, ResourceType::Iron, ResourceType::Gold,
        ResourceType::Fur, ResourceType::Cocoa, ResourceType::Silk, ResourceType::Paper,
        ResourceType::LuxuryAlcohol, ResourceType::Steel, ResourceType::Horses,
        ResourceType::UraniumOre, ResourceType::Whales, ResourceType::Pearls};

    this->SetPosition(position);
}

void MarketPanel::SetPosition(sf::Vector2f newPos)
{
    this->background.setPosition(newPos);
    this->titleBar.setPosition(newPos);
    this->closeButton.setPosition({newPos.x + this->background.getSize().x - 22.5f, newPos.y + 2.5f});
}

bool MarketPanel::Contains(const sf::Vector2f &point) const
{
    return this->isVisible && this->background.getGlobalBounds().contains(point);
}

// UI/MarketPanel.cpp
void MarketPanel::Update(const sf::Vector2i &mousePos, bool leftMouseDown, bool leftMouseReleased, float scrollDelta, const std::map<ResourceType, MarketCommodity> &market, const std::map<ResourceType, float> &warehouse)
{
    if (!this->isVisible)
        return;
    this->currentMarket = market;
    this->currentWarehouse = warehouse;

    sf::Vector2f fMousePos(mousePos.x, mousePos.y);

    if (leftMouseReleased && this->closeButton.getGlobalBounds().contains(fMousePos))
    {
        this->isVisible = false;
        this->isDragging = false;
        this->isDraggingScrollbar = false;
        return;
    }

    if (leftMouseDown && !this->isDragging && !this->isDraggingScrollbar && this->titleBar.getGlobalBounds().contains(fMousePos))
    {
        this->isDragging = true;
        this->dragOffset = this->background.getPosition() - fMousePos;
    }

    if (this->isDragging)
    {
        if (!sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
            this->isDragging = false;
        else
            this->SetPosition(fMousePos + this->dragOffset);
    }

    float rowHeight = 35.0f;
    float contentHeight = this->allResources.size() * rowHeight;
    float viewHeight = this->background.getSize().y - this->titleBar.getSize().y - 10.0f;
    this->maxScroll = std::max(0.0f, contentHeight - viewHeight);

    if (leftMouseReleased)
    {
        this->isDraggingScrollbar = false;
    }

    if (leftMouseDown && !this->isDragging && !this->isDraggingScrollbar && this->scrollbarThumb.getGlobalBounds().contains(fMousePos))
    {
        this->isDraggingScrollbar = true;
        this->scrollDragOffsetY = fMousePos.y - this->scrollbarThumb.getPosition().y;
    }

    if (this->isDraggingScrollbar)
    {
        if (!sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
        {
            this->isDraggingScrollbar = false;
        }
        else
        {
            float trackHeight = viewHeight - this->scrollbarThumb.getSize().y;
            float trackStartY = this->background.getPosition().y + this->titleBar.getSize().y + 5.0f;

            float newThumbY = fMousePos.y - this->scrollDragOffsetY;
            float scrollPercent = (newThumbY - trackStartY) / trackHeight;

            scrollPercent = std::clamp(scrollPercent, 0.0f, 1.0f);
            this->scrollOffset = scrollPercent * this->maxScroll;
        }
    }

    if (scrollDelta != 0.0f && this->Contains(fMousePos))
    {
        this->scrollOffset -= scrollDelta * 40.0f;
    }

    if (this->scrollOffset < 0.0f)
        this->scrollOffset = 0.0f;
    if (this->scrollOffset > this->maxScroll)
        this->scrollOffset = this->maxScroll;

    if (this->maxScroll > 0.0f)
    {
        float scrollPercent = this->scrollOffset / this->maxScroll;
        float trackHeight = viewHeight - this->scrollbarThumb.getSize().y;
        float thumbY = this->background.getPosition().y + this->titleBar.getSize().y + 5.0f + (scrollPercent * trackHeight);

        this->scrollbarThumb.setPosition({this->background.getPosition().x + this->background.getSize().x - 15.0f, thumbY});
    }
}

void MarketPanel::Draw(sf::RenderWindow *window)
{
    if (!this->isVisible)
        return;

    sf::View oldView = window->getView();
    window->setView(window->getDefaultView());

    window->draw(this->background);
    window->draw(this->titleBar);
    window->draw(this->closeButton);
    if (this->maxScroll > 0.0f)
        window->draw(this->scrollbarThumb);

    sf::Vector2f bgPos = this->background.getPosition();
    sf::Vector2f bgSize = this->background.getSize();

    float titleHeight = this->titleBar.getSize().y;
    float contentY = bgPos.y + titleHeight;
    float contentW = bgSize.x - 20.0f;
    float contentH = bgSize.y - titleHeight;

    sf::Vector2u winSize = window->getSize();
    sf::FloatRect viewport({bgPos.x / winSize.x,
                            contentY / winSize.y},
                           {contentW / winSize.x,
                            contentH / winSize.y});

    sf::View scrollView;
    scrollView.setViewport(viewport);
    scrollView.setSize({contentW, contentH});
    scrollView.setCenter({bgPos.x + (contentW / 2.0f), contentY + (contentH / 2.0f) + this->scrollOffset});

    window->setView(scrollView);

    float rowY = contentY + 5.0f;
    float col1 = bgPos.x + 5.0f; // Trend
    float col2 = col1 + 55.0f;   // Nazwa
    float col3 = col2 + 130.0f;  // Popyt
    float col4 = col3 + 70.0f;   // Podaż (Produkcja)
    float col5 = col4 + 70.0f;   // Cena + Magazyn

    for (ResourceType res : this->allResources)
    {
        // Pobieramy dane
        float currentPrice = this->currentMarket.count(res) ? this->currentMarket[res].currentPrice : 1.0f;
        float prevPrice = this->currentMarket.count(res) ? this->currentMarket[res].previousPrice : 1.0f;
        float demand = this->currentMarket.count(res) ? this->currentMarket[res].uiDemandDisplay : 0.0f;
        float production = this->currentMarket.count(res) ? this->currentMarket[res].uiProductionDisplay : 0.0f;
        float inWarehouse = this->currentWarehouse.count(res) ? this->currentWarehouse[res] : 0.0f;

        sf::RectangleShape rowBg(sf::Vector2f(contentW, 30.0f));
        rowBg.setPosition({bgPos.x, rowY});
        rowBg.setFillColor(sf::Color(35, 35, 40, (int)rowY % 2 == 0 ? 150 : 50));
        window->draw(rowBg);

        float trendPercent = prevPrice > 0.0f ? ((currentPrice - prevPrice) / prevPrice) * 100.0f : 0.0f;
        sf::Text tTrend(this->font, "", 14);
        if (trendPercent > 0.1f)
        {
            tTrend.setString("+" + std::to_string((int)trendPercent) + "%");
            tTrend.setFillColor(sf::Color(50, 220, 100));
        }
        else if (trendPercent < -0.1f)
        {
            tTrend.setString(std::to_string((int)trendPercent) + "%");
            tTrend.setFillColor(sf::Color(220, 50, 50));
        }
        else
        {
            tTrend.setString(" 0%");
            tTrend.setFillColor(sf::Color(150, 150, 150));
        }
        tTrend.setPosition({col1, rowY + 5.0f});

        sf::Text tName(this->font, MarketRegistry::GetResourceName(res), 14);
        tName.setFillColor(sf::Color::White);
        tName.setPosition({col2, rowY + 5.0f});

        sf::Text tDemand(this->font, std::to_string((int)demand), 14);
        tDemand.setFillColor(sf::Color(200, 150, 50));
        tDemand.setPosition({col3, rowY + 5.0f});

        sf::Text tSupply(this->font, std::to_string((int)production), 14);
        tSupply.setFillColor(sf::Color(50, 150, 200));
        tSupply.setPosition({col4, rowY + 5.0f});

        std::stringstream ss;
        ss << std::fixed << std::setprecision(1) << currentPrice << "$ | " << (int)inWarehouse << " szt.";
        sf::Text tPrice(this->font, ss.str(), 14);
        tPrice.setFillColor(sf::Color(230, 230, 230));
        tPrice.setPosition({col5, rowY + 5.0f});

        window->draw(tTrend);
        window->draw(tName);
        window->draw(tDemand);
        window->draw(tSupply);
        window->draw(tPrice);

        rowY += 35.0f;
    }

    window->setView(oldView);
}