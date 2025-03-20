#pragma once
#include <PoolingModels/SchedulePool.h>
#include <Shared/List.h>
#include <Shared/Random.h>
#include <Solvers/Solver.h>

namespace TransactionScheduling
{
    class SASolver : public Solver
    {
        UniformRandomDoubleGenerator* prob;
        UniformRandomIntGenerator* rnd;
        void (SASolver::*decrementPtr)();
        double T;
        double decrementParameter;        
        
        void DecrementExponential();

        void DecrementLinear();

        void DecrementSlow();

        void Permute(int* list, int ind1, int ind2);

        void Randomize(int size, int* ind1, int* ind2);

        void Randomize(int* list, int size);

    public:

        SolverOutput* Solve(const SolverInput& input) override;
    };
}