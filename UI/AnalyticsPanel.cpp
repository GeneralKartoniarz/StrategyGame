#include "AnalyticsPanel.hpp"
#include <algorithm>
#include <limits>

AnalyticsPanel::AnalyticsPanel(sf::Vector2f position, sf::Vector2f size, const sf::Font& font)
    : font(font),titleFood(font), titlePopulation(font)
{
    this->background.setPosition(position);
    this->background.setSize(size);
    this->background.setFillColor(sf::Color(20, 20, 25, 220));
    this->background.setOutlineThickness(1.5f);
    this->background.setOutlineColor(sf::Color(100, 110, 120));

    this->titlePopulation.setFont(font);
    this->titlePopulation.setString("Populacja (Zolty) / Satysfakcja (Zielony)");
    this->titlePopulation.setCharacterSize(11);
    this->titlePopulation.setFillColor(sf::Color(200, 200, 200));
    this->titlePopulation.setPosition(position + sf::Vector2f(10.0f, 10.0f));

    this->titleFood.setFont(font);
    this->titleFood.setString("Zapasy zywnosci (Niebieski)");
    this->titleFood.setCharacterSize(11);
    this->titleFood.setFillColor(sf::Color(200, 200, 200));
    this->titleFood.setPosition(position + sf::Vector2f(10.0f, 25.0f));
}

void AnalyticsPanel::Update(const City* activeCity)
{
    this->cityContext = activeCity;
}

void AnalyticsPanel::RenderLine(sf::RenderWindow* window, const std::vector<float>& data, sf::Color color, float maxVal)
{
    if (data.size() < 2) return;

    sf::VertexArray line(sf::PrimitiveType::LineStrip);
    sf::Vector2f bgPos = this->background.getPosition();
    sf::Vector2f bgSize = this->background.getSize();

    float chartMarginX = 15.0f;
    float chartMarginY = 45.0f;
    float chartWidth = bgSize.x - (chartMarginX * 2.0f);
    float chartHeight = bgSize.y - chartMarginY - 15.0f;

    float stepX = chartWidth / static_cast<float>(City::MAX_HISTORY_SAMPLES - 1);
    
    if (maxVal <= 0.0f) maxVal = 1.0f;

    for (size_t i = 0; i < data.size(); ++i)
    {
        float x = bgPos.x + chartMarginX + (static_cast<float>(i) * stepX);
        float y = bgPos.y + bgSize.y - 15.0f - ((data[i] / maxVal) * chartHeight);
        
        line.append(sf::Vertex({sf::Vector2f(x, y), color}));
    }

    window->draw(line);
}

void AnalyticsPanel::Draw(sf::RenderWindow* window)
{
    if (!this->isVisible || !this->cityContext || this->cityContext->economyHistory.empty()) return;

    sf::View oldView = window->getView();
    window->setView(window->getDefaultView());

    window->draw(this->background);
    window->draw(this->titlePopulation);
    window->draw(this->titleFood);

    std::vector<float> popData;
    std::vector<float> satData;
    std::vector<float> foodData;

    float maxPop = 10.0f;
    float maxFood = 100.0f;

    for (const auto& sample : this->cityContext->economyHistory)
    {
        popData.push_back(sample.population);
        satData.push_back(sample.averageSatisfaction);
        foodData.push_back(sample.foodSupply);

        if (sample.population > maxPop) maxPop = sample.population;
        if (sample.foodSupply > maxFood) maxFood = sample.foodSupply;
    }

    this->RenderLine(window, popData, sf::Color(240, 200, 50), maxPop);        
    this->RenderLine(window, satData, sf::Color(50, 220, 100), 255.0f);       
    this->RenderLine(window, foodData, sf::Color(50, 150, 250), maxFood);    

    window->setView(oldView);
}