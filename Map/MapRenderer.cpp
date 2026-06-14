#include "MapRenderer.hpp"
#include <algorithm>
#include <iostream>
MapRenderer::MapRenderer()
{
    this->terrainMesh.setPrimitiveType(sf::PrimitiveType::Triangles);
    this->borderMesh.setPrimitiveType(sf::PrimitiveType::Lines);
    this->riverMesh.setPrimitiveType(sf::PrimitiveType::Lines);
    this->politicalMesh.setPrimitiveType(sf::PrimitiveType::Triangles);
    this->resourceMesh.setPrimitiveType(sf::PrimitiveType::Triangles);
    if (!this->resourceAtlas.loadFromFile("resurces/textures/resource_atlas.png"))
    {
        std::cout << "[BŁĄD] Nie udalo sie znalezc pliku resources/textures/resource_atlas.png!" << std::endl;
    }
}

sf::Color MapRenderer::GetBiomeColor(BiomeType biome) const
{
    switch (biome)
    {
    case BiomeType::Ocean:
        return sf::Color(28, 81, 141);
    case BiomeType::IceSheet:
        return sf::Color(230, 245, 250);
    case BiomeType::Tundra:
        return sf::Color(145, 165, 140);
    case BiomeType::Desert:
        return sf::Color(225, 190, 110);
    case BiomeType::Plains:
        return sf::Color(120, 175, 90);
    case BiomeType::Forest:
        return sf::Color(55, 120, 65);
    case BiomeType::Taiga:
        return sf::Color(40, 95, 70);
    case BiomeType::Rainforest:
        return sf::Color(45, 80, 15);
    case BiomeType::MountainPeak:
        return sf::Color(110, 115, 120);
    default:
        return sf::Color(100, 100, 100);
    }
}
sf::IntRect MapRenderer::GetResourceTextureRect(const std::string &resourceName) const
{
    if (resourceName == "Brak")
        return sf::IntRect(sf::Vector2i(0, 0), sf::Vector2i(0, 0));
    if (resourceName == "Ruda Żelaza")
        return sf::IntRect(sf::Vector2i(0, 0), sf::Vector2i(32, 32));
    if (resourceName == "Złoto")
        return sf::IntRect(sf::Vector2i(32, 0), sf::Vector2i(32, 32));
    if (resourceName == "Żyzna Gleba")
        return sf::IntRect(sf::Vector2i(64, 0), sf::Vector2i(32, 32));
    if (resourceName == "Wieloryby")
        return sf::IntRect(sf::Vector2i(96, 0), sf::Vector2i(32, 32));
    if (resourceName == "Perły")
        return sf::IntRect(sf::Vector2i(128, 0), sf::Vector2i(32, 32));
    if (resourceName == "Ławica Ryb")
        return sf::IntRect(sf::Vector2i(160, 0), sf::Vector2i(32, 32));
    if (resourceName == "Zwierzęta Futerkowe")
        return sf::IntRect(sf::Vector2i(192, 0), sf::Vector2i(32, 32));
    if (resourceName == "Kakao")
        return sf::IntRect(sf::Vector2i(224, 0), sf::Vector2i(32, 32));
    if (resourceName == "Ropa")
        return sf::IntRect(sf::Vector2i(256, 0), sf::Vector2i(32, 32));
    if (resourceName == "Węgiel")
        return sf::IntRect(sf::Vector2i(288, 0), sf::Vector2i(32, 32));
    if (resourceName == "Konie")
        return sf::IntRect(sf::Vector2i(320, 0), sf::Vector2i(32, 32));
    if (resourceName == "Uran")
        return sf::IntRect(sf::Vector2i(352, 0), sf::Vector2i(32, 32));

    return sf::IntRect(sf::Vector2i(0, 0), sf::Vector2i(0, 0));
}
void MapRenderer::BuildMeshes(const std::vector<Tile> &map, const TopologyGraph &topoGraph, const GameManager &gm)
{
    this->terrainMesh.clear();
    this->borderMesh.clear();
    this->riverMesh.clear();

    for (size_t i = 0; i < topoGraph.nodes.size(); ++i)
    {
        const auto &node = topoGraph.nodes[i];
        if (!node.isRiver)
            continue;

        for (int nIdx : node.neighbors)
        {
            const auto &neighbor = topoGraph.nodes[nIdx];
            if (neighbor.isRiver && nIdx > static_cast<int>(i))
            {
                sf::Color riverColor(30, 144, 255);
                this->riverMesh.append(sf::Vertex{node.position, riverColor});
                this->riverMesh.append(sf::Vertex{neighbor.position, riverColor});
            }
        }
    }

    for (const auto &region : map)
    {
        for (const auto &borderVertex : region.provinceBorders)
        {
            this->borderMesh.append(borderVertex);
        }

        sf::Color baseColor = GetBiomeColor(region.terrain.biome);
        float heightFactor = region.terrain.elevationNoise;
        int brightnessAdjustment = static_cast<int>(heightFactor * 30.0f);
        sf::Color finalColor;

        if (region.terrain.biome == BiomeType::Ocean)
        {
            finalColor.r = std::max(0, std::min(255, baseColor.r + brightnessAdjustment / 2));
            finalColor.g = std::max(0, std::min(255, baseColor.g + brightnessAdjustment / 2));
            finalColor.b = std::max(0, std::min(255, baseColor.b + brightnessAdjustment));
        }
        else
        {
            finalColor.r = std::max(0, std::min(255, baseColor.r + brightnessAdjustment));
            finalColor.g = std::max(0, std::min(255, baseColor.g + brightnessAdjustment));
            finalColor.b = std::max(0, std::min(255, baseColor.b + brightnessAdjustment));
        }

        for (const auto &poly : region.subPolygons)
        {
            size_t pointCount = poly.size();
            if (pointCount < 3)
                continue;

            sf::Vector2f p0 = poly[0];
            for (size_t i = 1; i < pointCount - 1; ++i)
            {
                this->terrainMesh.append(sf::Vertex{p0, finalColor});
                this->terrainMesh.append(sf::Vertex{poly[i], finalColor});
                this->terrainMesh.append(sf::Vertex{poly[i + 1], finalColor});
            }
        }
    }

    this->RebuildPoliticalMesh(map, gm);
    this->resourceMesh.clear();

    for (const auto &region : map)
    {
        if (region.terrain.resourceName == "Brak")
            continue;

        sf::IntRect uv = this->GetResourceTextureRect(region.terrain.resourceName);
        if (uv.size.x == 0 || uv.size.y == 0)
            continue;

        sf::Vector2f center = region.position;

        float halfSize = 1.0f;

        sf::Vector2f tl(center.x - halfSize, center.y - halfSize);
        sf::Vector2f tr(center.x + halfSize, center.y - halfSize);
        sf::Vector2f br(center.x + halfSize, center.y + halfSize);
        sf::Vector2f bl(center.x - halfSize, center.y + halfSize);

        sf::Vector2f uvTl(static_cast<float>(uv.position.x), static_cast<float>(uv.position.y));
        sf::Vector2f uvTr(static_cast<float>(uv.position.x + uv.size.x), static_cast<float>(uv.position.y));
        sf::Vector2f uvBr(static_cast<float>(uv.position.x + uv.size.x), static_cast<float>(uv.position.y + uv.size.y));
        sf::Vector2f uvBl(static_cast<float>(uv.position.x), static_cast<float>(uv.position.y + uv.size.y));

        this->resourceMesh.append(sf::Vertex{tl, sf::Color::White, uvTl});
        this->resourceMesh.append(sf::Vertex{tr, sf::Color::White, uvTr});
        this->resourceMesh.append(sf::Vertex{br, sf::Color::White, uvBr});

        this->resourceMesh.append(sf::Vertex{tl, sf::Color::White, uvTl});
        this->resourceMesh.append(sf::Vertex{br, sf::Color::White, uvBr});
        this->resourceMesh.append(sf::Vertex{bl, sf::Color::White, uvBl});
    }
}
void MapRenderer::RebuildPoliticalMesh(const std::vector<Tile> &map, const GameManager &gm)
{
    this->politicalMesh.clear();

    for (const auto &city : gm.GetAllCities())
    {
        const Empire &owner = gm.GetEmpire(city.ownerEmpireID);
        sf::Color baseColor = owner.GetColor();

        for (int32_t tileID : city.jurisdictionTiles)
        {
            sf::Color tileColor = baseColor;

            if (tileID == city.centerTileID)
            {
                tileColor.a = 180;
            }
            else
            {
                tileColor.a = 60;
            }

            const auto &region = map[tileID];
            for (const auto &poly : region.subPolygons)
            {
                size_t pointCount = poly.size();
                if (pointCount < 3)
                    continue;

                sf::Vector2f p0 = poly[0];
                for (size_t i = 1; i < pointCount - 1; ++i)
                {
                    this->politicalMesh.append(sf::Vertex{p0, tileColor});
                    this->politicalMesh.append(sf::Vertex{poly[i], tileColor});
                    this->politicalMesh.append(sf::Vertex{poly[i + 1], tileColor});
                }
            }
        }
    }
}

void MapRenderer::DrawTerrain(sf::RenderWindow *window) const { window->draw(this->terrainMesh); }
void MapRenderer::DrawBordersAndRivers(sf::RenderWindow *window) const
{
    window->draw(this->borderMesh);
    window->draw(this->riverMesh);
}
void MapRenderer::DrawPolitical(sf::RenderWindow *window) const { window->draw(this->politicalMesh); }
void MapRenderer::DrawResources(sf::RenderWindow *window) const
{
    window->draw(this->resourceMesh, &this->resourceAtlas);
}