#include "PoliticalSetup.hpp"
/*
 * GENERACJA TESTOWEGO PAŃSTWA
 */
void PoliticalSetup::CreateTestEmpire(GameManager &gm, const std::vector<Tile> &map)
{
    Empire testEmpire(0, "Cesarstwo Testowe", sf::Color::Red);
    std::vector<int32_t> centerIndices = {4500, 5800, 7400};

    for (size_t i = 0; i < centerIndices.size(); ++i)
    {
        int32_t centerTileID = centerIndices[i];

        City newCity;
        newCity.nameID = static_cast<uint32_t>(i);
        newCity.centerTileID = centerTileID;
        newCity.ownerEmpireID = 0;

        newCity.jurisdictionTiles.push_back(centerTileID);
        for (int32_t neighborID : map[centerTileID].neighbors)
        {
            newCity.jurisdictionTiles.push_back(neighborID);
        }

        int32_t cityID = static_cast<int32_t>(gm.GetAllCities().size());
        gm.AddCity(newCity);

        testEmpire.AddCity(cityID);

        Unit settler;
        settler.ID = static_cast<int32_t>(gm.GetAllUnits().size());
        settler.ownerEmpireID = 0;
        settler.type = UnitType::Settler;
        sf::Vector2f firstBorderVertex = map[centerTileID].cellEdges.empty() ? map[centerTileID].position : map[centerTileID].cellEdges[0].p1;
        settler.position = firstBorderVertex;
        settler.maxMovementPoints = 3;
        settler.currentMovementPoints = 3;

        gm.AddUnit(settler);
        testEmpire.AddUnit(settler.ID);
    }

    gm.AddEmpire(testEmpire);
}
static void CreateEmpires(GameManager &gm, const std::vector<Tile> &map, int count)
{
    std::random_device rd;
    for (int i = 0; i < count; i++)
    {
        // Empire em(i, "temp", sf::Color::Red);
    }
}