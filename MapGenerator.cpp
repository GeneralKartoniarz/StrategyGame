#define JC_VORONOI_IMPLEMENTATION
#include "jc_voronoi.h"
#include "MapGenerator.hpp"
#include <random>
#include "Tile.hpp"
std::vector<Tile> MapGenerator::GenerateSeeds(int mapWidth, int mapHeight, int cellSize)
{
    std::vector<sf::Vector2f> seeds;

    std::random_device rd;
    std::mt19937 gen(rd());

    int cols = mapWidth / cellSize;
    int rows = mapHeight / cellSize;

    for (int y = 0; y < rows; ++y)
    {
        for (int x = 0; x < cols; ++x)
        {
            int minX = x * cellSize;
            int maxX = (x + 1) * cellSize;
            int minY = y * cellSize;
            int maxY = (y + 1) * cellSize;

            std::uniform_real_distribution<float> distX(minX, maxX);
            std::uniform_real_distribution<float> distY(minY, maxY);

            sf::Vector2f pos = {distX(gen), distY(gen)};
            seeds.push_back(pos);
        }
    }
    std::vector<jcv_point> jcvPoints(seeds.size());

    for (size_t i = 0; i < seeds.size(); ++i)
    {
        jcvPoints[i].x = seeds[i].x;
        jcvPoints[i].y = seeds[i].y;
    }
    jcv_diagram diagram;
    jcv_rect rect;
    rect.min.x = 0.0f;
    rect.min.y = 0.0f;
    rect.max.x = mapWidth;
    rect.max.y = mapHeight;
    memset(&diagram, 0, sizeof(jcv_diagram));
    jcv_diagram_generate(jcvPoints.size(), jcvPoints.data(), &rect, 0, &diagram);
    std::vector<Tile> generatedMap;
    const jcv_site *sites = jcv_diagram_get_sites(&diagram);

    for (int i = 0; i < diagram.numsites; ++i)
    {
        const jcv_site *site = &sites[i];

        Tile t;
        t.ID = site->index;
        t.position = sf::Vector2f(site->p.x, site->p.y);

        const jcv_graphedge *edge = site->edges;
        while (edge)
        {
            t.vertices.push_back(sf::Vector2f({edge->pos[0].x, edge->pos[0].y}));
            edge = edge->next;
        }

        generatedMap.push_back(t);
    }

    jcv_diagram_free(&diagram);
    return generatedMap;
}