#pragma once
#include "TxnSP/Shared/SharedParameters.h"

namespace TxnSP
{
    struct AnalyzerInput
    {
        string directory;
        double conflictParityStepSize;
        double distributionParameter1;
        double distributionParameter2;
        int jobNumber;
        int machineNumber;
        int problemNumber;
        int threadCount;
        ProbabilityDistribution distribution;
    };
    
}