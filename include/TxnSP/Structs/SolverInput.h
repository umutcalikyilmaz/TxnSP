#pragma once
#include "TxnSP/ProblemModels/Problem.h"

namespace TxnSP
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