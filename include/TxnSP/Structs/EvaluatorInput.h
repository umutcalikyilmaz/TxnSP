#pragma once
#include <utility>
#include <vector>
#include "TxnSP/Shared/SharedParameters.h"

namespace TxnSP
{
    struct EvaluatorInput
    {
        string directory;
        std::vector<std::pair<double,double>> uniformParameters;
        std::vector<std::pair<double,double>> normalParameters;
        std::vector<double>conflictParities;
        std::vector<std::pair<TemperatureEvolution,double>> SA_DecrementTypesAndParameters;
        std::vector<double> SA_MaxTemperatures;
        std::vector<int> jobNumbers;
        std::vector<int> machineNumbers;
        std::vector<Problem*> problems;
        int problemNumber;
        int threadCount;
        SolverType baseline;
        bool mip;
        bool es;
        bool dp_exact;
        bool dp_approximate;
        bool preset;
    };
    
}