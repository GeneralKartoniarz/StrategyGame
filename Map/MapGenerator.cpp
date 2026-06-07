#define JC_VORONOI_IMPLEMENTATION
#include "MapGenerator.hpp"
#include <random>
#include "Tile.hpp"
#include <SFML/Graphics.hpp>
#include <queue>
#include <algorithm>
#include <cmath>

Elevation GetElevation(float noise, const ClimateConfig &config)
{
    if (noise < config.waterThreshold)
        return Elevation::Water;
    if (noise < config.plainsThreshold)
        return Elevation::Plains;
    if (noise < config.hillsThreshold)
        return Elevation::Hills;
    return Elevation::Mountains;
}

Temperature GetTemperature(float noise, const ClimateConfig &config)
{
    if (noise < config.coldThreshold)
        return Temperature::Cold;
    if (noise < config.temperateThreshold)
        return Temperature::Temperate;
    return Temperature::Hot;
}

Moisture GetMoisture(float noise, const ClimateConfig &config)
{
    if (noise < config.dryThreshold)
        return Moisture::Dry;
    if (noise < config.normalThreshold)
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
    double sumX = 0.0, sumY = 0.0;
    int count = 0;
    const jcv_graphedge *edge = site->edges;

    while (edge)
    {
        sumX += edge->pos[0].x + edge->pos[1].x;
        sumY += edge->pos[0].y + edge->pos[1].y;
        count += 2;
        edge = edge->next;
    }

    if (count == 0)
    {
        return sf::Vector2f(static_cast<float>(site->p.x), static_cast<float>(site->p.y));
    }
    return sf::Vector2f(static_cast<float>(sumX / count), static_cast<float>(sumY / count));
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
            std::uniform_real_distribution<double> distX(margin + x * cellSize, margin + (x + 1) * cellSize);
            std::uniform_real_distribution<double> distY(margin + y * cellSize, margin + (y + 1) * cellSize);
            points.push_back({static_cast<jcv_real>(distX(gen)), static_cast<jcv_real>(distY(gen))});
        }
    }
    return points;
}

std::vector<Tile> MapGenerator::CreateTiles(const std::vector<jcv_point> &points, int mapWidth, int mapHeight)
{
    jcv_diagram diagram;
    jcv_rect rect = {{50.0, 50.0}, {static_cast<double>(mapWidth) - 50.0, static_cast<double>(mapHeight) - 50.0}};
    memset(&diagram, 0, sizeof(jcv_diagram));
    jcv_diagram_generate(points.size(), points.data(), &rect, 0, &diagram);

    std::vector<Tile> tiles(points.size());
    const jcv_site *sites = jcv_diagram_get_sites(&diagram);

    for (int i = 0; i < diagram.numsites; ++i)
    {
        const jcv_site *site = &sites[i];
        Tile t;
        t.ID = site->index;

        t.position = {static_cast<float>(site->p.x), static_cast<float>(site->p.y)};

        const jcv_graphedge *edge = site->edges;
        while (edge)
        {
            t.vertices.push_back({static_cast<float>(edge->pos[0].x), static_cast<float>(edge->pos[0].y)});

            int nID = edge->neighbor ? edge->neighbor->index : -1;
            if (nID != -1)
            {
                t.neighbors.push_back(nID);
            }

            t.cellEdges.push_back({sf::Vector2f(static_cast<float>(edge->pos[0].x), static_cast<float>(edge->pos[0].y)),
                                   sf::Vector2f(static_cast<float>(edge->pos[1].x), static_cast<float>(edge->pos[1].y)),
                                   nID});

            edge = edge->next;
        }

        tiles[site->index] = t;
    }
    jcv_diagram_free(&diagram);
    return tiles;
}
std::vector<Tile> MapGenerator::MergeTiles(const std::vector<Tile> &smallTiles, int targetClusterSize, int mapWidth, int mapHeight, const ClimateConfig &config)
{
    std::vector<bool> isAssigned(smallTiles.size(), false);
    std::vector<int> smallToBig(smallTiles.size(), -1);
    std::vector<std::vector<int>> clusters;

    std::vector<bool> inQueue(smallTiles.size(), false);

    for (size_t i = 0; i < smallTiles.size(); ++i)
    {
        if (isAssigned[i])
            continue;

        std::vector<int> currentCluster;
        std::queue<int> q;

        std::vector<int> nodesToReset;

        q.push(static_cast<int>(i));
        inQueue[i] = true;
        nodesToReset.push_back(static_cast<int>(i));

        while (!q.empty() && currentCluster.size() < static_cast<size_t>(targetClusterSize))
        {
            int curr = q.front();
            q.pop();

            if (isAssigned[curr])
                continue;

            isAssigned[curr] = true;
            currentCluster.push_back(curr);
            smallToBig[curr] = static_cast<int>(clusters.size());

            for (int neighborID : smallTiles[curr].neighbors)
            {
                if (!isAssigned[neighborID] && !inQueue[neighborID])
                {
                    q.push(neighborID);
                    inQueue[neighborID] = true;
                    nodesToReset.push_back(neighborID);
                }
            }
        }

        for (int node : nodesToReset)
        {
            if (!isAssigned[node])
            {
                inQueue[node] = false;
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

        float continental = continentalNoise.GetNoise(lt.position.x, lt.position.y);
        float detailElevation = elevNoise.GetNoise(lt.position.x, lt.position.y);
        float combinedElev = continental + (detailElevation * 0.4f);

        lt.terrain.elevationNoise = combinedElev - falloff;

        lt.terrain.temperatureNoise = tempNoise.GetNoise(lt.position.x, lt.position.y);
        lt.terrain.moistureNoise = moistNoise.GetNoise(lt.position.x, lt.position.y);

        lt.terrain.elevation = GetElevation(lt.terrain.elevationNoise, config);
        lt.terrain.temperature = GetTemperature(lt.terrain.temperatureNoise, config);
        lt.terrain.moisture = GetMoisture(lt.terrain.moistureNoise, config);

        lt.terrain.biome = DetermineBiome(lt.terrain.elevation, lt.terrain.temperature, lt.terrain.moisture);

        largeTiles[b] = lt;
    }

    return largeTiles;
}
std::vector<Tile> MapGenerator::GetMap(int mapWidth, int mapHeight, int cellSize, int iterations, ClimateConfig config)
{
    std::vector<jcv_point> points = InitializeSeeds(mapWidth, mapHeight, cellSize);
    jcv_rect rect = {{50.0, 50.0}, {static_cast<double>(mapWidth) - 50.0, static_cast<double>(mapHeight) - 50.0}};
    std::random_device rd;
    std::mt19937 seedGen(rd());

    elevNoise.SetFrequency(0.0040f);
    tempNoise.SetFrequency(0.0038f);
    moistNoise.SetFrequency(0.0038f);
    continentalNoise.SetFrequency(0.004f);

    unsigned int elevationSeed = seedGen();
    unsigned int temperatureSeed = seedGen();
    unsigned int moistureSeed = seedGen();

    elevNoise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    elevNoise.SetSeed(elevationSeed);

    tempNoise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    tempNoise.SetSeed(temperatureSeed);

    moistNoise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    moistNoise.SetSeed(moistureSeed);

    continentalNoise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    continentalNoise.SetSeed(seedGen());

    for (int it = 0; it < iterations; ++it)
    {
        jcv_diagram diagram;
        memset(&diagram, 0, sizeof(jcv_diagram));
        jcv_diagram_generate(points.size(), points.data(), &rect, 0, &diagram);

        const jcv_site *sites = jcv_diagram_get_sites(&diagram);
        for (int i = 0; i < diagram.numsites; ++i)
        {
            sf::Vector2f centroid = CalculateCentroid(&sites[i]);
            points[sites[i].index].x = static_cast<double>(centroid.x);
            points[sites[i].index].y = static_cast<double>(centroid.y);
        }
        jcv_diagram_free(&diagram);
    }

    std::vector<Tile> fineSiatka = CreateTiles(points, mapWidth, mapHeight);
    return MergeTiles(fineSiatka, 3, mapWidth, mapHeight, config);
}