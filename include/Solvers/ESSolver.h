#pragma once
#include <PoolingModels/SchedulePool.h>
#include <Solvers/Solver.h>

namespace TransactionScheduling
{
    class ESSolver : public Solver
    {        
    public:

        SolverOutput* Solve(const SolverInput& input) override;        
    };
}