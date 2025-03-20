#pragma once
#include <Structs/SolverInput.h>
#include <Structs/SolverOutput.h>

namespace TransactionScheduling
{
    class Solver
    {
    public:    
        virtual SolverOutput* Solve(const SolverInput& input) = 0;
    };
}