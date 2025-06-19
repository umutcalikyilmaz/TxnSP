#include "TxnSP/Structs/SolverOutput.h"

namespace TxnSP
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

	SolverOutput::SolverOutput(Problem* prb, double runtime)
	{
		jobNumber = prb->getJobNumber();
		machineNumber = prb->getMachineNumber();
		makespan = 0;
		minimumTime = DBL_MAX;
		jobs = vector<vector<int>>(machineNumber);
		processingTimes = vector<double>(machineNumber);
		startingTimes = vector<double>(jobNumber);
		endingTimes = vector<double>(jobNumber);
		assignments = vector<int>(jobNumber);
		conflicts = vector<vector<int>>(jobNumber);
		double* t = prb->getLengths();
		bool** conf = prb->getConflicts();

		for(int i = 0; i < jobNumber; i++)
		{
			jobs[i].push_back(i);
			double temp = 0;

			for(int j = 0; j < i; j++)
			{
				if(conf[i][j] && temp < processingTimes[j])
				{
					temp = processingTimes[j];
				}
			}

			startingTimes.push_back(temp);
			endingTimes.push_back(temp + t[i]);
			processingTimes.push_back(endingTimes[i]);
			assignments.push_back(i);
			
			if(makespan < processingTimes[i])
			{
				makespan = processingTimes[i];
			}

			if(minimumTime > processingTimes[i])
			{
				minimumTime = processingTimes[i];
			}
		}

		for(int i = 0; i < jobNumber; i++)
		{
			for(int j = 0; j < jobNumber; j++)
			{
				if(conf[i][j] && startingTimes[j] < startingTimes[i])
				{
					conflicts[i].push_back(j);
				}
			}
		}
	}

	SolverOutput::SolverOutput(Problem* prb, int* state, double runtime)
	{
		jobNumber = prb->getJobNumber();
		machineNumber = prb->getMachineNumber();        
        jobs = vector<vector<int>>(machineNumber);
		processingTimes = vector<double>(machineNumber);
		startingTimes = vector<double>(jobNumber);
		endingTimes = vector<double>(jobNumber);
		assignments = vector<int>(jobNumber);
		conflicts = vector<vector<int>>(jobNumber);
		double* T = prb->getLengths();
        bool** conf = prb->getConflicts();
		int* coreNums = new int[machineNumber];
		int* lastJobs = new int[machineNumber];
		int* order = new int[machineNumber];

		double temp;
		int job;

		for(int i = 0; i < jobNumber; i++)
		{
			startingTimes.push_back(0);
			endingTimes.push_back(0);
		}

		for (int i = 0; i < machineNumber; i++)
		{
			temp = 0;
			job = state[i];
			jobs[i].push_back(job);
			lastJobs[i] = job;
			coreNums[i] = 1;
			
			for (int j = i - 1; j > -1; j--)
			{
				if (conf[job][lastJobs[order[j]]])
				{
					temp = processingTimes[order[j]];
					break;
				}
			}

			startingTimes[job] = temp;
			endingTimes[job] = temp + T[job];
			processingTimes.push_back(endingTimes[job]);
			int ind = findPlace2(processingTimes, order, processingTimes[i], i);
			shift(order, ind, i);
			order[ind] = i;
		}

		for(int i = machineNumber; i < jobNumber; i++)
		{
			temp = processingTimes[order[0]];
			job = state[i];

			jobs[order[0]].push_back(job);
			lastJobs[order[0]] = job;
			coreNums[order[0]]++;

			for (int j = machineNumber - 1; j > 0; j--)
			{
				if (conf[job][lastJobs[order[j]]])
				{
					temp = processingTimes[order[j]];
					break;
				}
			}

			startingTimes[job] = temp;
			endingTimes[job] = temp + T[job];
			processingTimes[order[0]] = endingTimes[job];
			int ind = findPlace(processingTimes, order, processingTimes[order[0]], machineNumber);
			int tempi = order[0];
			shift(order, ind);
			order[ind] = tempi;
		}

		minimumTime = processingTimes[order[0]];
		makespan = processingTimes[order[machineNumber - 1]];

		for(int i = 0; i < jobNumber; i++)
		{
			for(int j = 0; j < jobNumber; j++)
			{
				if(conf[i][j] && startingTimes[j] < startingTimes[i])
				{
					conflicts[i].push_back(j);
				}
			}
		}
	}
	
    SolverOutput::SolverOutput(Problem* prb, Schedule* sch, double runtime) : runtime(runtime)
    {
        jobNumber = sch->getJobNumber();
		machineNumber = sch->getMachineNumber();
        makespan = sch->getMakespan();
		minimumTime = sch->getMinimumTime();
        jobs = vector<vector<int>>(machineNumber);
		processingTimes = vector<double>(machineNumber);
		startingTimes = vector<double>(jobNumber);
		endingTimes = vector<double>(jobNumber);
		assignments = vector<int>(jobNumber);
		conflicts = vector<vector<int>>(jobNumber);
        int* coreCount = new int[machineNumber];
		int* tempLast = new int[machineNumber];
        int* coreNums = sch->getJobNumbers();
        //double* ccoreTimes = sch->getProcessingTimes();
        int** schjobs = sch->getJobs();
        bool** conf = prb->getConflicts();
		double* T = prb->getLengths();
		double temp;	
		int job;

		for(int i = 0; i < machineNumber; i++)
		{
			for(int j = 0; j < coreNums[i]; j++)
			{
				jobs[i].push_back(schjobs[i][j]);
			}

			job = schjobs[i][0];
			temp = 0;

			for(int j = 0; j < i; j++)
			{
				if(conf[job][tempLast[j]] && temp < processingTimes[j])
				{
					temp = processingTimes[j];
				}
			}

			startingTimes[job] = temp;
			endingTimes[job] = temp + T[job];
			processingTimes[i] = endingTimes[job];
			assignments[job] = i;
			tempLast[i] = job;
			coreCount[i] = 1;
		}

		int minCore = 0;
		temp = 1.7976931e+308;

		for(int i = 0; i < machineNumber; i++)
		{
			if(processingTimes[i] < temp)
			{
				minCore = i;
				temp = processingTimes[i];
			}
		}

		for(int i = machineNumber; i < jobNumber; i++)
		{			
			job = schjobs[minCore][coreCount[minCore]];
			temp = processingTimes[minCore];

			for(int j = 0; j < machineNumber; j++)
			{
				if(conf[job][tempLast[j]] && temp < processingTimes[j])
				{
					temp = processingTimes[j];
				}
			}

			startingTimes[job] = temp;
			endingTimes[job] = temp + T[job];
			processingTimes[minCore] = endingTimes[job];
			assignments[job] = minCore;
			tempLast[minCore] = job;
			coreCount[minCore]++;
			

			minCore = 0;
			temp = 1.7976931e+308;

			for(int j = 0; j < machineNumber; j++)
			{
				if(processingTimes[j] < temp)
				{
					minCore = j;
					temp = processingTimes[j];
				}
			}
		}

        delete[] coreCount;
		delete[] tempLast;

        for(int i = 0; i < jobNumber; i++)
		{
			for(int j = 0; j < jobNumber; j++)
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
		jobNumber = prb->getJobNumber();
        machineNumber = prb->getMachineNumber();
        jobs = vector<vector<int>>(machineNumber);
		processingTimes = vector<double>(machineNumber);
		startingTimes = vector<double>(jobNumber);
		endingTimes = vector<double>(jobNumber);
		assignments = vector<int>(jobNumber);
		conflicts = vector<vector<int>>(jobNumber);
		double* T = prb->getLengths();
        bool** conf = prb->getConflicts();

		double* scpy = new double[jobNumber];
		int* ind = new int[jobNumber];
		bool* smt = new bool[jobNumber];
        int* lastJobs = new int[machineNumber];
        int minimumCore = 0;

		for (int i = 0; i < jobNumber; i++)
		{
			scpy[i] = s[i];
			smt[i] = true;
            startingTimes[i] = s[i];
            assignments[i] = x[i];
		}

		std::qsort(scpy, jobNumber, sizeof(double), Comp);

		for (int i = 0; i < jobNumber; i++)
		{	
			for (int j = 0; j < jobNumber; j++)
			{
				if (scpy[i] - s[j] < 0.0000001 && scpy[i] - s[j] > -0.0000001 && smt[j])
				{
					ind[i] = j;
					smt[j] = false;
					break;
				}
			}
		}

		for (int i = 0; i < jobNumber; i++)
		{
			jobs[x[ind[i]]].push_back(ind[i]);

			for(int j = 0; j < jobNumber; j++)
			{
				if(conf[i][j])
				{
					conflicts[i].push_back(j);
				}
			}
		}

		makespan = 0;
		minimumTime = 1.79769310e+308;

		for (int i = 0; i < machineNumber; i++)
		{
			if (jobs[i].size() > 0)
			{
				lastJobs[i] = jobs[i].back();
				processingTimes[i] = s[lastJobs[i]] + T[lastJobs[i]];
			}
			else
			{
				lastJobs[i] = -1;
				processingTimes[i] = 0;
			}

			if (processingTimes[i] <= minimumTime)
			{
				minimumTime = processingTimes[i];
				minimumCore = i;
			}

			if (processingTimes[i] > makespan)
			{
				makespan = processingTimes[i];
			}
		}

		delete[] scpy;
		delete[] ind;
		delete[] smt;
		delete[] lastJobs;
	}

	SolverOutput::~SolverOutput() { }
}