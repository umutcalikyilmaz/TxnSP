#include <Solvers/SASolver.h>

namespace TransactionScheduling
{
	void SASolver::DecrementExponential()
	{
		T *= decrementParameter;		
	}

    void SASolver::DecrementLinear()
	{
		T -= decrementParameter;
	}

    void SASolver::DecrementSlow()
	{
		T = T / (1 + decrementParameter * T);
	}

    void SASolver::Permute(int* list, int ind1, int ind2)
	{
		int temp = list[ind1];
		list[ind1] = list[ind2];
		list[ind2] = temp;
	}

    void SASolver::Randomize(int size, int* ind1, int* ind2)
	{
		(*ind1) = rnd->Generate();

		do
		{
			(*ind2) = rnd->Generate();
		} 
		while ((*ind1) == (*ind2));
	}

	void SASolver::Randomize(int* list, int size)
	{
		int ind1;
		int ind2;
		Randomize(size, &ind1, &ind2);
		Permute(list, ind1, ind2);
	}

    SolverOutput* SASolver::Solve(const SolverInput& input)
    {
		double beg = std::chrono::steady_clock::now().time_since_epoch().count();

        Problem* prb = input.prb;
        int n = prb->GetN();
		int m = prb->GetM();		
		__uint128_t size = prb->GetSize();

		switch (input.SA_DecrementType)
		{
		case Exponential:
			decrementPtr = &SASolver::DecrementExponential;
			break;
		case Linear:
			decrementPtr = &SASolver::DecrementLinear;
			break;
		case Slow:
			decrementPtr = &SASolver::DecrementSlow;
			break;
		}

		SchedulePool schp(n, m, 2);
        int* state = new int[n];
        int* bestState = new int[n];
        prob = new UniformRandomDoubleGenerator(0, 1);
        rnd = new UniformRandomIntGenerator(0, n - 1);
		T = input.SA_MaxTemperature;
		decrementParameter = input.SA_DecrementParameter;

		int ind1;
		int ind2;
		int decrementType = input.SA_DecrementType;
		double cost;
		double tempCost;
		double bestCost;		

        for(int i = 0; i < n; i++)
        {
            state[i] = i;
        }

        for (int i = 0; i < 3 * n; i++)
		{
			Randomize(state, n);
		}

        Copy(state, bestState, n);

        Schedule* sch = schp.GetSchedule(prb, state);
		cost = sch->GetMakespan();
		bestCost = cost;

		while (T > 0.000001)
		{
			Randomize(n, &ind1, &ind2);
			Permute(state, ind1, ind2);
			schp.ReturnSchedule(sch);
			sch = schp.GetSchedule(prb, state);
			tempCost = sch->GetMakespan();

			if (tempCost < cost)
			{
				cost = tempCost;
				
				if(cost < bestCost)
				{
					bestCost = cost;
					Copy(state, bestState, n);
				}				
			}
			else
			{
				double p = prob->Generate();
				double pp = exp((cost - tempCost) / T);

				if (p < pp)
				{
					cost = tempCost;
				}
				else
				{
					Permute(state, ind1, ind2);
				}
			}

			(this->*decrementPtr)();
		}

		double end = std::chrono::steady_clock::now().time_since_epoch().count();

		Schedule* bestSch = schp.GetSchedule(prb, bestState);
		SolverOutput* res = new SolverOutput(prb, bestSch, (end - beg) / 1000000000);
		schp.ReturnSchedule(bestSch);

		delete[] state;
        delete[] bestState;
        delete prob;
        delete rnd;

		return res;
    }
}