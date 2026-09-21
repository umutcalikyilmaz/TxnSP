#pragma once
#include "TxnSP/PoolingModels/SchedulePool.h"
#include "TxnSP/Shared/SharedFunctions.h"
#include "TxnSP/Solvers/Solver.h"

namespace TxnSP
{
    class SASolver : public Solver
    {
    public:

        SolverOutput solve(const SolverInput& input) override;

    private:

        std::unique_ptr<UniformRandomDoubleGenerator> prob_;
        std::unique_ptr<UniformRandomIntGenerator> rnd_;
        void (SASolver::*decrementPtr_)();
        double T_;
        double decrementParameter_;        
        
        void decrementExponential();

        void decrementLinear();

        void decrementSlow();

        void permute(std::vector<int>& list, int ind1, int ind2);

        void randomize(int size, int& ind1, int& ind2);

        void randomize(std::vector<int>& list, int size);
    };
}