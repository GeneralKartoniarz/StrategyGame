#include "MapRenderer.hpp"
#include <algorithm>
#include <iostream>

MapRenderer::MapRenderer()
{
    this->terrainMesh.setPrimitiveType(sf::PrimitiveType::Triangles);
    this->borderMesh.setPrimitiveType(sf::PrimitiveType::Lines);
    this->riverMesh.setPrimitiveType(sf::PrimitiveType::Lines);

    this->desertTerrainMesh.setPrimitiveType(sf::PrimitiveType::Triangles);
    this->plainsTerrainMesh.setPrimitiveType(sf::PrimitiveType::Triangles);
    this->oceanTerrainMesh.setPrimitiveType(sf::PrimitiveType::Triangles);
    this->forestTerrainMesh.setPrimitiveType(sf::PrimitiveType::Triangles);
    this->rainForestTerrainMesh.setPrimitiveType(sf::PrimitiveType::Triangles);
    this->tundraTerrainMesh.setPrimitiveType(sf::PrimitiveType::Triangles);
    this->iceTerrainMesh.setPrimitiveType(sf::PrimitiveType::Triangles);
    this->taigaTerrainMesh.setPrimitiveType(sf::PrimitiveType::Triangles);
    this->mountainTerrainMesh.setPrimitiveType(sf::PrimitiveType::Triangles);

    this->politicalMesh.setPrimitiveType(sf::PrimitiveType::Triangles);
    this->resourceMesh.setPrimitiveType(sf::PrimitiveType::Triangles);

    if (!this->resourceAtlas.loadFromFile("resources/textures/resource_atlas.png"))
    {
        std::cout << "[BŁĄD] Nie udalo sie znalezc pliku resources/textures/resource_atlas.png!" << std::endl;
    }

    auto loadTex = [](sf::Texture &tex, const std::string &path)
    {
        if (!tex.loadFromFile(path))
        {
            std::cout << "[BŁĄD] Brak pliku: " << path << std::endl;
        }
        tex.setRepeated(true);
    };

    loadTex(this->desertTexture, "resources/textures/DesertBackground.png");
    loadTex(this->plainsTexture, "resources/textures/PlainsBackground.png");
    loadTex(this->oceanTexture, "resources/textures/OceanBackground.png");
    loadTex(this->forestTexture, "resources/textures/ForestBackground.png");
    loadTex(this->rainForestTexture, "resources/textures/RainForestBackground.png");
    loadTex(this->tundraTexture, "resources/textures/TundraBackground.png");
    loadTex(this->iceTexture, "resources/textures/IceBackground.png");
    loadTex(this->taigaTexture, "resources/textures/TaigaBackground.png");
    loadTex(this->mountainTexture, "resources/textures/MountainBackground.png");
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
/*
 * [PL] METODA: BuildMeshes
 * LOGIKA: Wypieka surowe obiekty sf::VertexArray (Trójkąty dla terenów, Linie dla granic) 
 * na podstawie twardych danych z mapy. Dzieli teren na osobne meshe na podstawie biomów dla wydajności.
 * POWIĄZANIA: Ściśle zależne od sprzętowego API SFML (sf::VertexArray, sf::Texture).
 * * [EN] METHOD: BuildMeshes
 * LOGIC: Bakes raw sf::VertexArray objects (Triangles for terrain, Lines for borders) 
 * based on hard map data. Splits terrain into separate meshes by biome for rendering performance.
 * DEPENDENCIES: Strictly reliant on the hardware SFML API (sf::VertexArray, sf::Texture).
 */
void MapRenderer::BuildMeshes(const std::vector<Tile> &map, const TopologyGraph &topoGraph, const GameManager &gm)
{
    this->terrainMesh.clear();
    this->borderMesh.clear();
    this->riverMesh.clear();
    this->desertTerrainMesh.clear();
    this->plainsTerrainMesh.clear();
    this->oceanTerrainMesh.clear();
    this->forestTerrainMesh.clear();
    this->rainForestTerrainMesh.clear();
    this->tundraTerrainMesh.clear();
    this->iceTerrainMesh.clear();
    this->taigaTerrainMesh.clear();
    this->mountainTerrainMesh.clear();

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
            sf::Vertex v = borderVertex;
            v.color = sf::Color(30, 30, 30, 220); 
            this->borderMesh.append(v);
        }

        sf::VertexArray* targetMesh = &this->terrainMesh;
        switch (region.terrain.biome)
        {
            case BiomeType::Desert:       targetMesh = &this->desertTerrainMesh; break;
            case BiomeType::Plains:       targetMesh = &this->plainsTerrainMesh; break;
            case BiomeType::Ocean:        targetMesh = &this->oceanTerrainMesh; break;
            case BiomeType::Forest:       targetMesh = &this->forestTerrainMesh; break;
            case BiomeType::Rainforest:   targetMesh = &this->rainForestTerrainMesh; break;
            case BiomeType::Tundra:       targetMesh = &this->tundraTerrainMesh; break;
            case BiomeType::IceSheet:     targetMesh = &this->iceTerrainMesh; break;
            case BiomeType::Taiga:        targetMesh = &this->taigaTerrainMesh; break;
            case BiomeType::MountainPeak: targetMesh = &this->mountainTerrainMesh; break;
            default:                      targetMesh = &this->terrainMesh; break;
        }

        float heightFactor = region.terrain.elevationNoise;
        int brightnessAdjustment = static_cast<int>(heightFactor * 30.0f);
        
        int tintVal = std::max(0, std::min(255, 255 + brightnessAdjustment));
        sf::Color finalColor(tintVal, tintVal, tintVal, 255);

        if (region.terrain.biome == BiomeType::Ocean)
        {
            finalColor.r = std::max(0, std::min(255, tintVal / 2));
            finalColor.g = std::max(0, std::min(255, tintVal / 2));
            finalColor.b = std::max(0, std::min(255, tintVal));
        }

        for (const auto &poly : region.subPolygons)
        {
            size_t pointCount = poly.size();
            if (pointCount < 3)
                continue;

            sf::Vector2f p0 = poly[0];
            for (size_t i = 1; i < pointCount - 1; ++i)
            {
                targetMesh->append(sf::Vertex{p0, finalColor, p0});
                targetMesh->append(sf::Vertex{poly[i], finalColor, poly[i]});
                targetMesh->append(sf::Vertex{poly[i + 1], finalColor, poly[i + 1]});
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
/*
 * [PL] METODA: RebuildPoliticalMesh
 * LOGIKA: Przebudowuje i nakłada półprzezroczystą warstwę, oznaczającą jurysdykcję
 * i własność polityczną poszczególnych terytoriów państw. Centrum miasta jest mocniej cieniowane.
 * POWIĄZANIA: GameManager.hpp (zaciąga dane o granicach imperiów).
 * * [EN] METHOD: RebuildPoliticalMesh
 * LOGIC: Rebuilds and applies a semi-transparent layer indicating political jurisdiction 
 * and territory ownership. The city center tile receives heavier shading.
 * DEPENDENCIES: GameManager.hpp (pulls data on empire borders).
 */
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

void MapRenderer::DrawTerrain(sf::RenderWindow *window) const 
{ 
    window->draw(this->terrainMesh); 
    window->draw(this->desertTerrainMesh, &this->desertTexture);
    window->draw(this->plainsTerrainMesh, &this->plainsTexture);
    window->draw(this->oceanTerrainMesh, &this->oceanTexture);
    window->draw(this->forestTerrainMesh, &this->forestTexture);
    window->draw(this->rainForestTerrainMesh, &this->rainForestTexture);
    window->draw(this->tundraTerrainMesh, &this->tundraTexture);
    window->draw(this->iceTerrainMesh, &this->iceTexture);
    window->draw(this->taigaTerrainMesh, &this->taigaTexture);
    window->draw(this->mountainTerrainMesh, &this->mountainTexture);
}
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