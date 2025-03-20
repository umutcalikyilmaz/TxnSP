#pragma once
#include <ProblemModels/Problem.h>

namespace TransactionScheduling
{
    struct SolverInput
    {
        Problem* prb;
        double SA_DecrementParameter;
        double SA_MaxTemperature;
        TemperatureEvolution SA_DecrementType;
        SolutionType DP_SolutionType;
    };
}