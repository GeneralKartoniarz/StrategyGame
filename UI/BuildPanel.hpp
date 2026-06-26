#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include "Button.hpp"
#include "Label.hpp"
#include "GameInterface.hpp"
struct City;
class GameManager;

class BuildPanel
{
public:
    BuildPanel(const sf::Vector2f &position, const sf::Vector2f &size, const sf::Font &font);
    ~BuildPanel() = default;

    void Update(const sf::Vector2i &mousePos, bool mouseClicked, const City *activeCity);
    void Draw(sf::RenderWindow *window);
    bool Contains(const sf::Vector2f &point) const;

    void SetVisible(bool visible) { this->isVisible = visible; }
    bool GetVisible() const { return this->isVisible; }

    static void OnBuildFarmClick();
    static void OnRecruitSettlerClick();
    static const City *currentCityContext;
    static const std::vector<Tile> *mapContext;

private:
    sf::RectangleShape background;
    sf::Font panelFont;
    bool isVisible = false;

    std::unique_ptr<Label> sectionTitle;
    std::unique_ptr<Label> costTrackerLabel;

    std::vector<std::unique_ptr<Button>> buildButtons;

    static GameManager *gameManagerContext;
    friend class GameInterface;
    friend class GameManager;
    friend class InputController;
};