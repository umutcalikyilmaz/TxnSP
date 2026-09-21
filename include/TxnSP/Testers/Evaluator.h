#pragma once
#include <filesystem>
#include <fstream>
#include <mutex>
#include <queue>
#include <sstream>
#include <thread>
#include "TxnSP/TxnSP.h"
#include "TxnSP/Solvers/DPSolver.h"
#include "TxnSP/Solvers/ESSolver.h"
#include "TxnSP/Solvers/SASolver.h"
#include "TxnSP/Structs/EvaluatorInput.h"

#ifdef ENABLE_MIP
    #include "TxnSP/Solvers/MIPSolver.h"
#endif

namespace TxnSP
{
    class Evaluator
    {
    public:

        void evaluate(const EvaluatorInput& inp);

    private:

        std::mutex problemLock_;
        std::queue<Problem*> problemQueue_;
        std::queue<int> indexQueue_;

        bool pullProblem(Problem*& prb, int& ind);

        void threadFunction(bool es, bool mip, bool dp_exact, bool dp_approximate,
            const std::vector<std::pair<TemperatureEvolution,double>>& SA_DecrementTypesAndParameters, 
            const std::vector<double>& SA_MaxTemperatures, std::vector<int>& indices,
            std::vector<double>& dpeTime, std::vector<double>& dpeVal, std::vector<double>& dpaTime,
            std::vector<double>& dpaVal, std::vector<double>& esTime, std::vector<double>& esVal,
            std::vector<double>& mipTime, std::vector<double>& mipVal,
            std::vector<std::vector<double>>& saTime, std::vector<std::vector<double>>& saVal);

        void evaluatePreset(const EvaluatorInput& inp);

        void evaluateRandom(const EvaluatorInput& inp);
    };
}