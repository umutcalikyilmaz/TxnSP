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
    
        Problem(int jobNumber, int machineNumber, const std::vector<double>& lengths,
            const std::vector<std::vector<uint8_t>>& conflicts);

        Problem(int jobNumber, int machineNumber, ProbabilityDistribution dist, double distributionParameter1,
        double distributionParameter2, double conflictParity);

		int getJobNumber() const;
		
		int getMachineNumber() const;

		const std::vector<double>& getLengths() const;

        double getLength(int ind) const;

		const std::vector<std::vector<uint8_t>>& getConflicts() const;

		const std::vector<LargeInt>& getDivid() const;

		LargeInt getSize() const;

        void arrangeConflicts();

    private:

        std::vector<double> lengths_;
        std::vector<LargeInt> divid_;
        std::vector<std::vector<uint8_t>> conflicts_;
        LargeInt size_;
        int jobNumber_;
        int machineNumber_;        
    };    
}