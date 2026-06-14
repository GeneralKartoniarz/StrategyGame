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
    sf::VertexArray borderMesh;
    sf::VertexArray riverMesh;
    sf::VertexArray politicalMesh;
    sf::Texture resourceAtlas;
    sf::VertexArray resourceMesh;
    sf::IntRect GetResourceTextureRect(const std::string &resourceName) const;
    sf::Color GetBiomeColor(BiomeType biome) const;
};