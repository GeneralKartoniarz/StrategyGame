#include "MapGenerator.hpp"
#include "VoronoiBuilder.hpp"
#include <random>
#include <queue>
#include <algorithm>
#include <cmath>
#include <unordered_map>
TopologyGraph MapGenerator::ExtractTopology(const std::vector<Tile>& map, int mapWidth, int mapHeight, const ClimateEngine& climate)
{
    TopologyGraph graph;
    std::unordered_map<int64_t, int> vertexMap;

    auto getHash = [](const sf::Vector2f& p) -> int64_t 
    {
        int64_t x = static_cast<int64_t>(std::round(p.x * 10.0f));
        int64_t y = static_cast<int64_t>(std::round(p.y * 10.0f));
        return (x << 32) | (y & 0xFFFFFFFF);
    };

    for (const auto& tile : map)
    {
        bool isOceanTile = (tile.terrain.biome == BiomeType::Ocean);

        for (const auto& edge : tile.cellEdges)
        {
            int64_t hash1 = getHash(edge.p1);
            int64_t hash2 = getHash(edge.p2);
            int idx1 = -1, idx2 = -1;

            if (vertexMap.find(hash1) == vertexMap.end())
            {
                idx1 = static_cast<int>(graph.nodes.size());
                vertexMap[hash1] = idx1;
                
                TopologyNode node;
                node.position = edge.p1;
                node.elevation = climate.GetVertexElevation(node.position.x, node.position.y, static_cast<float>(mapWidth), static_cast<float>(mapHeight));
                node.isWater = isOceanTile; 
                node.isRiver = false;
                
                graph.nodes.push_back(node);
            }
            else
            {
                idx1 = vertexMap[hash1];
                if (isOceanTile) graph.nodes[idx1].isWater = true; 
            }

            if (vertexMap.find(hash2) == vertexMap.end())
            {
                idx2 = static_cast<int>(graph.nodes.size());
                vertexMap[hash2] = idx2;
                
                TopologyNode node;
                node.position = edge.p2;
                node.elevation = climate.GetVertexElevation(node.position.x, node.position.y, static_cast<float>(mapWidth), static_cast<float>(mapHeight));
                node.isWater = isOceanTile; 
                node.isRiver = false;
                
                graph.nodes.push_back(node);
            }
            else
            {
                idx2 = vertexMap[hash2];
                if (isOceanTile) graph.nodes[idx2].isWater = true; 
            }

            auto& n1 = graph.nodes[idx1].neighbors;
            if (std::find(n1.begin(), n1.end(), idx2) == n1.end()) n1.push_back(idx2);

            auto& n2 = graph.nodes[idx2].neighbors;
            if (std::find(n2.begin(), n2.end(), idx1) == n2.end()) n2.push_back(idx1);
        }
    }

    return graph;
}

/*
 * AGREGACJA GEOMETRII (KLASTERYZACJA ALGORYTMEM BFS)
 * Łączymy małe, surowe komórki Voronoia w większe regiony prowincjonalne.
 * Ta funkcja odpowiada wyłącznie za scalanie granic, wektorów wierzchołków i sąsiedztwa.
 */
std::vector<Tile> MapGenerator::MergeTiles(const std::vector<Tile> &smallTiles, int targetClusterSize, int mapWidth, int mapHeight)
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

    for (size_t b = 0; b < clusters.size(); ++b)
    {
        Tile lt;
        lt.ID = static_cast<int32_t>(b);

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
                    lt.cellEdges.push_back({cEdge.p1, cEdge.p2, bigNeighborID});
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
void MapGenerator::GenerateRivers(TopologyGraph& graph, int numRivers)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(0, static_cast<int>(graph.nodes.size() - 1));

    for (int i = 0; i < numRivers; ++i)
    {
        int currIdx = dist(gen);
        int attempts = 0;

        while ((graph.nodes[currIdx].isWater || graph.nodes[currIdx].elevation < 0.2f) && attempts < 100)
        {
            currIdx = dist(gen);
            attempts++;
        }

        if (attempts >= 100) continue;

        while (!graph.nodes[currIdx].isWater)
        {
            graph.nodes[currIdx].isRiver = true;

            int lowestNeighbor = -1;
            float minElev = graph.nodes[currIdx].elevation;

            for (int nIdx : graph.nodes[currIdx].neighbors)
            {
                if (graph.nodes[nIdx].elevation < minElev)
                {
                    minElev = graph.nodes[nIdx].elevation;
                    lowestNeighbor = nIdx;
                }
            }

            if (lowestNeighbor == -1) 
            {
                break; 
            }

            currIdx = lowestNeighbor;
        }
        
        if (graph.nodes[currIdx].isWater)
        {
            graph.nodes[currIdx].isRiver = true;
        }
    }
}
std::vector<Tile> MapGenerator::GetMap(int mapWidth, int mapHeight, int cellSize, int iterations, ClimateConfig config)
{
    std::random_device rd;
    std::mt19937 seedGen(rd());
    
    ClimateEngine climate(seedGen());

    std::vector<Tile> fineSiatka = VoronoiBuilder::GenerateRawGrid(mapWidth, mapHeight, cellSize, iterations);
    std::vector<Tile> finalMap = MergeTiles(fineSiatka, 3, mapWidth, mapHeight);
    
    climate.ApplyClimate(finalMap, mapWidth, mapHeight, config);

    return finalMap;
}