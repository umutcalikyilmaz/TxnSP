#pragma once
#include "scip/scip.h"
#include "scip/scipdefplugins.h"
#include "scip/cons_linear.h"
#include "TxnSP/ProblemModels/Problem.h"
#include "TxnSP/Solvers/Solver.h"

namespace TxnSP
{
	class MIPSolver : public Solver
	{
	public:		

		SolverOutput* solve(const SolverInput& input) override;
	};
}