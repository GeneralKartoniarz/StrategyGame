#include "InputController.hpp"
#include <algorithm>
#include <iostream>

InputController::InputController(sf::RenderWindow *window, std::vector<Tile> &mapRef, GameManager &gmRef, GameInterface *guiPtr)
    : windowPtr(window), map(mapRef), gm(gmRef), gui(guiPtr), isDragging(false), selectedTileID(-1), selectedUnitID(-1)
{
    this->camera = windowPtr->getDefaultView();
    this->camera.zoom(0.3f);
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
    if (const auto *mouseEvent = event.getIf<sf::Event::MouseButtonPressed>())
    {
        sf::Vector2i mousePos = sf::Mouse::getPosition(*this->windowPtr);
        sf::Vector2f worldPos = this->windowPtr->mapPixelToCoords(mousePos, this->camera);

        if (mouseEvent->button == sf::Mouse::Button::Left)
        {
            if (this->gui && this->gui->IsMouseOverUI(mousePos))
                return;

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
}