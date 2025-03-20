#include <Solvers/DPSolver.h>

namespace TransactionScheduling
{
    SolverOutput* DPSolver::SolveExact(Problem* prb)
    {
        double beg = std::chrono::steady_clock::now().time_since_epoch().count();

        //map<int, Subset*> ssMaps[2];
        Subset** subsets[2];
		subsets[0] = new Subset*[ssNum];
		subsets[1] = new Subset*[ssNum];
        bool* blist = new bool[n];

		for (int i = 0; i < n; i++)
		{            	
			Schedule** slist = new Schedule*[1];
			slist[0] = schp->GetSchedule(prb, i);
            subsets[0][i] = sp->GetSubset(n, 1, 1, slist);
		}

        int maxSch = 1;
        int maxNum;
		int oind;
		int nind;
		int preNum;
		int curNum = n;        

        for(int i = 2; i <= n; i++)
        {            
            maxNum = maxSch * i;
			maxSch = 0;
			preNum = curNum;
			curNum = Combination(n, i);
			oind = i % 2;
			nind = 1 - oind;

            if(i == 3)
            {
                int asd = 0;
            }

            for (int j = 0; j < curNum; j++)
            {
                Decode(blist, j, n, i);
                double minimax = 1.7976931e+308;
				int schCount = 0;
				Schedule** slist = new Schedule*[maxNum];

                for(int k = 0; k < n; k++)
                {
                    if(blist[k])
                    {
                        blist[k] = false;
                        int ind = Encode(blist, n, i - 1);
                        blist[k] = true;
                        Subset* currentSS = subsets[oind][ind];

                        for(int l = 0; l < currentSS->GetScheduleNum(); l++)
                        {
                            Schedule* temp = schp->GetSchedule(prb, currentSS->GetSchedule(l), k);

                            if(temp->GetMakespan() < minimax)
							{
								minimax = temp->GetMakespan();
								slist[schCount] = temp;
								schCount++;
							}
							else if(temp->GetMintime() < minimax)
							{
								slist[schCount] = temp;
								schCount++;
							}
							else
							{
								schp->ReturnSchedule(temp);
							}
                        }
                    }
                }

                subsets[nind][j] = sp->GetSubset(n, i, schCount, slist, minimax);

				if (schCount > maxSch)
				{
					maxSch = schCount;
				}
            }

            for (int x = 0; x < preNum; x++)
			{
				sp->ReturnSubset(subsets[oind][x]);
			}
        }

        double end = std::chrono::steady_clock::now().time_since_epoch().count();
        SolverOutput* res = new SolverOutput(prb, subsets[nind][0]->GetSchedule(0), (end - beg) / 1000000000);
		sp->ReturnSubset(subsets[nind][0]);

		delete[] blist;
        delete[] subsets[0];
        delete[] subsets[1];

		return res;
    }

	SolverOutput* DPSolver::SolveApproximate(Problem* prb)
    {
        double beg = std::chrono::steady_clock::now().time_since_epoch().count();  

        //map<int, Subset*> ssMaps[2];
        bool* blist = new bool[n];
        Subset** subsets[2];
		subsets[0] = new Subset*[ssNum];
		subsets[1] = new Subset*[ssNum];

        for(int i = 0; i < n; i++)
        {
            blist[i] = false;
        }

		for (int i = 0; i < n; i++)
		{            	
			Schedule** slist = new Schedule*[1];
			slist[0] = schp->GetSchedule(prb, i);
            subsets[0][i] = sp->GetSubset(n, 1, 1, slist);
		}

        int maxSch = 1;
        int maxNum;
		int oind;
		int nind;
		int preNum;
		int curNum = n;

        for(int i = 2; i <= n; i++)
        {
			preNum = curNum;
			curNum = Combination(n, i);
			oind = i % 2;
			nind = 1 - oind;

            for (int j = 0; j < curNum; j++)
            {
                Decode(blist, j, n, i);
                double minimax = 1.7976931e+308;
				bool full = false;
				Schedule** slist = new Schedule*[1];

                for(int k = 0; k < n; k++)
                {
                    if(blist[k])
                    {
                        blist[k] = false;
                        int ind = Encode(blist, n, i - 1);                        
                        blist[k] = true;
                        
                        Subset* currentSS = subsets[oind][ind];
                        Schedule* temp = schp->GetSchedule(prb, currentSS->GetSchedule(0), k);

                        if (temp->GetMakespan() < minimax)
						{
							if (full)
							{
								schp->ReturnSchedule(*slist);
							}

							full = true;
							*slist = temp;
							minimax = temp->GetMakespan();
						}
						else
						{
							schp->ReturnSchedule(temp);
						}                        
                    }
                }

                subsets[nind][j] = sp->GetSubset(n, i, 1, slist);                
            }

            for (int x = 0; x < preNum; x++)
			{
				sp->ReturnSubset(subsets[oind][x]);
			}           
        }

        double end = std::chrono::steady_clock::now().time_since_epoch().count();
        SolverOutput* res = new SolverOutput(prb, subsets[nind][0]->GetSchedule(0), (end - beg) / 1000000000);
		sp->ReturnSubset(subsets[nind][0]);

		delete[] blist;
        delete[] subsets[0];
        delete[] subsets[1];

		return res;
    }

    DPSolver::DPSolver() : n(0), m(0), init(false) { }

    SolverOutput* DPSolver::Solve(const SolverInput& input)
    {
        if(input.prb->GetN() != n || input.prb->GetM() != m || input.DP_SolutionType != type)
        {
            n = input.prb->GetN();
            m = input.prb->GetM();
            type = input.DP_SolutionType;
            ssNum = Combination(n, n / 2);
            schNum = type == Approximate ? 2.5 * ssNum : log(n) * pow(n * ssNum, 2) / Factorial(m - 1);

            if(init)
            {                
                delete sp;
                delete schp;
            }

            schp = new SchedulePool(n, m, schNum);
            sp = new SubsetPool(n, ssNum, schp);
            init = true;
        }

        return type == Approximate ? SolveApproximate(input.prb) : SolveExact(input.prb);
    }

    DPSolver::~DPSolver()
    {
        if(init)
        {
            delete schp;
            delete sp;
        }
    }
}
/*
namespace TransactionScheduling
{
    SolverOutput* DPSolver::SolveExact(Problem* prb, bool bubble, double berror)
    {
        double beg = std::chrono::steady_clock::now().time_since_epoch().count();

        map<int, Subset*> ssMaps[2];
        bool* blist = new bool[n];

        for(int i = 0; i < n; i++)
        {
            blist[i] = false;
        }

		for (int i = 0; i < n; i++)
		{            	
			Schedule** slist = new Schedule*[1];
			slist[0] = schp->GetSchedule(prb, i);
            blist[i] = true;
            ssMaps[0][Encode(blist, n)] = sp->GetSubset(n, 1, 1, slist);
            blist[i] = false;
		}

        int maxSch = 1;
        int maxNum;
		int oind;
		int nind;
		int preNum;
		int curNum = n;        

        for(int i = 2; i <= n; i++)
        {            
            maxNum = maxSch * i;
			maxSch = 0;
			preNum = curNum;
			curNum = Combination(n, i);
			oind = i % 2;
			nind = 1 - oind;

            if(i == 3)
            {
                int asd = 0;
            }

            for (int j = 0; j < curNum; j++)
            {
                Decode(blist, j, n, i);
                double minimax = 1.7976931e+308;
				int schCount = 0;
				Schedule** slist = new Schedule*[maxNum];

                for(int k = 0; k < n; k++)
                {
                    if(blist[k])
                    {
                        blist[k] = false;
                        Subset* currentSS = ssMaps[oind][Encode(blist, n)];
                        blist[k] = true;                        

                        for(int l = 0; l < currentSS->GetScheduleNum(); l++)
                        {
                            Schedule* temp = schp->GetSchedule(prb, currentSS->GetSchedule(l), k);

                            if(temp->GetMakespan() < minimax)
							{
								minimax = temp->GetMakespan();
								slist[schCount] = temp;
								schCount++;
							}
							else if(temp->GetMintime() < minimax)
							{
								slist[schCount] = temp;
								schCount++;
							}
							else
							{
								schp->ReturnSchedule(temp);
							}
                        }
                    }
                }

                ssMaps[nind][Encode(blist, n)] = sp->GetSubset(n, i, schCount, slist, minimax);

				if (schCount > maxSch)
				{
					maxSch = schCount;
				}
            }

            for (auto const& [key, val] : ssMaps[oind])
            {
                sp->ReturnSubset(val);
            }

            ssMaps[oind].clear();
        }

        double end = std::chrono::steady_clock::now().time_since_epoch().count();

        int ind = (int)(pow(2, n) - 1);
        SolverOutput* res = new SolverOutput(prb, ssMaps[nind][ind]->GetSchedule(0), bubble, berror, (end - beg) / 1000000000);
		sp->ReturnSubset(ssMaps[nind][ind]);

		delete[] blist;

		return res;
    }

	SolverOutput* DPSolver::SolveApproximate(Problem* prb, bool bubble, double berror)
    {
        double beg = std::chrono::steady_clock::now().time_since_epoch().count();  

        map<int, Subset*> ssMaps[2];
        bool* blist = new bool[n];

        for(int i = 0; i < n; i++)
        {
            blist[i] = false;
        }

		for (int i = 0; i < n; i++)
		{            	
			Schedule** slist = new Schedule*[1];
			slist[0] = schp->GetSchedule(prb, i);
            blist[i] = true;
            ssMaps[0][Encode(blist, n)] = sp->GetSubset(n, 1, 1, slist);
            blist[i] = false;
		}

        int maxSch = 1;
        int maxNum;
		int oind;
		int nind;
		int preNum;
		int curNum = n;

        for(int i = 2; i <= n; i++)
        {
			preNum = curNum;
			curNum = Combination(n, i);
			oind = i % 2;
			nind = 1 - oind;

            for (int j = 0; j < curNum; j++)
            {
                Decode(blist, j, n, i);
                double minimax = 1.7976931e+308;
				bool full = false;
				Schedule** slist = new Schedule*[1];

                for(int k = 0; k < n; k++)
                {
                    if(blist[k])
                    {
                        blist[k] = false;
                        int dede = Encode(blist, n);
                        Subset* currentSS = ssMaps[oind][Encode(blist, n)];
                        blist[k] = true;
                        
                        Schedule* temp = schp->GetSchedule(prb, currentSS->GetSchedule(0), k);

                        if (temp->GetMakespan() < minimax)
						{
							if (full)
							{
								schp->ReturnSchedule(*slist);
							}

							full = true;
							*slist = temp;
							minimax = temp->GetMakespan();
						}
						else
						{
							schp->ReturnSchedule(temp);
						}                        
                    }
                }

                ssMaps[nind][Encode(blist, n)] = sp->GetSubset(n, i, 1, slist);                
            }

            for (auto const& [key, val] : ssMaps[oind])
            {
                sp->ReturnSubset(val);
            }

            ssMaps[oind].clear();            
        }

        double end = std::chrono::steady_clock::now().time_since_epoch().count();

        int ind = (int)(pow(2, n) - 1);
        SolverOutput* res = new SolverOutput(prb, ssMaps[nind][ind]->GetSchedule(0), bubble, berror, (end - beg) / 1000000000);
		sp->ReturnSubset(ssMaps[nind][ind]);

		delete[] blist;

		return res;
    }

    DPSolver::DPSolver() : n(0), m(0), init(false) { }

    SolverOutput* DPSolver::Solve(const SolverInput& input)
    {
        if(input.prb->GetN() != n || input.prb->GetM() != m || input.DP_SolutionType != type)
        {
            n = input.prb->GetN();
            m = input.prb->GetM();
            type = input.DP_SolutionType;
            ssNum = Combination(n, n / 2);
            schNum = type == Approximate ? 2.5 * ssNum : log(n) * pow(n * ssNum, 2) / Factorial(m - 1);

            if(init)
            {
                delete schp;
                delete sp;
            }

            schp = new SchedulePool(n, m, schNum);
            sp = new SubsetPool(n, ssNum, schp);
            init = true;
        }

        return type == Approximate ? SolveApproximate(input.prb, input.bubble, input.berror) : SolveExact(input.prb, input.bubble, input.berror);
    }

    DPSolver::~DPSolver()
    {
        if(init)
        {
            delete schp;
            delete sp;
        }
    }
}
*/