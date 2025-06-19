#pragma once
#include <fstream>
#include <map>
#include <set>
#include <sstream>
#include <thread>
#include "TxnSP/PoolingModels/SchedulePool.h"
#include "TxnSP/Structs/AnalyzerInput.h"

namespace TxnSP
{
    void ThreadFunction(Problem** problems, int n, int m, int prbNum, double* spaceSizes, double* optNums, double* per1, double* per5, double* per10, double* per20);

    class Analyzer
    {
    public:
        void analyze(const AnalyzerInput& inp);
    };
}