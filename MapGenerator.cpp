#define JC_VORONOI_IMPLEMENTATION
#include "jc_voronoi.h"
#include "MapGenerator.hpp"
#include <random>
#include "Tile.hpp"
#include <SFML/Graphics.hpp>
#include <queue>
#include <algorithm>

sf::Vector2f MapGenerator::CalculateCentroid(const jcv_site *site)
{
    float sumX = 0, sumY = 0;
    int count = 0;
    const jcv_graphedge *edge = site->edges;
    while (edge)
    {
        sumX += edge->pos[0].x + edge->pos[1].x;
        sumY += edge->pos[0].y + edge->pos[1].y;
        count += 2;
        edge = edge->next;
    }
    return sf::Vector2f(sumX / count, sumY / count);
}

std::vector<jcv_point> MapGenerator::InitializeSeeds(int mapWidth, int mapHeight, int cellSize)
{
    std::vector<jcv_point> points;
    int margin = 50;
    int cols = (mapWidth - 2 * margin) / cellSize;
    int rows = (mapHeight - 2 * margin) / cellSize;
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

std::vector<Tile> MapGenerator::CreateTiles(const std::vector<jcv_point> &points, int mapWidth, int mapHeight)
{
    jcv_diagram diagram;
    jcv_rect rect = {{50.0f, 50.0f}, {(float)mapWidth - 50.0f, (float)mapHeight - 50.0f}};
    memset(&diagram, 0, sizeof(jcv_diagram));
    jcv_diagram_generate(points.size(), points.data(), &rect, 0, &diagram);

    std::vector<Tile> tiles(diagram.numsites);
    const jcv_site *sites = jcv_diagram_get_sites(&diagram);

    for (int i = 0; i < diagram.numsites; ++i)
    {
        const jcv_site *site = &sites[i];
        Tile t;
        t.ID = site->index;
        t.position = {site->p.x, site->p.y};

        const jcv_graphedge *edge = site->edges;
        while (edge)
        {
            t.vertices.push_back({edge->pos[0].x, edge->pos[0].y});

            int nID = edge->neighbor ? edge->neighbor->index : -1;
            if (nID != -1)
            {
                t.neighbors.push_back(nID);
            }

            t.cellEdges.push_back({sf::Vector2f(edge->pos[0].x, edge->pos[0].y),
                                   sf::Vector2f(edge->pos[1].x, edge->pos[1].y),
                                   nID});

            edge = edge->next;
        }

        t.shape.setPointCount(t.vertices.size());
        for (size_t j = 0; j < t.vertices.size(); j++)
            t.shape.setPoint(j, t.vertices[j]);
        t.shape.setFillColor(sf::Color(200, 0, 40));
        t.shape.setOutlineThickness(0);

        tiles[site->index] = t;
    }
    jcv_diagram_free(&diagram);
    return tiles;
}

std::vector<Tile> MapGenerator::MergeTiles(const std::vector<Tile> &smallTiles, int targetClusterSize)
{
    std::vector<bool> isAssigned(smallTiles.size(), false);
    std::vector<int> smallToBig(smallTiles.size(), -1);
    std::vector<std::vector<int>> clusters;

    for (size_t i = 0; i < smallTiles.size(); ++i)
    {
        if (isAssigned[i])
            continue;

        std::vector<int> currentCluster;
        std::queue<int> q;
        std::vector<bool> inQueue(smallTiles.size(), false);

        q.push(i);
        inQueue[i] = true;

        while (!q.empty() && currentCluster.size() < static_cast<size_t>(targetClusterSize))
        {
            int curr = q.front();
            q.pop();

            if (isAssigned[curr])
                continue;

            isAssigned[curr] = true;
            currentCluster.push_back(curr);
            smallToBig[curr] = clusters.size();

            for (int neighborID : smallTiles[curr].neighbors)
            {
                if (!isAssigned[neighborID] && !inQueue[neighborID])
                {
                    q.push(neighborID);
                    inQueue[neighborID] = true;
                }
            }
        }
        clusters.push_back(currentCluster);
    }

    std::vector<Tile> largeTiles(clusters.size());

    for (size_t b = 0; b < clusters.size(); ++b)
    {
        Tile lt;
        lt.ID = b;

        sf::Vector2f centerSum(0.0f, 0.0f);
        for (int smallID : clusters[b])
        {
            const Tile &st = smallTiles[smallID];
            centerSum += st.position;

            sf::ConvexShape copyShape = st.shape;
            lt.subShapes.push_back(copyShape);

            for (const auto &cEdge : st.cellEdges)
            {
                int bigNeighborID = (cEdge.neighborID == -1) ? -1 : smallToBig[cEdge.neighborID];

                if (bigNeighborID != static_cast<int>(b))
                {
                    sf::Color borderColor(15, 15, 15);
                    lt.provinceBorders.push_back(sf::Vertex{cEdge.p1, borderColor});
                    lt.provinceBorders.push_back(sf::Vertex{cEdge.p2, borderColor});
                }
            }
        }
        lt.position = centerSum / static_cast<float>(clusters[b].size());

        for (int smallID : clusters[b])
        {
            for (int smallNeighborID : smallTiles[smallID].neighbors)
            {
                int bigNeighborID = smallToBig[smallNeighborID];
                if (bigNeighborID != -1 && bigNeighborID != static_cast<int>(b))
                {
                    if (std::find(lt.neighbors.begin(), lt.neighbors.end(), bigNeighborID) == lt.neighbors.end())
                    {
                        lt.neighbors.push_back(bigNeighborID);
                    }
                }
            }
        }
        largeTiles[b] = lt;
    }

    return largeTiles;
}

std::vector<Tile> MapGenerator::GetMap(int mapWidth, int mapHeight, int cellSize, int iterations)
{
    std::vector<jcv_point> points = InitializeSeeds(mapWidth, mapHeight, cellSize);
    jcv_rect rect = {{50.0f, 50.0f}, {(float)mapWidth - 50.0f, (float)mapHeight - 50.0f}};

    for (int it = 0; it < iterations; ++it)
    {
        jcv_diagram diagram;
        memset(&diagram, 0, sizeof(jcv_diagram));
        jcv_diagram_generate(points.size(), points.data(), &rect, 0, &diagram);

        const jcv_site *sites = jcv_diagram_get_sites(&diagram);
        for (int i = 0; i < diagram.numsites; ++i)
        {
            sf::Vector2f centroid = CalculateCentroid(&sites[i]);
            points[i].x = centroid.x;
            points[i].y = centroid.y;
        }
        jcv_diagram_free(&diagram);
    }

    std::vector<Tile> fineSiatka = CreateTiles(points, mapWidth, mapHeight);
    return MergeTiles(fineSiatka, 6);
}