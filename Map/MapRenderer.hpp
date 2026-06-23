#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "Tile.hpp"
#include "Map/MapGenerator.hpp"
#include "GameManager.hpp"

class MapRenderer
{
public:
    MapRenderer();
    void BuildMeshes(const std::vector<Tile> &map, const TopologyGraph &topoGraph, const GameManager &gm);

    void DrawTerrain(sf::RenderWindow *window) const;
    void DrawBordersAndRivers(sf::RenderWindow *window) const;
    void DrawPolitical(sf::RenderWindow *window) const;
    void RebuildPoliticalMesh(const std::vector<Tile> &map, const GameManager &gm);
    void DrawResources(sf::RenderWindow *window) const;

private:
    sf::VertexArray terrainMesh;

    sf::VertexArray biomeMesh;
    sf::VertexArray desertTerrainMesh;
    sf::VertexArray plainsTerrainMesh;
    sf::VertexArray taigaTerrainMesh;
    sf::VertexArray tundraTerrainMesh;
    sf::VertexArray oceanTerrainMesh;
    sf::VertexArray mountainTerrainMesh;
    sf::VertexArray iceTerrainMesh;
    sf::VertexArray rainForestTerrainMesh;
    sf::VertexArray forestTerrainMesh;

    sf::Texture desertTexture;
    sf::Texture plainsTexture;
    sf::Texture taigaTexture;
    sf::Texture tundraTexture;
    sf::Texture oceanTexture;
    sf::Texture mountainTexture;
    sf::Texture iceTexture;
    sf::Texture rainForestTexture;
    sf::Texture forestTexture;

    sf::VertexArray borderMesh;
    sf::VertexArray riverMesh;
    sf::VertexArray politicalMesh;
    sf::Texture resourceAtlas;
    sf::VertexArray resourceMesh;
    sf::IntRect GetResourceTextureRect(const std::string &resourceName) const;
};