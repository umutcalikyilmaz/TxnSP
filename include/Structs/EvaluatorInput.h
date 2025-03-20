#pragma once
#include <utility>
#include <vector>
#include <Shared/Parameters.h>

namespace TransactionScheduling
{
    struct EvaluatorInput
    {
        string directory;
        std::vector<std::pair<double,double>> uniformParameters;
        std::vector<std::pair<double,double>> normalParameters;
        std::vector<double>cps;
        std::vector<std::pair<TemperatureEvolution,double>> SA_DecrementTypesAndParameters;
        std::vector<double> SA_MaxTemperatures;
        std::vector<int> nValues;
        std::vector<int> mValues;
        std::vector<Problem*> problems;
        int prbNum;
        int threadCount;
        SolverType baseline;
        bool mip;
        bool es;
        bool dp_exact;
        bool dp_approximate;
        bool preset;
    };
    
}