#pragma once
#include <map>
#include "TxnSP/PoolingModels/SubsetPool.h"
#include "TxnSP/Shared/SharedFunctions.h"
#include "TxnSP/Solvers/Solver.h"

namespace TxnSP
{
    class DPSolver : public Solver
    {
    public:

        SolverOutput solve(const SolverInput& input) override;

    private:
        
        std::unique_ptr<SchedulePool> schedulePool_;
        std::unique_ptr<SubsetPool> subsetPool_;
        int jobNumber_;
        int machineNumber_;
        SolutionType type_;

        SolverOutput solveExact(Problem* prb);

        SolverOutput solveApproximate(Problem* prb);
    };
}