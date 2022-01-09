#ifndef TRIAD_OPTIMIZER_H
#define TRIAD_OPTIMIZER_H


#include "pn.h"
#include "triad.h"

class TriadOptimizer
{
public:
    TriadOptimizer() {};

    std::vector<PnToken> optimize(std::vector<PnToken> pn);

// private:
    std::vector<Triad> getTriads(std::vector<PnToken> pn);
    std::vector<Triad> optimizeConstants(std::vector<Triad> triads);
    std::vector<Triad> optimizeSameOperations(std::vector<Triad> triads,
                                              std::vector<std::pair<int,int>> blocks);
    std::vector<Triad> optimizeAssigns(std::vector<Triad> triads,
                                       std::vector<std::pair<int,int>> blocks);

    std::vector<std::pair<int,int>> getBlocks(std::vector<Triad> triads);

    std::vector<PnToken> generatePn(std::vector<Triad> triads);
    // std::vector<std::vector<PnToken>> getPnTokensToOptimize(std::vector<PnToken> tokens);
};


#endif  // TRIAD_OPTIMIZER_H
