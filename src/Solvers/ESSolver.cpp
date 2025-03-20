#include <Solvers/ESSolver.h>

namespace TransactionScheduling
{
    SolverOutput* ESSolver::Solve(const SolverInput& input)
    {
		double beg = std::chrono::steady_clock::now().time_since_epoch().count();

        Problem* prb = input.prb;
        int n = prb->GetN();
		int m = prb->GetM();		
		__uint128_t size = prb->GetSize();

		SchedulePool schp(n, m, 2);
		int* perm = new int[n];
		int* a = new int[n];
		Schedule* ressch = schp.GetSchedule(prb, 0, perm, a);
		Schedule* temp = schp.GetSchedule(ressch);
		double makespan = ressch->GetMakespan();

		for(__uint128_t i = 1; i < size; i++)
		{
			schp.ReturnSchedule(temp);
			temp = schp.GetSchedule(prb, i, perm, a);

			if (temp->GetMakespan() < makespan)
			{
				makespan = temp->GetMakespan();
				schp.ReturnSchedule(ressch);
				ressch = schp.GetSchedule(temp);
			}
		}

		double end = std::chrono::steady_clock::now().time_since_epoch().count();

		SolverOutput* res = new SolverOutput(prb, ressch, (end - beg) / 1000000000);
		return res;
    }
}