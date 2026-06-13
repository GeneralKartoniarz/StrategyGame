#include "UnitPanel.hpp"

UnitPanel::UnitPanel(const sf::Vector2f &position, const sf::Vector2f &size, const sf::Font &font)
{
    this->background.setPosition(position);
    this->background.setSize(size);
    this->background.setFillColor(sf::Color(25, 25, 25, 230));
    this->background.setOutlineThickness(2.0f);
    this->background.setOutlineColor(sf::Color(110, 160, 180));

    sf::Vector2f labelSize(size.x - 30.0f, 25.0f);

    this->typeLabel = std::make_unique<Label>(sf::Vector2f(position.x + 15.0f, position.y + 15.0f), labelSize, font, "Brak aktywnej jednostki", 14);
    this->movementLabel = std::make_unique<Label>(sf::Vector2f(position.x + 15.0f, position.y + 45.0f), labelSize, font, "Ruch: --", 14);
    this->actionLabel = std::make_unique<Label>(sf::Vector2f(position.x + 15.0f, position.y + 75.0f), labelSize, font, "", 14);

    this->UpdateSelection(nullptr);
}

void UnitPanel::UpdateSelection(const Unit *unit)
{

    if (!unit)
    {
        this->isVisible = false;
        return;
    }

    this->isVisible = true;

    if (unit)
    {
        std::string typeStr = (unit->type == UnitType::Settler) ? "Jednostka: Osadnik" : "Jednostka: Wojownik";
        this->typeLabel->SetText(typeStr);

        std::string mpStr = "Ruch: " + std::to_string(unit->currentMovementPoints) + " / " + std::to_string(unit->maxMovementPoints);
        this->movementLabel->SetText(mpStr);

        std::string actionStr = (unit->type == UnitType::Settler) ? "Akcja: [B] Zaloz Miasto" : "Akcja: Brak";
        this->actionLabel->SetText(actionStr);
    }
    else
    {
        this->typeLabel->SetText("Brak aktywnej jednostki");
        this->movementLabel->SetText("Ruch: --");
        this->actionLabel->SetText("");
    }
}

void UnitPanel::Draw(sf::RenderWindow *window)
{
    if (!this->isVisible) return;

    window->draw(this->background);
    if (this->typeLabel)
        this->typeLabel->Draw(window);
    if (this->movementLabel)
        this->movementLabel->Draw(window);
    if (this->actionLabel)
        this->actionLabel->Draw(window);
}

bool UnitPanel::Contains(const sf::Vector2f &point) const
{
    if (!this->isVisible) return false;

    return this->background.getGlobalBounds().contains(point);
}