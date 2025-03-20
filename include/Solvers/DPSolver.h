#pragma once
#include <map>
#include <PoolingModels/SubsetPool.h>
#include <Shared/Decode.h>
#include <Shared/Math.h>
#include <Solvers/Solver.h>

namespace TransactionScheduling
{
    class DPSolver : public Solver
    {
        SchedulePool* schp;
		SubsetPool* sp;		
		int n;
		int m;		
		int ssNum;
		int schNum;
        SolutionType type;
        bool init;

        SolverOutput* SolveExact(Problem* prb);

		SolverOutput* SolveApproximate(Problem* prb);
		
    public:
    
        DPSolver();

        SolverOutput* Solve(const SolverInput& input) override;

        ~DPSolver();
    };
}