#pragma once
#include "TxnSP/PoolingModels/SchedulePool.h"
#include "TxnSP/Shared/SharedFunctions.h"
#include "TxnSP/Solvers/Solver.h"

namespace TxnSP
{
    class SASolver : public Solver
    {
    public:

        SolverOutput* solve(const SolverInput& input) override;

    private:

        UniformRandomDoubleGenerator* prob_;
        UniformRandomIntGenerator* rnd_;
        void (SASolver::*decrementPtr_)();
        double T_;
        double decrementParameter_;        
        
        void decrementExponential();

        void decrementLinear();

        void decrementSlow();

        void permute(int* list, int ind1, int ind2);

        void randomize(int size, int* ind1, int* ind2);

        void randomize(int* list, int size);
    };
}