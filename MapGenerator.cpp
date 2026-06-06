#define JC_VORONOI_IMPLEMENTATION
#include "jc_voronoi.h"
#include "MapGenerator.hpp"
#include <random>
#include "Tile.hpp"
#include <SFML/Graphics.hpp>
#include <queue>
#include <algorithm>
#include <cmath>

Elevation GetElevation(float noise)
{
    if (noise < -0.4f)
        return Elevation::Water;
    if (noise < 0.2f)
        return Elevation::Plains;
    if (noise < 0.6f)
        return Elevation::Hills;
    return Elevation::Mountains;
}

Temperature GetTemperature(float noise)
{
    if (noise < -0.3f)
        return Temperature::Cold;
    if (noise < 0.3f)
        return Temperature::Temperate;
    return Temperature::Hot;
}

Moisture GetMoisture(float noise)
{
    if (noise < -0.2f)
        return Moisture::Dry;
    if (noise < 0.4f)
        return Moisture::Normal;
    return Moisture::Wet;
}

BiomeType DetermineBiome(Elevation elev, Temperature temp, Moisture moist)
{
    if (elev == Elevation::Water)
        return BiomeType::Ocean;
    if (elev == Elevation::Mountains)
        return BiomeType::MountainPeak;

    switch (temp)
    {
    case Temperature::Cold:
        if (moist == Moisture::Dry)
            return BiomeType::IceSheet;
        return BiomeType::Tundra;

    case Temperature::Temperate:
        if (moist == Moisture::Dry)
            return BiomeType::Plains;
        if (moist == Moisture::Normal)
            return BiomeType::Forest;
        return BiomeType::Taiga;

    case Temperature::Hot:
        if (moist == Moisture::Dry)
            return BiomeType::Desert;
        if (moist == Moisture::Normal)
            return BiomeType::Plains;
        return BiomeType::Rainforest;
    }
    return BiomeType::Plains;
}

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

        tiles[site->index] = t;
    }
    jcv_diagram_free(&diagram);
    return tiles;
}

std::vector<Tile> MapGenerator::MergeTiles(const std::vector<Tile> &smallTiles, int targetClusterSize, int mapWidth, int mapHeight)
{
    std::vector<bool> isAssigned(smallTiles.size(), false);
    std::vector<int> smallToBig(smallTiles.size(), -1);
    std::vector<std::vector<int>> clusters;

    // Jednorazowa alokacja flag kolejkowych zapobiegająca heap thrashingowi
    std::vector<int> inQueueFlags(smallTiles.size(), -1);

    for (size_t i = 0; i < smallTiles.size(); ++i)
    {
        if (isAssigned[i])
            continue;

        std::vector<int> currentCluster;
        std::queue<int> q;

        q.push(i);
        inQueueFlags[i] = i;

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
                if (!isAssigned[neighborID] && inQueueFlags[neighborID] != static_cast<int>(i))
                {
                    q.push(neighborID);
                    inQueueFlags[neighborID] = i;
                }
            }
        }

        clusters.push_back(currentCluster);
    }

    std::vector<Tile> largeTiles(clusters.size());
    sf::Vector2f mapCenter(mapWidth / 2.0f, mapHeight / 2.0f);
    float maxDist = std::min(mapWidth, mapHeight) / 2.0f;

    for (size_t b = 0; b < clusters.size(); ++b)
    {
        Tile lt;
        lt.ID = b;

        sf::Vector2f centerSum(0.0f, 0.0f);
        for (int smallID : clusters[b])
        {
            const Tile &st = smallTiles[smallID];
            centerSum += st.position;

            lt.subPolygons.push_back(st.vertices);

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
        float distToCenter = std::sqrt(std::pow(lt.position.x - mapCenter.x, 2) + std::pow(lt.position.y - mapCenter.y, 2));
        float falloff = distToCenter / maxDist;
        falloff = std::pow(falloff, 2.5f);

        float rawElev = elevNoise.GetNoise(lt.position.x, lt.position.y);
        lt.terrain.elevationNoise = rawElev - falloff;

        lt.terrain.temperatureNoise = tempNoise.GetNoise(lt.position.x, lt.position.y);
        lt.terrain.moistureNoise = moistNoise.GetNoise(lt.position.x, lt.position.y);

        lt.terrain.elevation = GetElevation(lt.terrain.elevationNoise);
        lt.terrain.temperature = GetTemperature(lt.terrain.temperatureNoise);
        lt.terrain.moisture = GetMoisture(lt.terrain.moistureNoise);

        lt.terrain.biome = DetermineBiome(lt.terrain.elevation, lt.terrain.temperature, lt.terrain.moisture);

        largeTiles[b] = lt;
    }

    return largeTiles;
}

std::vector<Tile> MapGenerator::GetMap(int mapWidth, int mapHeight, int cellSize, int iterations)
{
    std::vector<jcv_point> points = InitializeSeeds(mapWidth, mapHeight, cellSize);
    jcv_rect rect = {{50.0f, 50.0f}, {(float)mapWidth - 50.0f, (float)mapHeight - 50.0f}};
    std::random_device rd;
    std::mt19937 seedGen(rd());
    elevNoise.SetFrequency(0.0048f);
    tempNoise.SetFrequency(0.003f);
    moistNoise.SetFrequency(0.003f);
    unsigned int elevationSeed = seedGen();
    unsigned int temperatureSeed = seedGen();
    unsigned int moistureSeed = seedGen();

    elevNoise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    elevNoise.SetSeed(elevationSeed);

    tempNoise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    tempNoise.SetSeed(temperatureSeed);

    moistNoise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    moistNoise.SetSeed(moistureSeed);
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
    return MergeTiles(fineSiatka, 3, mapWidth, mapHeight);
}