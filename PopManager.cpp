#include "PopManager.hpp"

void PopManager::AddPop(const Pop& newPop)
{
    this->population.push_back(newPop);
}
void PopManager::TransferPopsByTile(int32_t tileID, PopManager& winnerManager)
{
    for (int i = static_cast<int>(this->population.size()) - 1; i >= 0; --i)
    {
        if (this->population[i].locationTileID == tileID)
        {
            winnerManager.AddPop(this->population[i]);

            this->population[i] = this->population.back();
            this->population.pop_back();
        }
    }
}