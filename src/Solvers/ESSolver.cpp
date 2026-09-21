#include "TxnSP/Solvers/ESSolver.h"

namespace TxnSP
{
    SolverOutput ESSolver::solve(const SolverInput& input)
    {
		double beg = std::chrono::steady_clock::now().time_since_epoch().count();
        Problem* prb = input.prb;

		if(prb->getJobNumber() <= prb->getMachineNumber())
        {
            double end = std::chrono::steady_clock::now().time_since_epoch().count();
            return SolverOutput(prb, (end - beg) / 1000000000);
        }

        int n = prb->getJobNumber();
		int m = prb->getMachineNumber();		
		LargeInt size = prb->getSize();

		SchedulePool schp(n, m);
		std::vector<int> perm(n);
		std::vector<int> a(n);
		std::unique_ptr<Schedule> ressch = schp.getSchedule(prb, 0, perm, a);
		std::unique_ptr<Schedule> temp = move(schp.getSchedule(ressch.get()));
		double makespan = ressch->getMakespan();

		for(LargeInt i = 1; i < size; i++)
		{
			schp.returnSchedule(move(temp));
			temp = schp.getSchedule(prb, i, perm, a);

			if (temp->getMakespan() < makespan)
			{
				makespan = temp->getMakespan();
				schp.returnSchedule(move(ressch));
				ressch = schp.getSchedule(temp.get());
			}
		}

		double end = std::chrono::steady_clock::now().time_since_epoch().count();

		return SolverOutput(prb, ressch.get(), (end - beg) / 1000000000);
    }
}