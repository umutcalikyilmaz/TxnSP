#pragma once
#include <fstream>
#include <sstream>
#include <thread>
#include <TxnSPLib.h>
#include <Solvers/DPSolver.h>
#include <Solvers/ESSolver.h>
#include <Solvers/SASolver.h>
#include <Structs/EvaluatorInput.h>

namespace TransactionScheduling
{
    void ThreadFunction(Problem** problems, int prbNum, bool es, bool mip, bool dp_exact, bool dp_approximate,
    const std::vector<std::pair<TemperatureEvolution,double>> SA_DecrementTypesAndParameters, 
    const std::vector<double> SA_MaxTemperatures, double* dpeTime, double* dpeVal, double* dpaTime, double* dpaVal,
    double* esTime, double* esVal, double* mipTime, double* mipVal, double** saTime, double** saVal);

    class Evaluator
    {
        void EvaluatePreset(const EvaluatorInput& inp);

        void EvaluateRandom(const EvaluatorInput& inp);

    public:

        void Evaluate(const EvaluatorInput& inp);
    };
}