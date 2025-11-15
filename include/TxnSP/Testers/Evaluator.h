#pragma once
#include <fstream>
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
    void ThreadFunction(Problem** problems, int prbNum, bool es, bool mip, bool dp_exact, bool dp_approximate,
    const std::vector<std::pair<TemperatureEvolution,double>> SA_DecrementTypesAndParameters, 
    const std::vector<double> SA_MaxTemperatures, double* dpeTime, double* dpeVal, double* dpaTime, double* dpaVal,
    double* esTime, double* esVal, double* mipTime, double* mipVal, double** saTime, double** saVal);

    class Evaluator
    {
    public:

        void evaluate(const EvaluatorInput& inp);

    private:

        void evaluatePreset(const EvaluatorInput& inp);

        void evaluateRandom(const EvaluatorInput& inp);
    };
}