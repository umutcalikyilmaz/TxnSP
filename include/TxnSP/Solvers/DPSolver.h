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
    
        DPSolver();

        SolverOutput* solve(const SolverInput& input) override;

        ~DPSolver();

    private:
        
        SchedulePool* schedulePool_;
        SubsetPool* subsetPool_;
        int jobNumber_;
        int machineNumber_;		
        int subsetNumber_;
        int scheduleNumber_;
        SolutionType type_;
        bool init_;

        SolverOutput* solveExact(Problem* prb);

        SolverOutput* solveApproximate(Problem* prb);
    };
}