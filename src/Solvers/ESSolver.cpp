#include "TxnSP/Solvers/ESSolver.h"

namespace TxnSP
{
    SolverOutput* ESSolver::solve(const SolverInput& input)
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

		SchedulePool schp(n, m, 2);
		int* perm = new int[n];
		int* a = new int[n];
		Schedule* ressch = schp.getSchedule(prb, 0, perm, a);
		Schedule* temp = schp.getSchedule(ressch);
		double makespan = ressch->getMakespan();

		for(__uint128_t i = 1; i < size; i++)
		{
			schp.returnSchedule(temp);
			temp = schp.getSchedule(prb, i, perm, a);

			if (temp->getMakespan() < makespan)
			{
				makespan = temp->getMakespan();
				schp.returnSchedule(ressch);
				ressch = schp.getSchedule(temp);
			}
		}

		double end = std::chrono::steady_clock::now().time_since_epoch().count();

		SolverOutput* res = new SolverOutput(prb, ressch, (end - beg) / 1000000000);
		return res;
    }
}