#include "InputController.hpp"
#include <algorithm>
#include <iostream>

InputController::InputController(sf::RenderWindow *window, std::vector<Tile> &mapRef, GameManager &gmRef, GameInterface *guiPtr)
    : windowPtr(window), map(mapRef), gm(gmRef), gui(guiPtr), isDragging(false), selectedTileID(-1), selectedUnitID(-1), isPlanningCity(false), settlerUnitIDForCity(-1), isTypingCityName(false), pendingCityTileID(-1)
{
    this->camera = windowPtr->getDefaultView();
    this->camera.zoom(0.3f);

    if (!this->font.openFromFile("resources/fonts/ARIAL.TTF"))
    {
        this->font.openFromFile("C:/Windows/Fonts/Arial.ttf");
    }
}

bool InputController::IsPointInTriangle(const sf::Vector2f &p, const sf::Vector2f &a, const sf::Vector2f &b, const sf::Vector2f &c)
{
    auto crossProduct = [](const sf::Vector2f &p1, const sf::Vector2f &p2, const sf::Vector2f &p3)
    {
        return (p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y);
    };

    float d1 = crossProduct(p, a, b);
    float d2 = crossProduct(p, b, c);
    float d3 = crossProduct(p, c, a);

    bool has_neg = (d1 < 0) || (d2 < 0) || (d3 < 0);
    bool has_pos = (d1 > 0) || (d2 > 0) || (d3 > 0);

    return !(has_neg && has_pos);
}

void InputController::ClampCamera()
{
    sf::Vector2f viewSize = this->camera.getSize();
    sf::Vector2f viewCenter = this->camera.getCenter();

    float minX = (viewSize.x / 2.0f) + 50.0f;
    float maxX = 1920.0f - (viewSize.x / 2.0f) - 50.0f;
    float minY = (viewSize.y / 2.0f) + 50.0f;
    float maxY = 1080.0f - (viewSize.y / 2.0f) - 50.0f;

    viewCenter.x = std::clamp(viewCenter.x, minX, maxX);
    viewCenter.y = std::clamp(viewCenter.y, minY, maxY);

    this->camera.setCenter(viewCenter);
}

void InputController::Update(float dt)
{
    if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Middle))
    {
        sf::Vector2i currentMousePos = sf::Mouse::getPosition(*this->windowPtr);

        if (this->isDragging)
        {
            sf::Vector2f oldWorldPos = this->windowPtr->mapPixelToCoords(this->lastMousePos, this->camera);
            sf::Vector2f newWorldPos = this->windowPtr->mapPixelToCoords(currentMousePos, this->camera);

            sf::Vector2f delta = oldWorldPos - newWorldPos;
            this->camera.move(delta);
        }

        this->isDragging = true;
        this->lastMousePos = currentMousePos;
    }
    else
    {
        this->isDragging = false;
    }

    this->ClampCamera();
}

void InputController::HandleEvent(const sf::Event &event)
{
    /*
     * BLOK WPROWADZANIA TEKSTU (NATIVE SFML)
     * Kiedy gracz wybierze kafel pod miasto, silnik wchodzi w tryb isTypingCityName.
     * Przechwytujemy tutaj zdarzenia TextEntered do budowania stringa oraz KeyPressed
     * dla akcji specjalnych (Enter zatwierdza, Escape anuluje). Na końcu znajduje się
     * return, który fizycznie blokuje graczowi możliwość klikania w mapę lub ruszania
     * jednostkami do czasu zamknięcia okna dialogowego.
     */
    if (this->isTypingCityName)
    {
        if (const auto *textEvent = event.getIf<sf::Event::TextEntered>())
        {
            if (textEvent->unicode == '\b' && !this->typedCityName.empty())
            {
                this->typedCityName.pop_back();
            }
            else if (textEvent->unicode >= 32 && textEvent->unicode < 128 && this->typedCityName.size() < 20)
            {
                this->typedCityName += static_cast<char>(textEvent->unicode);
            }
        }
        else if (const auto *keyEvent = event.getIf<sf::Event::KeyPressed>())
        {
            if (keyEvent->code == sf::Keyboard::Key::Enter)
            {
                if (this->typedCityName.empty())
                    this->typedCityName = "Nowa Osada";
                uint32_t assignedNameID = this->gm.RegisterCityName(this->typedCityName);
                this->gm.TransformSettlerToCity(this->settlerUnitIDForCity, this->pendingCityTileID, assignedNameID, this->map);

                this->isTypingCityName = false;
                this->selectedUnitID = -1;
                this->validCityTiles.clear();

                if (this->onMapChanged)
                    this->onMapChanged();
            }
            else if (keyEvent->code == sf::Keyboard::Key::Escape)
            {
                this->isTypingCityName = false;
                this->isPlanningCity = false;
                this->validCityTiles.clear();
            }
        }
        return;
    }

    if (const auto *mouseEvent = event.getIf<sf::Event::MouseButtonPressed>())
    {
        sf::Vector2i mousePos = sf::Mouse::getPosition(*this->windowPtr);
        sf::Vector2f worldPos = this->windowPtr->mapPixelToCoords(mousePos, this->camera);

        if (mouseEvent->button == sf::Mouse::Button::Left)
        {
            if (this->gui && this->gui->IsMouseOverUI(mousePos))
                return;

            if (this->isPlanningCity)
            {
                int32_t clickedTileIndex = -1;
                for (size_t i = 0; i < this->map.size(); ++i)
                {
                    const auto &region = this->map[i];
                    bool insideRegion = false;
                    for (const auto &poly : region.subPolygons)
                    {
                        size_t pointCount = poly.size();
                        if (pointCount < 3)
                            continue;
                        sf::Vector2f p0 = poly[0];
                        for (size_t j = 1; j < pointCount - 1; ++j)
                        {
                            if (IsPointInTriangle(worldPos, p0, poly[j], poly[j + 1]))
                            {
                                insideRegion = true;
                                break;
                            }
                        }
                        if (insideRegion)
                            break;
                    }
                    if (insideRegion)
                    {
                        clickedTileIndex = static_cast<int32_t>(i);
                        break;
                    }
                }

                auto it = std::find(this->validCityTiles.begin(), this->validCityTiles.end(), clickedTileIndex);
                if (it != this->validCityTiles.end())
                {
                    this->isPlanningCity = false;
                    this->isTypingCityName = true;
                    this->pendingCityTileID = clickedTileIndex;
                    this->typedCityName = "";
                    return;
                }
                else
                {
                    this->isPlanningCity = false;
                    this->validCityTiles.clear();
                }
            }

            this->selectedTileID = -1;
            for (const auto &region : this->map)
            {
                bool insideRegion = false;

                for (const auto &poly : region.subPolygons)
                {
                    size_t pointCount = poly.size();
                    if (pointCount < 3)
                        continue;

                    sf::Vector2f p0 = poly[0];
                    for (size_t i = 1; i < pointCount - 1; ++i)
                    {
                        if (IsPointInTriangle(worldPos, p0, poly[i], poly[i + 1]))
                        {
                            insideRegion = true;
                            break;
                        }
                    }
                    if (insideRegion)
                        break;
                }

                if (insideRegion)
                {
                    this->selectedTileID = static_cast<int>(region.ID);
                    break;
                }
            }

            const City *clickedCity = nullptr;
            std::string ownerName = "";

            if (this->selectedTileID != -1)
            {
                for (const auto &city : this->gm.GetAllCities())
                {
                    if (city.centerTileID == this->selectedTileID)
                    {
                        clickedCity = &city;
                        ownerName = this->gm.GetEmpire(city.ownerEmpireID).GetName();
                        break;
                    }
                }
            }
            this->gui->UpdateCitySelection(clickedCity, ownerName);

            if (this->gui)
            {
                const Tile *clickedTile = (this->selectedTileID != -1) ? &this->map[this->selectedTileID] : nullptr;
                this->gui->UpdateSelection(clickedTile);
            }

            this->selectedUnitID = -1;
            float clickToleranceSq = 9.0f * 9.0f;

            for (const auto &unit : this->gm.GetAllUnits())
            {
                float dx = worldPos.x - unit.position.x;
                float dy = worldPos.y - unit.position.y;
                if (dx * dx + dy * dy <= clickToleranceSq)
                {
                    this->selectedUnitID = unit.ID;
                    break;
                }
            }

            if (this->gui)
            {
                const Unit *clickedUnit = (this->selectedUnitID != -1) ? &this->gm.GetUnit(this->selectedUnitID) : nullptr;
                this->gui->UpdateUnitSelection(clickedUnit);
            }
        }
        else if (mouseEvent->button == sf::Mouse::Button::Right)
        {
            if (this->selectedUnitID != -1 && !this->gm.GetAllUnits().empty())
            {
                int32_t targetNodeID = this->gm.GetNearestNodeID(worldPos);
                Unit &activeUnit = this->gm.GetUnit(this->selectedUnitID);

                int32_t startNode = (activeUnit.nextNodeID != -1) ? activeUnit.nextNodeID : activeUnit.currentNodeID;
                activeUnit.movementPath = Pathfinder::FindPath(this->gm.GetNavGraph(), startNode, targetNodeID);

                if (!activeUnit.movementPath.empty() && activeUnit.movementPath.front() == startNode)
                {
                    activeUnit.movementPath.erase(activeUnit.movementPath.begin());
                }
            }
        }
    }

    if (const auto *scrollEvent = event.getIf<sf::Event::MouseWheelScrolled>())
    {
        if (scrollEvent->wheel == sf::Mouse::Wheel::Vertical)
        {
            if (scrollEvent->delta > 0.0f)
            {
                this->camera.zoom(0.9f);
                if (this->camera.getSize().x < 150.0f)
                    this->camera.zoom(1.0f / 0.9f);
            }
            else
            {
                this->camera.zoom(1.1f);
                if (this->camera.getSize().y > 980.0f)
                    this->camera.zoom(1.0f / 1.1f);
            }
        }
    }

    if (const auto *keyEvent = event.getIf<sf::Event::KeyPressed>())
    {
        if (keyEvent->code == sf::Keyboard::Key::B)
        {
            if (this->selectedUnitID != -1 && !this->isPlanningCity)
            {
                Unit &activeUnit = this->gm.GetUnit(this->selectedUnitID);

                if (activeUnit.colonization.has_value())
                {
                    int32_t currentTileID = -1;
                    for (const auto &region : this->map)
                    {
                        bool insideRegion = false;
                        for (const auto &poly : region.subPolygons)
                        {
                            size_t pointCount = poly.size();
                            if (pointCount < 3)
                                continue;

                            sf::Vector2f p0 = poly[0];
                            for (size_t i = 1; i < pointCount - 1; ++i)
                            {
                                if (IsPointInTriangle(activeUnit.position, p0, poly[i], poly[i + 1]))
                                {
                                    insideRegion = true;
                                    break;
                                }
                            }
                            if (insideRegion)
                                break;
                        }

                        if (insideRegion)
                        {
                            currentTileID = static_cast<int32_t>(region.ID);
                            break;
                        }
                    }

                    if (currentTileID != -1)
                    {
                        this->isPlanningCity = true;
                        this->settlerUnitIDForCity = this->selectedUnitID;
                        this->validCityTiles.clear();

                        float toleranceSq = 1.0f * 1.0f;

                        for (size_t i = 0; i < this->map.size(); ++i)
                        {
                            const auto &region = this->map[i];
                            bool touchesSettler = false;

                            for (const auto &poly : region.subPolygons)
                            {
                                for (const auto &point : poly)
                                {
                                    float dx = point.x - activeUnit.position.x;
                                    float dy = point.y - activeUnit.position.y;

                                    if (dx * dx + dy * dy <= toleranceSq)
                                    {
                                        touchesSettler = true;
                                        break;
                                    }
                                }
                                if (touchesSettler)
                                    break;
                            }

                            if (touchesSettler)
                            {
                                if (this->gm.CanFoundCity(static_cast<int32_t>(i), this->map))
                                {
                                    this->validCityTiles.push_back(static_cast<int32_t>(i));
                                }
                            }
                        }

                        if (this->validCityTiles.empty())
                        {
                            this->isPlanningCity = false;
                        }
                    }
                }
            }
        }
    }
}

void InputController::DrawCityPlanningHighlights(sf::RenderWindow *window)
{
    if (this->isPlanningCity && !this->validCityTiles.empty())
    {
        sf::View oldView = window->getView();
        window->setView(this->camera);

        sf::Color highlightColor(0, 255, 0, 100);

        for (int32_t tileID : this->validCityTiles)
        {
            const auto &region = this->map[tileID];
            for (const auto &poly : region.subPolygons)
            {
                size_t pointCount = poly.size();
                if (pointCount < 3)
                    continue;

                sf::VertexArray trigs(sf::PrimitiveType::Triangles);
                sf::Vector2f p0 = poly[0];

                for (size_t i = 1; i < pointCount - 1; ++i)
                {
                    trigs.append(sf::Vertex{p0, highlightColor});
                    trigs.append(sf::Vertex{poly[i], highlightColor});
                    trigs.append(sf::Vertex{poly[i + 1], highlightColor});
                }
                window->draw(trigs);
            }
        }
        window->setView(oldView);
    }

    if (this->isTypingCityName)
    {
        sf::View oldView = window->getView();
        window->setView(window->getDefaultView());

        sf::RectangleShape bg(sf::Vector2f(400.0f, 150.0f));
        bg.setPosition(sf::Vector2f(1920.0f / 2.0f - 200.0f, 1080.0f / 2.0f - 75.0f));
        bg.setFillColor(sf::Color(30, 30, 35, 240));
        bg.setOutlineThickness(2.0f);
        bg.setOutlineColor(sf::Color(100, 100, 105));

        sf::Text title(this->font, "Wpisz nazwe miasta:", 20);
        title.setPosition(sf::Vector2f(bg.getPosition().x + 20.0f, bg.getPosition().y + 20.0f));
        title.setFillColor(sf::Color::White);

        sf::RectangleShape inputField(sf::Vector2f(360.0f, 40.0f));
        inputField.setPosition(sf::Vector2f(bg.getPosition().x + 20.0f, bg.getPosition().y + 60.0f));
        inputField.setFillColor(sf::Color(15, 15, 20));
        inputField.setOutlineThickness(1.0f);
        inputField.setOutlineColor(sf::Color(80, 80, 80));

        sf::Text inputText(this->font, this->typedCityName + "_", 24);
        inputText.setPosition(sf::Vector2f(inputField.getPosition().x + 10.0f, inputField.getPosition().y + 5.0f));
        inputText.setFillColor(sf::Color::White);

        sf::Text info(this->font, "ENTER - zatwierdz | ESC - anuluj", 14);
        info.setPosition(sf::Vector2f(bg.getPosition().x + 20.0f, bg.getPosition().y + 115.0f));
        info.setFillColor(sf::Color(150, 150, 150));

        window->draw(bg);
        window->draw(title);
        window->draw(inputField);
        window->draw(inputText);
        window->draw(info);

        window->setView(oldView);
    }
}

void InputController::RefreshSelectedUnitUI()
{
    if (this->gui && this->selectedUnitID != -1)
    {
        if (this->selectedUnitID < static_cast<int32_t>(this->gm.GetAllUnits().size()))
        {
            const Unit &activeUnit = this->gm.GetUnit(this->selectedUnitID);
            this->gui->UpdateUnitSelection(&activeUnit);
        }
    }
}