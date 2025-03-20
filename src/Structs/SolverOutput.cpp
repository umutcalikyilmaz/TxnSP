#include <Structs/SolverOutput.h>

namespace TransactionScheduling
{
	int Comp(const void* a, const void* b)
	{
		if ( *(double*)a < *(double*)b ) 
		{
			return -1;
		}	
		else if ( *(double*)a > *(double*)b )
		{
			return 1;
		} 
		else
		{
			return 0;
		} 
	}

    SolverOutput::SolverOutput(Problem* prb, Schedule* sch, double runtime) : runtime(runtime)
    {
        n = sch->GetN();
		m = sch->GetM();
        makespan = sch->GetMakespan();
		mintime = sch->GetMintime();
        cores = vector<vector<int>>(m);
		coreTimes = vector<double>(m);
		startingTimes = vector<double>(n);
		endingTimes = vector<double>(n);
		assignments = vector<int>(n);
		conflicts = vector<vector<int>>(n);
        int* coreCount = new int[m];
		int* tempLast = new int[m];
        int* coreNums = sch->GetCoreNums();
        double* ccoreTimes = sch->GetCoreTimes();
        int** jobs = sch->GetJobs();
        bool** conf = prb->GetConf();
		double* T = prb->GetT();
		double temp;	
		int job;

		//std::cout << "Begin\n";

		for(int i = 0; i < m; i++)
		{
			for(int j = 0; j < coreNums[i]; j++)
			{
				cores[i].push_back(jobs[i][j]);
			}

			job = jobs[i][0];
			temp = 0;

			for(int j = 0; j < i; j++)
			{
				if(conf[job][tempLast[j]] && temp < coreTimes[j])
				{
					temp = coreTimes[j];
				}
			}

			startingTimes[job] = temp;
			endingTimes[job] = temp + T[job];
			coreTimes[i] = endingTimes[job];
			assignments[job] = i;
			tempLast[i] = job;
			coreCount[i] = 1;
			/*
			std::cout << "job:" << to_string(i) << "\n";
			std::cout << coreTimes[0] << "\n";
			std::cout << coreTimes[1] << "\n";
			std::cout << coreTimes[2] << "\n";
			*/
		}

		int minCore = 0;
		temp = 1.7976931e+308;

		for(int i = 0; i < m; i++)
		{
			if(coreTimes[i] < temp)
			{
				minCore = i;
				temp = coreTimes[i];
			}
		}

		for(int i = m; i < n; i++)
		{
			int asd = coreCount[minCore];
			int asd0 = coreTimes[0];
			int asd1 = coreTimes[1];
			int asd2 = coreTimes[2];
			
			job = jobs[minCore][coreCount[minCore]];
			temp = coreTimes[minCore];

			for(int j = 0; j < m; j++)
			{
				if(conf[job][tempLast[j]] && temp < coreTimes[j])
				{
					temp = coreTimes[j];
				}
			}

			startingTimes[job] = temp;
			endingTimes[job] = temp + T[job];
			coreTimes[minCore] = endingTimes[job];
			assignments[job] = minCore;
			tempLast[minCore] = job;
			coreCount[minCore]++;
			/*
			std::cout << "job:" << to_string(i) << "\n";
			std::cout << coreTimes[0] << "\n";
			std::cout << coreTimes[1] << "\n";
			std::cout << coreTimes[2] << "\n";
			*/
			

			minCore = 0;
			temp = 1.7976931e+308;

			for(int j = 0; j < m; j++)
			{
				if(coreTimes[j] < temp)
				{
					minCore = j;
					temp = coreTimes[j];
				}
			}
		}

        delete[] coreCount;
		delete[] tempLast;

        for(int i = 0; i < n; i++)
		{
			for(int j = 0; j < n; j++)
			{
				if(conf[i][j] && startingTimes[j] < startingTimes[i])
				{
					conflicts[i].push_back(j);
				}
			}
		}
    }

	SolverOutput::SolverOutput(Problem* prb, int* x, double* s, double runtime) : runtime(runtime)
	{
		n = prb->GetN();
        m = prb->GetM();
        cores = vector<vector<int>>(m);
		coreTimes = vector<double>(m);
		startingTimes = vector<double>(n);
		endingTimes = vector<double>(n);
		assignments = vector<int>(n);
		conflicts = vector<vector<int>>(n);
		double* T = prb->GetT();
        bool** conf = prb->GetConf();

		double* scpy = new double[n];
		int* ind = new int[n];
		bool* smt = new bool[n];
        int* lastJobs = new int[m];
        int minimumCore = 0;

		for (int i = 0; i < n; i++)
		{
			scpy[i] = s[i];
			smt[i] = true;
            startingTimes[i] = s[i];
            assignments[i] = x[i];
		}

		std::qsort(scpy, n, sizeof(double), Comp);

		for (int i = 0; i < n; i++)
		{	
			for (int j = 0; j < n; j++)
			{
				if (scpy[i] - s[j] < 0.0000001 && scpy[i] - s[j] > -0.0000001 && smt[j])
				{
					ind[i] = j;
					smt[j] = false;
					break;
				}
			}
		}

		for (int i = 0; i < n; i++)
		{
			cores[x[ind[i]]].push_back(ind[i]);

			for(int j = 0; j < n; j++)
			{
				if(conf[i][j])
				{
					conflicts[i].push_back(j);
				}
			}
		}

		makespan = 0;
		mintime = 1.79769310e+308;

		for (int i = 0; i < m; i++)
		{
			if (cores[i].size() > 0)
			{
				lastJobs[i] = cores[i].back();
				coreTimes[i] = s[lastJobs[i]] + T[lastJobs[i]];
			}
			else
			{
				lastJobs[i] = -1;
				coreTimes[i] = 0;
			}

			if (coreTimes[i] <= mintime)
			{
				mintime = coreTimes[i];
				minimumCore = i;
			}

			if (coreTimes[i] > makespan)
			{
				makespan = coreTimes[i];
			}
		}

		delete[] scpy;
		delete[] ind;
		delete[] smt;
		delete[] lastJobs;
	}

	SolverOutput::~SolverOutput() { }
}