#pragma once
#include <float.h>
#include <vector>
#include "TxnSP/ProblemModels/Schedule.h"

namespace TxnSP
{
    struct SolverOutput
    {
        std::vector<std::vector<int>> jobs;  
        std::vector<double> processingTimes;
        std::vector<double> startingTimes;
        std::vector<double> endingTimes;
        std::vector<int> assignments;
        std::vector<std::vector<int>> conflicts;
        double makespan;
		double minimumTime;
        double runtime;
        int jobNumber;
        int machineNumber;

        SolverOutput(Problem* prb, double runtime);

        SolverOutput(Problem* prb, const std::vector<int>& state, double runtime);

        SolverOutput(Problem* prb, Schedule* sch, double runtime);

        SolverOutput(Problem* prb, const std::vector<int>& x, const std::vector<double>& s, double runtime);

        ~SolverOutput();
    };
}
