#pragma once
#include "TxnSP/PoolingModels/SchedulePool.h"
#include "TxnSP/Solvers/Solver.h"

namespace TxnSP
{
    class ESSolver : public Solver
    {        
    public:

        SolverOutput* solve(const SolverInput& input) override;
    };
}