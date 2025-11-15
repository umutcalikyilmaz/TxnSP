#include "TxnSP/ProblemModels/Problem.h"

namespace TxnSP
{
    Problem::Problem(int jobNumber, int machineNumber, double* lengths, bool** conflicts) : jobNumber_(jobNumber),
    machineNumber_(machineNumber), lengths_(lengths), conflicts_(conflicts), divid_(new int[jobNumber]), size_(1)
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
    double distributionParameter2, double conflictParity) : jobNumber_(jobNumber), machineNumber_(machineNumber), size_(1)
    {
        for (int i = 2; i <= jobNumber; i++)
		{
			size_ *= i;
		}

		lengths_ = new double[jobNumber];
		divid_ = new int[jobNumber];
		conflicts_ = new bool*[jobNumber];

        NormalRandomNumberGenerator* nrnd;
        UniformRandomDoubleGenerator* urnd;
        UniformRandomDoubleGenerator rnd2(0, 1);

        if(dist == ProbabilityDistribution::Normal)
        {
            nrnd = new NormalRandomNumberGenerator(distributionparameter1, distributionParameter2);
        }
        else
        {
            urnd = new UniformRandomDoubleGenerator(distributionparameter1, distributionParameter2);
        }

        double p;

        for(int i = 0; i < jobNumber; i++)
        {
            do
            {
                lengths_[i] = (dist == ProbabilityDistribution::Normal) ? nrnd->generate() : urnd->generate();
            }
            while(lengths_[i] < 0);
            
            conflicts_[i] = new bool[jobNumber];
            divid_[i] = 1;
            conflicts_[i][i] = false;

            for(int j = 0; j < i; j++)
            {
                p = rnd2.generate();
                conflicts_[i][j] = p < conflictParity;
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

        if(dist == ProbabilityDistribution::Normal)
        {
            delete nrnd;
        }
        else
        {
            delete urnd;
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

	double* Problem::getLengths() const
    {
        return lengths_;
    }

    double Problem::getLength(int ind) const
    {
        return lengths_[ind];
    }

    bool** Problem::getConflicts() const
    {
        return conflicts_;
    }

	int* Problem::getDivid() const
    {
        return divid_;
    }

	__uint128_t Problem::getSize() const
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

    Problem::~Problem()
    {
        delete[] lengths_;

        for(int i = 0; i < jobNumber_; i++)
        {
            delete[] conflicts_[i];
        }

        delete[] conflicts_;
        delete[] divid_;
    }
}