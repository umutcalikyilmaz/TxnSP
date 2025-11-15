#include "TxnSP/Solvers/SASolver.h"

namespace TxnSP
{
	void SASolver::decrementExponential()
	{
		T_ *= decrementParameter_;		
	}

    void SASolver::decrementLinear()
	{
		T_ -= decrementParameter_;
	}

    void SASolver::decrementSlow()
	{
		T_ = T_ / (1 + decrementParameter_ * T_);
	}

    void SASolver::permute(int* list, int ind1, int ind2)
	{
		int temp = list[ind1];
		list[ind1] = list[ind2];
		list[ind2] = temp;
	}

    void SASolver::randomize(int size, int* ind1, int* ind2)
	{
		(*ind1) = rnd_->generate();

		do
		{
			(*ind2) = rnd_->generate();
		} 
		while ((*ind1) == (*ind2));
	}

	void SASolver::randomize(int* list, int size)
	{
		int ind1;
		int ind2;
		randomize(size, &ind1, &ind2);
		permute(list, ind1, ind2);
	}

    SolverOutput* SASolver::solve(const SolverInput& input)
    {
		double beg = std::chrono::steady_clock::now().time_since_epoch().count();
        Problem* prb = input.prb;

		if(prb->getJobNumber() <= prb->getMachineNumber())
        {
            double end = std::chrono::steady_clock::now().time_since_epoch().count();
            return new SolverOutput(prb, (end - beg) / 1000000000);
        }

        int n = prb->getJobNumber();
		int m = prb->getMachineNumber();		
		__uint128_t size = prb->getSize();

		switch (input.SA_DecrementType)
		{
		case TemperatureEvolution::Exponential:
			decrementPtr_ = &SASolver::decrementExponential;
			break;
		case TemperatureEvolution::Linear:
			decrementPtr_ = &SASolver::decrementLinear;
			break;
		case TemperatureEvolution::Slow:
			decrementPtr_ = &SASolver::decrementSlow;
			break;
		}

		SchedulePool schp(n, m, 2);
        int* state = new int[n];
        int* bestState = new int[n];
        prob_ = new UniformRandomDoubleGenerator(0, 1);
        rnd_ = new UniformRandomIntGenerator(0, n - 1);
		T_ = input.SA_MaxTemperature;
		decrementParameter_ = input.SA_DecrementParameter;

		int ind1;
		int ind2;
		int decrementType = static_cast<int>(input.SA_DecrementType);
		double cost;
		double tempCost;
		double bestCost;		

        for(int i = 0; i < n; i++)
        {
            state[i] = i;
        }

        for (int i = 0; i < 3 * n; i++)
		{
			randomize(state, n);
		}

        copy(state, bestState, n);

        Schedule* sch = schp.getSchedule(prb, state);
		cost = sch->getMakespan();
		bestCost = cost;

		while (T_ > 0.000001)
		{
			randomize(n, &ind1, &ind2);
			permute(state, ind1, ind2);
			schp.returnSchedule(sch);
			sch = schp.getSchedule(prb, state);
			tempCost = sch->getMakespan();

			if (tempCost < cost)
			{
				cost = tempCost;
				
				if(cost < bestCost)
				{
					bestCost = cost;
					copy(state, bestState, n);
				}				
			}
			else
			{
				double p = prob_->generate();
				double pp = exp((cost - tempCost) / T_);

				if (p < pp)
				{
					cost = tempCost;
				}
				else
				{
					permute(state, ind1, ind2);
				}
			}

			(this->*decrementPtr_)();
		}

		double end = std::chrono::steady_clock::now().time_since_epoch().count();
		SolverOutput* res = new SolverOutput(prb, bestState, (end - beg) / 1000000000);

		delete[] state;
        delete[] bestState;
        delete prob_;
        delete rnd_;

		return res;
    }
}