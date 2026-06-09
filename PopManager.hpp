#pragma once
#include <vector>
#include "Pop.hpp"

class PopManager 
{
public:
    PopManager() = default;
    ~PopManager() = default;

    void AddPop(const Pop& newPop); 
    void TransferPopsByTile(int32_t tileID, PopManager& winnerManager);
    const std::vector<Pop>& GetAllPops() const { return population; }

private:
    std::vector<Pop> population;
};