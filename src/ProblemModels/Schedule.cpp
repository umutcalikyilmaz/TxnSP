#include <ProblemModels/Schedule.h>

namespace TransactionScheduling
{
	Schedule::Schedule(Schedule* sch) : size(sch->size), n(sch->n), m(sch->m), makespan(sch->makespan), mintime(sch->mintime), minimumCore(sch->minimumCore), del(false)
	{
		cores = new int* [m];
		coreNums = new int[m];
		coreTimes = new double[m];
		lastJobs = new int[m];

		for (int i = 0; i < m; i++)
		{
			cores[i] = new int[n];
			coreNums[i] = sch->coreNums[i];
			coreTimes[i] = sch->coreTimes[i];
			lastJobs[i] = sch->lastJobs[i];

			for (int j = 0; j < n; j++)
			{
				cores[i][j] = sch->cores[i][j];
			}
		}
	}

	Schedule::Schedule(Problem* prb, int job) : n(prb->GetN()), m(prb->GetM()), size(1), makespan(prb->GetT(job)), minimumCore(1), mintime(0), del(false)
	{
		cores = new int*[m];
		coreTimes = new double[m];
		lastJobs = new int[m];
		coreNums = new int[m];

		for (int i = 0; i < m; i++)
		{
			cores[i] = new int[n];
			coreTimes[i] = 0;
			coreNums[i] = 0;

			for (int j = 0; j < n; j++)
			{
				cores[i][j] = -1;
			}
		}

		lastJobs[0] = job;
		cores[0][0] = job;
		coreTimes[0] = makespan;
		coreNums[0] = 1;
	}

	Schedule::Schedule(Problem* prb, Schedule* sch, int job) : n(prb->GetN()), m(prb->GetM()), size(sch->size), minimumCore(sch->minimumCore), makespan(sch->makespan), mintime(sch->mintime), del(false)
	{
		cores = new int*[m];
		coreNums = new int[m];
		coreTimes = new double[m];
		lastJobs = new int[m];

		for (int i = 0; i < m; i++)
		{
			cores[i] = new int[n];
			coreNums[i] = sch->coreNums[i];
			coreTimes[i] = sch->coreTimes[i];
			lastJobs[i] = sch->lastJobs[i];

			for (int j = 0; j < n; j++)
			{
				cores[i][j] = sch->cores[i][j];
			}
		}

		double temp = mintime;
		int coun = (size < m) ? size : m;
		bool** conf = prb->GetConf();

		for (int i = 0; i < coun; i++)
		{
			if (conf[job][lastJobs[i]])
			{
				if (temp < coreTimes[i])
				{
					temp = coreTimes[i];
				}
			}
		}

		size++;
		lastJobs[minimumCore] = job;
		cores[minimumCore][coreNums[minimumCore]] = job;
		coreNums[minimumCore]++;
		coreTimes[minimumCore] = temp + prb->GetT(job);
		mintime = 1.7976931e+308;

		if (coreTimes[minimumCore] > makespan)
		{
			makespan = coreTimes[minimumCore];
		}

		for (int i = 0; i < m; i++)
		{
			if (coreTimes[i] < mintime)
			{
				mintime = coreTimes[i];
				minimumCore = i;
			}
		}
	}

    Schedule::Schedule(Problem* prb, __uint128_t index, int* perm, int* a) : n(prb->GetN()), m(prb->GetM()), size(prb->GetN()), minimumCore(0), del(false)
    {
        Decode(index, perm, a, n, prb->GetDivid());
		cores = new int*[m]; 
		coreTimes = new double[m];
		lastJobs = new int[m]; 
		coreNums = new int[m]; 
        makespan = 0;
		mintime = 1.7976931e+308;

		for (int i = 0; i < m; i++)
		{
			cores[i] = new int[n];
			coreNums[i] = 0;
			coreTimes[i] = 0;

			for (int j = 0; j < n; j++) 
			{
				cores[i][j] = -1;
			}
		}

        bool** conf = prb->GetConf();

		for (int i = 0; i < m; i++)
		{
			double temp = 0;

			cores[i][0] = perm[i];
			lastJobs[i] = perm[i];
			coreNums[i]++;            		

			for (int j = 0; j <= i; j++)
			{
				if (conf[perm[i]][lastJobs[j]])
				{
					if (coreTimes[j] > temp)
					{
						temp = coreTimes[j];
					}
				}
			}

			coreTimes[i] = temp + prb->GetT(perm[i]);

			if (coreTimes[i] < mintime)
			{
				mintime = coreTimes[i];
				minimumCore = i;
			}
		}

		for (int i = m; i < n; i++)
		{
			double temp = coreTimes[minimumCore];

			cores[minimumCore][coreNums[minimumCore]] = perm[i];
			lastJobs[minimumCore] = perm[i];			
			coreNums[minimumCore]++;

			for (int j = 0; j < m; j++)
			{
				if (conf[perm[i]][lastJobs[j]])
				{
					if (coreTimes[j] > temp)
					{
						temp = coreTimes[j];
					}
				}
			}

			coreTimes[minimumCore] = temp + prb->GetT(perm[i]);
			mintime = 1.7976931e+308;

			for (int j = 0; j < m; j++)
			{
				if (coreTimes[j] < mintime)
				{
					mintime = coreTimes[j];
					minimumCore = j;
				}
			}
		}		

		for (int i = 0; i < m; i++)
		{
			if (coreTimes[i] > makespan)
			{
				makespan = coreTimes[i];
			}
		}
    }

	Schedule::Schedule(Problem* prb, int* state) : n(prb->GetN()), m(prb->GetM()), size(prb->GetN()), minimumCore(0), del(true)
	{		
		cores = new int*[m];
		coreTimes = new double[m];
		lastJobs = new int[m];
		coreNums = new int[m];
		order = new int[m];
		bool** conf = prb->GetConf();
		double* T = prb->GetT();
		int job;
		double temp;
		
		for (int i = 0; i < m; i++)
		{
			temp = 0;
			job = state[i];

			cores[i] = new int[n];
			cores[i][0] = job;
			lastJobs[i] = job;
			coreNums[i] = 1;
			
			for (int j = i - 1; j > -1; j--)
			{
				if (conf[job][lastJobs[order[j]]])
				{
					temp = coreTimes[order[j]];
					break;
				}
			}

			coreTimes[i] = temp + T[job];
			int ind = FindPlace2(coreTimes, order, coreTimes[i], i);
			Shift(order, ind, i);
			order[ind] = i;			
		}

		for (int i = m; i < n; i++)
		{
			temp = coreTimes[order[0]];
			job = state[i];

			cores[order[0]][coreNums[order[0]]] = job;
			lastJobs[order[0]] = job;
			coreNums[order[0]]++;
			
			for (int j = m - 1; j > 0; j--)
			{
				if (conf[job][lastJobs[order[j]]])
				{
					temp = coreTimes[order[j]];
					break;
				}
			}

			coreTimes[order[0]] = temp + T[job];
			int ind = FindPlace(coreTimes, order, coreTimes[order[0]], m);
			int tempi = order[0];
			Shift(order, ind);
			order[ind] = tempi;
		}

		mintime = coreTimes[order[0]];
		makespan = coreTimes[order[m - 1]];
	}

	void Schedule::Change(Schedule* sch)
	{	
		size = sch->size;
		makespan = sch->makespan;
		mintime = sch->mintime;
		minimumCore = sch->minimumCore;
			
		for (int i = 0; i < m; i++)
		{
			coreNums[i] = sch->coreNums[i];
			coreTimes[i] = sch->coreTimes[i];
			lastJobs[i] = sch->lastJobs[i];
			
			for (int j = 0; j < n; j++)
			{
				cores[i][j] = sch->cores[i][j];
			}
		}
	}

	void Schedule::Change(Problem* prb, int job)
	{
		size = 1;
		minimumCore = 1;
		mintime = 0;
		makespan = prb->GetT(job);

		for (int i = 0; i < m; i++)
		{
			coreTimes[i] = 0;
			coreNums[i] = 0;

			for (int j = 0; j < n; j++)
			{
				cores[i][j] = -1;
			}
		}

		lastJobs[0] = job;
		cores[0][0] = job;
		coreTimes[0] = makespan;		
		coreNums[0] = 1;
	}

	void Schedule::Change(Problem* prb, Schedule* sch, int job)
	{
		Change(sch);

		int ind = minimumCore;
		double temp = mintime;
		int coun = (size < m) ? size : m;
		bool** conf = prb->GetConf();

		for(int i = 0; i < coun; i++)
		{
			if(conf[job][lastJobs[i]])
			{
				if (temp < coreTimes[i])
				{
					temp = coreTimes[i];
				}
			}
		}

		size++;
		lastJobs[ind] = job;
		cores[ind][coreNums[ind]] = job;
		coreNums[ind]++;
		coreTimes[ind] = temp + prb->GetT(job);
		mintime = 1.7976931e+308;

		if (coreTimes[ind] > makespan)
		{
			makespan = coreTimes[ind];
		}

		for (int i = 0; i < m; i++)
		{
			if (coreTimes[i] < mintime)
			{
				mintime = coreTimes[i];
				minimumCore = i;
			}
		}	
	}

	void Schedule::Change(Problem* prb, __uint128_t index, int* perm, int* a)
	{
		size = prb->GetN();
		minimumCore = 0;
		Decode(index, perm, a, n, prb->GetDivid());
		mintime = 1.7976931e+308;

		for (int i = 0; i < m; i++)
		{
			coreNums[i] = 0;
			coreTimes[i] = 0;

			for (int j = 0; j < n; j++) 
			{
				cores[i][j] = -1;
			}
		}

		bool** conf = prb->GetConf();
		double* t = prb->GetT();

		for (int i = 0; i < m; i++)
		{
			double temp = 0;

			cores[i][0] = perm[i];
			lastJobs[i] = perm[i];
			coreNums[i]++;

			for (int j = 0; j <= i; j++)
			{
				if (conf[perm[i]][lastJobs[j]])
				{
					if (coreTimes[j] > temp)
					{
						temp = coreTimes[j];
					}
				}
			}

			coreTimes[i] = temp + t[perm[i]];

			if (coreTimes[i] < mintime)
			{
				mintime = coreTimes[i];
				minimumCore = i;
			}
		}

		for (int i = m; i < n; i++)
		{
			double temp = coreTimes[minimumCore];

			cores[minimumCore][coreNums[minimumCore]] = perm[i];
			lastJobs[minimumCore] = perm[i];
			coreNums[minimumCore]++;

			for (int j = 0; j < m; j++)
			{
				if (conf[perm[i]][lastJobs[j]])
				{
					if (coreTimes[j] > temp)
					{
						temp = coreTimes[j];
					}
				}
			}

			coreTimes[minimumCore] = temp + t[perm[i]];
			mintime = 1.7976931e+308;

			for (int j = 0; j < m; j++)
			{
				if (coreTimes[j] < mintime)
				{
					mintime = coreTimes[j];
					minimumCore = j;
				}
			}
		}

		makespan = 0;

		for (int i = 0; i < m; i++)
		{
			if (coreTimes[i] > makespan)
			{
				makespan = coreTimes[i];
			}
		}
	}

	void Schedule::Change(Problem* prb, int* state)
	{		
		bool** conf = prb->GetConf();
		double* T = prb->GetT();
		double temp;
		int job;

		for (int i = 0; i < m; i++)
		{
			temp = 0;
			job = state[i];

			cores[i][0] = job;
			lastJobs[i] = job;
			coreNums[i] = 1;
			
			for (int j = i - 1; j > -1; j--)
			{
				if (conf[job][lastJobs[order[j]]])
				{
					temp = coreTimes[order[j]];
					break;
				}
			}

			coreTimes[i] = temp + T[job];
			int ind = FindPlace2(coreTimes, order, coreTimes[i], i);
			Shift(order, ind, i);
			order[ind] = i;	
			
		}

		for (int i = m; i < n; i++)
		{			
			temp = coreTimes[order[0]];
			job = state[i];

			cores[order[0]][coreNums[order[0]]] = job;
			lastJobs[order[0]] = job;
			coreNums[order[0]]++;

			for (int j = m - 1; j > 0; j--)
			{
				if (conf[job][lastJobs[order[j]]])
				{
					temp = coreTimes[order[j]];
					break;
				}
			}

			coreTimes[order[0]] = temp + T[job];
			int tempi = order[0];
			int ind = FindPlace(coreTimes, order, coreTimes[order[0]], m);			
			Shift(order, ind);
			order[ind] = tempi;
			
		}

		makespan = coreTimes[order[m - 1]];
		mintime = coreTimes[order[0]];
	}

    int Schedule::GetN() const
    {
        return n;
    }

	int Schedule::GetM() const
    {
        return m;
    }

	int Schedule::GetSize() const
    {
        return size;
    }

	double Schedule::GetMakespan() const
    {
        return makespan;
    }

	double Schedule::GetMintime() const
    {
        return mintime;
    }

	int Schedule::GetMinimumCore() const
    {
        return minimumCore;
    }

	int** Schedule::GetJobs() const
    {
        return cores;
    }    

	double* Schedule::GetCoreTimes() const
    {
        return coreTimes;
    }

	int* Schedule::GetLastJobs() const
    {
        return lastJobs;
    }

	int* Schedule::GetCoreNums() const
    {
        return coreNums;
    }

	bool Schedule::IsEquivalent(Schedule* sch)
	{
		std::map<int,double> ptMap;
		
		for(int i = 0; i < size; i++)
		{
			ptMap[cores[i][0]] = coreTimes[i];
		}

		for(int i = 0; i < size; i++)
		{
			std::map<int,double>::iterator item = ptMap.find(sch->cores[i][0]);

			if(item == ptMap.end())
			{
				return false;
			}

			if(item->second - coreTimes[i] < 0.00001 && item->second - coreTimes[i] > -0.00001)
			{
				return false;
			}
		}

		return true;
	}

    Schedule::~Schedule()
    {
        for (int i = 0; i < m; i++)
		{
			delete[] cores[i];
		}

		delete[] cores;
		delete[] coreNums;
		delete[] coreTimes;
		delete[] lastJobs;

		if(del)
		{
			delete[] order;
		}
    }
}