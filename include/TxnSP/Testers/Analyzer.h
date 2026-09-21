#pragma once
#include <filesystem>
#include <fstream>
#include <map>
#include <mutex>
#include <queue>
#include <set>
#include <sstream>
#include <thread>
#include "TxnSP/PoolingModels/SchedulePool.h"
#include "TxnSP/Structs/AnalyzerInput.h"

namespace TxnSP
{
    class Analyzer
    {
    public:

        void analyze(const AnalyzerInput& inp);

    private:

        std::mutex problemLock_;
        std::queue<std::unique_ptr<Problem>> problemQueue_;
        std::queue<int> indexQueue_;

        bool pullProblem(std::unique_ptr<Problem>& prb, int& ind);

        void threadFunction(int n, int m, std::vector<int>& indices, std::vector<double>& spaceSizes,
            std::vector<double>& optNums, std::vector<double>& per1, std::vector<double>& per5,
            std::vector<double>& per10, std::vector<double>& per20);
    };
}