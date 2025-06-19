#pragma once
#include <float.h>
#include <vector>
#include "TxnSP/ProblemModels/Schedule.h"

using namespace std;

namespace TxnSP
{
    struct SolverOutput
    {
        vector<vector<int>> jobs;  
        vector<double> processingTimes;
        vector<double> startingTimes;
        vector<double> endingTimes;
        vector<int> assignments;
        vector<vector<int>> conflicts;
        double makespan;
		double minimumTime;
        double runtime;
        int jobNumber;
        int machineNumber;

        SolverOutput(Problem* prb, double runtime);

        SolverOutput(Problem* prb, int* state, double runtime);

        SolverOutput(Problem* prb, Schedule* sch, double runtime);

        SolverOutput(Problem* prb, int* x, double* s, double runtime);

        ~SolverOutput();
    };
}
