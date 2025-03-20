#pragma once
#include <Solvers/Solver.h>
#include <scip/scip.h>
#include <scip/scipdefplugins.h>
#include <scip/cons_linear.h>
#include <ProblemModels/Problem.h>
#include <Solvers/Solver.h>

using namespace std;

namespace TransactionScheduling
{
	class MIPSolver : public Solver
	{
	public:		

		SolverOutput* Solve(const SolverInput& input) override;
	};
}