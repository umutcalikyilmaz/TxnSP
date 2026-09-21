#include "TxnSP/ProblemModels/Problem.h"

namespace TxnSP
{
    Problem::Problem(int jobNumber, int machineNumber, const std::vector<double>& lengths,
        const std::vector<std::vector<uint8_t>>& conflicts)
        : jobNumber_(jobNumber),
          machineNumber_(machineNumber),
          lengths_(lengths),
          conflicts_(conflicts),
          divid_(jobNumber),
          size_(1)
    {
        for (int i = 2; i <= jobNumber; i++)
		{
			size_ *= i;
		}

		for (int i = 0; i < jobNumber; i++)
		{
			divid_[i] = 1;

			for (int j = 2; j < (jobNumber - i); j++)
			{
				divid_[i] *= j;
			}
		}
    }

    Problem::Problem(int jobNumber, int machineNumber, ProbabilityDistribution dist, double distributionparameter1,
        double distributionParameter2, double conflictParity)
        : jobNumber_(jobNumber),
          machineNumber_(machineNumber),
          size_(1),
          lengths_(jobNumber),
          divid_(jobNumber),
          conflicts_(jobNumber)
    {
        for (int i = 2; i <= jobNumber; i++)
		{
			size_ *= i;
		}

        std::unique_ptr<NormalRandomNumberGenerator> nrnd;
        std::unique_ptr<UniformRandomDoubleGenerator> urnd;
        UniformRandomDoubleGenerator rnd2(0, 1);

        if(dist == ProbabilityDistribution::Normal)
        {
            nrnd = std::make_unique<NormalRandomNumberGenerator>(distributionparameter1, distributionParameter2);
        }
        else
        {
            urnd = std::make_unique<UniformRandomDoubleGenerator>(distributionparameter1, distributionParameter2);
        }

        double p;

        for(int i = 0; i < jobNumber; i++)
        {
            do
            {
                lengths_[i] = (dist == ProbabilityDistribution::Normal) ? nrnd->generate() : urnd->generate();
            }
            while(lengths_[i] < 0);
            
            divid_[i] = 1;
            conflicts_[i].resize(jobNumber);
            conflicts_[i][i] = 0;

            for(int j = 0; j < i; j++)
            {
                p = rnd2.generate();
                conflicts_[i][j] = p < conflictParity ? 1 : 0;
            }

            for(int j = 2; j < (jobNumber - i); j++)
			{
				divid_[i] *= j;
			}
        }

        for (int i = 0; i < jobNumber; i++)
		{
			for (int j = i + 1; j < jobNumber; j++)
			{
				conflicts_[i][j] = conflicts_[j][i];
			}
		}
    }

    int Problem::getJobNumber() const
    {
        return jobNumber_;
    }
		
	int Problem::getMachineNumber() const
    {
        return machineNumber_;
    }

	const std::vector<double>& Problem::getLengths() const
    {
        return lengths_;
    }

    double Problem::getLength(int ind) const
    {
        return lengths_[ind];
    }

    const std::vector<std::vector<uint8_t>>& Problem::getConflicts() const
    {
        return conflicts_;
    }

	const std::vector<LargeInt>& Problem::getDivid() const
    {
        return divid_;
    }

	LargeInt Problem::getSize() const
    {
        return size_;
    }

    void Problem::arrangeConflicts()
    {
        for (int i = 0; i < jobNumber_; i++)
		{
            conflicts_[i][i] = 0;
            
			for (int j = i + 1; j < jobNumber_; j++)
			{
				conflicts_[i][j] = conflicts_[j][i];
			}
		}
    }
}