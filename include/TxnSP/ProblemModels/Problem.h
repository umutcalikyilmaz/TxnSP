#pragma once
#include <cstdlib>
#include <chrono>
#include <math.h>
#include "TxnSP/Shared/SharedParameters.h"
#include "TxnSP/Shared/SharedModels.h"

namespace TxnSP
{
    class Problem
    {
    public:
    
        Problem(int jobNumber, int machineNumber, double* lengths, bool** conflicts);

        Problem(int jobNumber, int machineNumber, ProbabilityDistribution dist, double distributionParameter1,
        double distributionParameter2, double conflictParity);

		int getJobNumber() const;
		
		int getMachineNumber() const;

		double* getLengths() const;

        double getLength(int ind) const;

		bool** getConflicts() const;

		int* getDivid() const;

		__uint128_t getSize() const;

        void arrangeConflicts();

        ~Problem();

    private:

        double* lengths_;
        int* divid_;
        bool** conflicts_;
        __uint128_t size_;
        int jobNumber_;
        int machineNumber_;        
    };    
}