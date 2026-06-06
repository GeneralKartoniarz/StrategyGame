#include "MapGenerator.hpp"
#include <random>

std::vector<sf::Vector2f> MapGenerator::GenerateSeeds(int mapWidth, int mapHeight, int cellSize)
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

    return seeds;
}