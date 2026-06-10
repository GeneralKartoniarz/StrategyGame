#define JC_VORONOI_IMPLEMENTATION
#include "jc_voronoi.h"
#include "VoronoiBuilder.hpp"
#include <random>
#include <cstring>

std::vector<sf::Vector2f> VoronoiBuilder::InitializeSeeds(int mapWidth, int mapHeight, int cellSize)
{
    std::vector<sf::Vector2f> points;
    int margin = 50;
    int cols = (mapWidth - 2 * margin) / cellSize;
    int rows = (mapHeight - 2 * margin) / cellSize;
    points.reserve(cols * rows);
    
    std::random_device rd;
    std::mt19937 gen(rd());

    for (int y = 0; y < rows; ++y)
    {
        for (int x = 0; x < cols; ++x)
        {
            std::uniform_real_distribution<float> distX(margin + x * cellSize, margin + (x + 1) * cellSize);
            std::uniform_real_distribution<float> distY(margin + y * cellSize, margin + (y + 1) * cellSize);
            points.push_back({distX(gen), distY(gen)});
        }
    }
    return points;
}

void VoronoiBuilder::ApplyLloydRelaxation(std::vector<sf::Vector2f>& points, int mapWidth, int mapHeight, int iterations)
{
    jcv_rect rect = {{50.0, 50.0}, {static_cast<double>(mapWidth) - 50.0, static_cast<double>(mapHeight) - 50.0}};
    std::vector<jcv_point> jcvPoints(points.size());

    for (int it = 0; it < iterations; ++it)
    {
        for (size_t i = 0; i < points.size(); ++i) 
        {
            jcvPoints[i].x = points[i].x;
            jcvPoints[i].y = points[i].y;
        }

        jcv_diagram diagram;
        memset(&diagram, 0, sizeof(jcv_diagram));
        jcv_diagram_generate(jcvPoints.size(), jcvPoints.data(), &rect, 0, &diagram);

        const jcv_site* sites = jcv_diagram_get_sites(&diagram);
        for (int i = 0; i < diagram.numsites; ++i)
        {
            double sumX = 0.0, sumY = 0.0;
            int count = 0;
            const jcv_graphedge* edge = sites[i].edges;

            while (edge)
            {
                sumX += edge->pos[0].x + edge->pos[1].x;
                sumY += edge->pos[0].y + edge->pos[1].y;
                count += 2;
                edge = edge->next;
            }

            if (count > 0)
            {
                points[sites[i].index].x = static_cast<float>(sumX / count);
                points[sites[i].index].y = static_cast<float>(sumY / count);
            }
        }
        jcv_diagram_free(&diagram);
    }
}

std::vector<Tile> VoronoiBuilder::CreateTiles(const std::vector<sf::Vector2f>& points, int mapWidth, int mapHeight)
{
    jcv_rect rect = {{50.0, 50.0}, {static_cast<double>(mapWidth) - 50.0, static_cast<double>(mapHeight) - 50.0}};
    std::vector<jcv_point> jcvPoints(points.size());
    for (size_t i = 0; i < points.size(); ++i) 
    {
        jcvPoints[i].x = points[i].x;
        jcvPoints[i].y = points[i].y;
    }

    jcv_diagram diagram;
    memset(&diagram, 0, sizeof(jcv_diagram));
    jcv_diagram_generate(jcvPoints.size(), jcvPoints.data(), &rect, 0, &diagram);

    std::vector<Tile> tiles(points.size());
    const jcv_site* sites = jcv_diagram_get_sites(&diagram);

    for (int i = 0; i < diagram.numsites; ++i)
    {
        const jcv_site* site = &sites[i];
        Tile t;
        t.ID = site->index;
        t.position = {static_cast<float>(site->p.x), static_cast<float>(site->p.y)};

        const jcv_graphedge* edge = site->edges;
        while (edge)
        {
            t.vertices.push_back({static_cast<float>(edge->pos[0].x), static_cast<float>(edge->pos[0].y)});

            int nID = edge->neighbor ? edge->neighbor->index : -1;
            if (nID != -1) t.neighbors.push_back(nID);

            t.cellEdges.push_back({
                sf::Vector2f(static_cast<float>(edge->pos[0].x), static_cast<float>(edge->pos[0].y)),
                sf::Vector2f(static_cast<float>(edge->pos[1].x), static_cast<float>(edge->pos[1].y)),
                nID
            });
            edge = edge->next;
        }
        tiles[site->index] = t;
    }
    
    jcv_diagram_free(&diagram);
    return tiles;
}

std::vector<Tile> VoronoiBuilder::GenerateRawGrid(int mapWidth, int mapHeight, int cellSize, int iterations)
{
    std::vector<sf::Vector2f> points = InitializeSeeds(mapWidth, mapHeight, cellSize);
    ApplyLloydRelaxation(points, mapWidth, mapHeight, iterations);
    return CreateTiles(points, mapWidth, mapHeight);
}