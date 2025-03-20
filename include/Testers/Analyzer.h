#pragma once
#include <fstream>
#include <map>
#include <set>
#include <sstream>
#include <thread>
#include <PoolingModels/SchedulePool.h>
#include <Structs/AnalyzerInput.h>

namespace TransactionScheduling
{
    void ThreadFunction(Problem** problems, int n, int m, int prbNum, double* spaceSizes, double* optNums, double* per1, double* per5, double* per10, double* per20);

    class Analyzer
    {
    public:
        void Analyze(const AnalyzerInput& inp);
    };
}