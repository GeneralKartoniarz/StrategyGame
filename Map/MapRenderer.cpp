#include "MapRenderer.hpp"
#include <algorithm>

MapRenderer::MapRenderer()
{
    this->terrainMesh.setPrimitiveType(sf::PrimitiveType::Triangles);
    this->borderMesh.setPrimitiveType(sf::PrimitiveType::Lines);
    this->riverMesh.setPrimitiveType(sf::PrimitiveType::Lines);
    this->politicalMesh.setPrimitiveType(sf::PrimitiveType::Triangles);
}

sf::Color MapRenderer::GetBiomeColor(BiomeType biome) const
{
    switch (biome)
    {
        case BiomeType::Ocean:        return sf::Color(28, 81, 141);
        case BiomeType::IceSheet:     return sf::Color(230, 245, 250);
        case BiomeType::Tundra:       return sf::Color(145, 165, 140);
        case BiomeType::Desert:       return sf::Color(225, 190, 110);
        case BiomeType::Plains:       return sf::Color(120, 175, 90);
        case BiomeType::Forest:       return sf::Color(55, 120, 65);
        case BiomeType::Taiga:        return sf::Color(40, 95, 70);
        case BiomeType::Rainforest:   return sf::Color(45, 80, 15);
        case BiomeType::MountainPeak: return sf::Color(110, 115, 120);
        default:                      return sf::Color(100, 100, 100);
    }
}

void MapRenderer::BuildMeshes(const std::vector<Tile>& map, const TopologyGraph& topoGraph, const GameManager& gm)
{
    this->terrainMesh.clear();
    this->borderMesh.clear();
    this->riverMesh.clear();

    for (size_t i = 0; i < topoGraph.nodes.size(); ++i)
    {
        const auto& node = topoGraph.nodes[i];
        if (!node.isRiver) continue;

        for (int nIdx : node.neighbors)
        {
            const auto& neighbor = topoGraph.nodes[nIdx];
            if (neighbor.isRiver && nIdx > static_cast<int>(i))
            {
                sf::Color riverColor(30, 144, 255);
                this->riverMesh.append(sf::Vertex{node.position, riverColor});
                this->riverMesh.append(sf::Vertex{neighbor.position, riverColor});
            }
        }
    }

    for (const auto& region : map)
    {
        for (const auto& borderVertex : region.provinceBorders)
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

        for (const auto& poly : region.subPolygons)
        {
            size_t pointCount = poly.size();
            if (pointCount < 3) continue;

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
}
void MapRenderer::RebuildPoliticalMesh(const std::vector<Tile>& map, const GameManager& gm)
{
    this->politicalMesh.clear();

    for (const auto& city : gm.GetAllCities())
    {
        const Empire& owner = gm.GetEmpire(city.ownerEmpireID);
        sf::Color baseColor = owner.GetColor(); // Pobieramy czysty, bazowy kolor

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

            const auto& region = map[tileID];
            for (const auto& poly : region.subPolygons)
            {
                size_t pointCount = poly.size();
                if (pointCount < 3) continue;

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

void MapRenderer::DrawTerrain(sf::RenderWindow* window) const { window->draw(this->terrainMesh); }
void MapRenderer::DrawBordersAndRivers(sf::RenderWindow* window) const { window->draw(this->borderMesh); window->draw(this->riverMesh); }
void MapRenderer::DrawPolitical(sf::RenderWindow* window) const { window->draw(this->politicalMesh); }