#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include "Label.hpp"
#include "Button.hpp"
#include "SidePanel.hpp"
#include "UnitPanel.hpp"
#include "CityPanel.hpp"
#include <functional>
#include "Industry.hpp"
#include "AnalyticsPanel.hpp"
class GameManager;
struct Tile;
class BuildPanel;
enum class InterfaceState
{
    Default,
    PlacingBuilding
};

class GameInterface
{
public:
    GameInterface(sf::RenderWindow *window, GameManager &gm);
    ~GameInterface();

    void Update(float dt, const sf::Vector2i &mousePos, bool mouseClicked, std::vector<Tile> &map);
    void Draw(sf::RenderWindow *window);
    bool IsMouseOverUI(const sf::Vector2i &mousePos) const;
    int GetTurnCount() const { return this->turnCount; }
    void UpdateSelection(const Tile *tile);
    void UpdateUnitSelection(const Unit *unit);
    std::function<void()> onNextTurnAction;
    void UpdateCitySelection(const City *city, const std::string &empireName);
    CityPanel *GetCityPanel() const { return this->cityPanel.get(); }

    static GameInterface *GetInstance() { return instance; }

    InterfaceState currentInterfaceState = InterfaceState::Default;
    BuildingType buildingUnderCursor = BuildingType::Farm;

private:
    void NextTurn();

    static void OnNextTurnClick();
    static GameInterface *instance;
    GameManager &gm;
    sf::Font font;
    std::vector<std::unique_ptr<Label>> statLabels;
    std::unique_ptr<Label> turnCounterLabel;
    std::unique_ptr<Button> nextTurnButton;
    std::unique_ptr<SidePanel> sidePanel;
    std::unique_ptr<UnitPanel> unitPanel;
    std::unique_ptr<CityPanel> cityPanel;
    std::unique_ptr<AnalyticsPanel> analyticsPanel;
    int turnCount;
    std::unique_ptr<BuildPanel> buildPanel;
    const City *selectedCityPtr = nullptr;
    bool isUnitSelected = false;
    void UpdateCityPanelPosition();
};