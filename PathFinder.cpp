#include "Pathfinder.hpp"
#include <queue>
#include <unordered_map>
#include <cmath>
#include <algorithm>
float Pathfinder::Heuristic(sf::Vector2f a, sf::Vector2f b)
{
    return std::sqrt(std::pow(a.x - b.x, 2) + std::pow(a.y - b.y, 2));
}

std::vector<int32_t> Pathfinder::FindPath(const NavigationGraph& graph, int32_t startNodeID, int32_t targetNodeID)
{
    if (startNodeID < 0 || targetNodeID < 0 || startNodeID >= static_cast<int32_t>(graph.nodes.size()) || targetNodeID >= static_cast<int32_t>(graph.nodes.size())) 
    {
        return {};
    }

    std::unordered_map<int32_t, int32_t> cameFrom;
    std::unordered_map<int32_t, float> gScore;
    
    auto cmp = [&gScore, &graph, targetNodeID](int32_t left, int32_t right) 
    {
        float fLeft = gScore[left] + Pathfinder::Heuristic(graph.nodes[left].position, graph.nodes[targetNodeID].position);
        float fRight = gScore[right] + Pathfinder::Heuristic(graph.nodes[right].position, graph.nodes[targetNodeID].position);
        return fLeft > fRight;
    };
    
    std::priority_queue<int32_t, std::vector<int32_t>, decltype(cmp)> openSet(cmp);

    gScore[startNodeID] = 0.0f;
    openSet.push(startNodeID);

    while (!openSet.empty())
    {
        int32_t current = openSet.top();
        openSet.pop();

        if (current == targetNodeID)
        {
            std::vector<int32_t> path;
            while (current != startNodeID)
            {
                path.push_back(current);
                current = cameFrom[current];
            }
            path.push_back(startNodeID);
            std::reverse(path.begin(), path.end());
            return path;
        }

        for (int32_t neighbor : graph.nodes[current].connectedNodes)
        {
            float tentative_gScore = gScore[current] + Heuristic(graph.nodes[current].position, graph.nodes[neighbor].position);

            if (gScore.find(neighbor) == gScore.end() || tentative_gScore < gScore[neighbor])
            {
                cameFrom[neighbor] = current;
                gScore[neighbor] = tentative_gScore;
                openSet.push(neighbor);
            }
        }
    }

    return {};
}