#pragma once
#include <vector>
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics.hpp>
using namespace std;
enum class TerrainType
{
    Plains,
    Water,
    Mountain,
    Desert
};
struct CellEdge
{
    sf::Vector2f p1;
    sf::Vector2f p2;
    int neighborID;
};
struct Tile
{
    sf::Vector2f position;
    std::vector<sf::Vector2f> vertices;
    std::vector<sf::ConvexShape> subShapes;
    sf::ConvexShape shape;
    std::size_t ID;
    float size;
    int ownerID;
    std::vector<CellEdge> cellEdges;
    std::vector<sf::Vertex> provinceBorders;
    TerrainType type;
    std::vector<std::size_t> neighbors;
};