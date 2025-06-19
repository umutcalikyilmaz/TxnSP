#pragma once
#include "TxnSP/Structs/SolverInput.h"
#include "TxnSP/Structs/SolverOutput.h"

namespace TxnSP
{
    class Solver
    {
    public:    
        virtual SolverOutput* solve(const SolverInput& input) = 0;
    };
}