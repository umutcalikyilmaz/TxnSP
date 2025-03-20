#pragma once
#include <vector>
#include <ProblemModels/Schedule.h>

namespace TransactionScheduling
{
    struct SolverOutput
    {
        vector<vector<int>> cores;  
        vector<double> coreTimes;
        vector<double> startingTimes;
        vector<double> endingTimes;
        vector<int> assignments;
        vector<vector<int>> conflicts;
        double makespan;
		double mintime;
        double runtime;
        int n;
        int m;

        SolverOutput(Problem* prb, Schedule* sch, double runtime);

        SolverOutput(Problem* prb, int* x, double* s, double runtime);

        ~SolverOutput();
    };
}
