#include "PoliticalSetup.hpp"
/*
 * [PL] METODA: CreateTestEmpire
 * LOGIKA: Wstrzykuje na mapę środowisko testowe (państwo + Settler) na suchym lądzie.
 * POWIĄZANIA: GameManager.
 * [EN] METHOD: CreateTestEmpire
 * LOGIC: Injects a test environment (empire + Settler) onto dry land on the map.
 * DEPENDENCIES: GameManager.
 */
 void PoliticalSetup::CreateTestEmpire(GameManager &gm, const std::vector<Tile> &map)
{
    Empire testEmpire(0, "Cesarstwo Testowe", sf::Color::Red);

    int32_t dropZoneTileID = 0;
    for (size_t i = 0; i < map.size(); ++i)
    {
        if (map[i].terrain.biome != BiomeType::Ocean)
        {
            dropZoneTileID = static_cast<int32_t>(i);
            break;
        }
    }

    Unit settler;
    settler.ID = static_cast<int32_t>(gm.GetAllUnits().size());
    settler.ownerEmpireID = 0;
    settler.type = UnitType::Settler;
    settler.colonization = ColonizationComponent();
    settler.combat = std::nullopt;

    sf::Vector2f firstBorderVertex = map[dropZoneTileID].cellEdges.empty() ? map[dropZoneTileID].position : map[dropZoneTileID].cellEdges[0].p1;
    settler.currentNodeID = gm.GetNearestNodeID(firstBorderVertex);

    if (settler.currentNodeID != -1)
    {
        settler.position = gm.GetNavGraph().nodes[settler.currentNodeID].position;
    }
    else
    {
        settler.position = firstBorderVertex;
    }

    settler.maxMovementPoints = 3;
    settler.currentMovementPoints = 3;

    gm.AddUnit(settler);
    testEmpire.AddUnit(settler.ID);
    gm.AddEmpire(testEmpire);
}
/*
 * [PL] METODA: CreateEmpires
 * LOGIKA: Przyszła metoda do losowego rozmieszczania "N" imperiów komputerowych.
 * [EN] METHOD: CreateEmpires
 * LOGIC: Future automated method for randomly placing 'N' AI empires.
 */
 void PoliticalSetup::CreateEmpires(GameManager &gm, const std::vector<Tile> &map, int count)
{
    std::random_device rd;
    for (int i = 0; i < count; i++)
    {
        // Empire em(i, "temp", sf::Color::Red);
    }
}