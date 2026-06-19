#include "States/TestState.hpp"
#include "Map/MapGenerator.hpp"
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <SFML/Graphics.hpp>
#include <iostream>
TestState::TestState(sf::RenderWindow *windowPtr) : States(windowPtr), gm(windowPtr)
{
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    MapGenerator mg;
    TopologyGraph graph;

    bool isValidMap = false;
    while (!isValidMap)
    {
        this->map.clear();
        this->map = mg.GetMap(1920, 1080, 5, 1);

        graph = mg.ExtractTopology(this->map, 1920, 1080, ClimateEngine(std::random_device{}()));

        if (!graph.nodes.empty())
        {
            isValidMap = true;
        }
    }

    mg.GenerateRivers(graph, 150);

    NavigationGraph roadGraph = mg.BuildNavigationGraph(this->map);
    this->gm.SetNavGraph(roadGraph);

    PoliticalSetup::CreateTestEmpire(this->gm, this->map);

    this->mapRenderer = std::make_unique<MapRenderer>();
    this->mapRenderer->BuildMeshes(this->map, graph, this->gm);

    this->gui = std::make_unique<GameInterface>(windowPtr, this->gm);
    this->inputCtrl = std::make_unique<InputController>(windowPtr, this->map, this->gm, this->gui.get());

    this->gui->onNextTurnAction = [this]()
    {
        this->gm.NextTurn(this->map);

        int32_t selectedTile = this->inputCtrl->GetSelectedTileID();
        if (selectedTile != -1 && this->gui->GetCityPanel() && this->gui->GetCityPanel()->isVisible)
        {
            for (const auto &city : this->gm.GetAllCities())
            {
                if (city.centerTileID == selectedTile)
                {
                    const Empire &owner = this->gm.GetEmpire(city.ownerEmpireID);
                    const PopManager &popMgr = const_cast<Empire &>(owner).GetPopManager();

                    this->gui->GetCityPanel()->UpdateCityData(city, popMgr);
                    break;
                }
            }
        }

        this->inputCtrl->RefreshSelectedUnitUI();
    };

    this->inputCtrl->onMapChanged = [this]()
    {
        this->mapRenderer->RebuildPoliticalMesh(this->map, this->gm);
    };
}

void TestState::Update(float dt)
{
    sf::Vector2i mousePos = sf::Mouse::getPosition(*this->windowPtr);
    bool mouseClicked = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);

    this->gui->Update(dt, mousePos, mouseClicked, this->map);

    if (!this->gui->IsMouseOverUI(mousePos) && this->gui->currentInterfaceState != InterfaceState::PlacingBuilding)
    {
        this->inputCtrl->Update(dt);
    }

    this->gm.UpdateUnits(dt);
}

void TestState::HandleEvent(const sf::Event &event)
{
    if (this->gui->currentInterfaceState == InterfaceState::PlacingBuilding)
    {
        if (const auto *mouseBtnDown = event.getIf<sf::Event::MouseButtonPressed>())
        {
            if (mouseBtnDown->button == sf::Mouse::Button::Left)
            {
                sf::Vector2i pixelPos = mouseBtnDown->position;

                bool success = this->gm.TryPlaceBuildingAt(pixelPos, this->gui->buildingUnderCursor, this->map);
                
                if (success)
                {
                    this->gui->currentInterfaceState = InterfaceState::Default;
                }
                return; 
            }
            else if (mouseBtnDown->button == sf::Mouse::Button::Right)
            {
                this->gui->currentInterfaceState = InterfaceState::Default;
                std::cout << "[INTERFEJS] Anulowano tryb budowy (PPM)." << std::endl;
                return;
            }
        }
    }

    if (const auto *mouseBtnDown = event.getIf<sf::Event::MouseButtonPressed>())
    {
        if (this->gui->IsMouseOverUI(mouseBtnDown->position))
            return;
    }

    if (const auto *mouseBtnUp = event.getIf<sf::Event::MouseButtonReleased>())
    {
        if (this->gui->IsMouseOverUI(mouseBtnUp->position))
            return;
    }

    if (const auto *mouseScroll = event.getIf<sf::Event::MouseWheelScrolled>())
    {
        if (this->gui->IsMouseOverUI(mouseScroll->position))
            return;
    }

    this->inputCtrl->HandleEvent(event);
}

void TestState::Render(sf::RenderWindow *windowPtr)
{
    windowPtr->setView(this->inputCtrl->GetCamera());

    this->mapRenderer->DrawTerrain(windowPtr);

    int selectedID = this->inputCtrl->GetSelectedTileID();
    if (selectedID != -1 && static_cast<size_t>(selectedID) < this->map.size())
    {
        const auto &selectedRegion = this->map[selectedID];
        sf::VertexArray highlightMesh(sf::PrimitiveType::Triangles);
        sf::Color highlightColor(255, 255, 255, 80);

        for (const auto &poly : selectedRegion.subPolygons)
        {
            size_t pointCount = poly.size();
            if (pointCount < 3)
                continue;

            sf::Vector2f p0 = poly[0];
            for (size_t i = 1; i < pointCount - 1; ++i)
            {
                highlightMesh.append(sf::Vertex{p0, highlightColor});
                highlightMesh.append(sf::Vertex{poly[i], highlightColor});
                highlightMesh.append(sf::Vertex{poly[i + 1], highlightColor});
            }
        }
        windowPtr->draw(highlightMesh);
    }

    for (const auto &unit : this->gm.GetAllUnits())
    {
        sf::CircleShape unitShape(2.0f);
        unitShape.setOrigin({2.0f, 2.0f});
        unitShape.setPosition(unit.position);

        if (unit.type == UnitType::Settler)
        {
            unitShape.setFillColor(sf::Color::Cyan);
            unitShape.setOutlineThickness(.5f);
            unitShape.setOutlineColor(sf::Color::Black);
        }
        windowPtr->draw(unitShape);
    }

    this->mapRenderer->DrawBordersAndRivers(windowPtr);
    this->mapRenderer->DrawResources(windowPtr);
    this->mapRenderer->DrawPolitical(windowPtr);

    int32_t activeUnitID = this->inputCtrl->GetSelectedUnitID();
    if (activeUnitID != -1)
    {
        const Unit &selectedUnit = this->gm.GetUnit(activeUnitID);

        if (!selectedUnit.movementPath.empty())
        {
            sf::VertexArray pathLine(sf::PrimitiveType::LineStrip);
            sf::Color pathColor(255, 255, 0, 200);

            for (int32_t nodeID : selectedUnit.movementPath)
            {
                sf::Vector2f nodePos = this->gm.GetNavGraph().nodes[nodeID].position;
                pathLine.append(sf::Vertex{nodePos, pathColor});
            }

            windowPtr->draw(pathLine);

            sf::Vector2f targetPos = this->gm.GetNavGraph().nodes[selectedUnit.movementPath.back()].position;
            sf::CircleShape targetMarker(2.0f);
            targetMarker.setOrigin({2.0f, 2.0f});
            targetMarker.setPosition(targetPos);
            targetMarker.setFillColor(sf::Color::Yellow);
            windowPtr->draw(targetMarker);
        }
    }

    windowPtr->setView(windowPtr->getDefaultView());
    this->inputCtrl->DrawCityPlanningHighlights(this->windowPtr);
    this->gui->Draw(windowPtr);
}