#include "TxnSP/Structs/SolverOutput.h"

namespace TxnSP
{
	SolverOutput::SolverOutput(Problem* prb, double runtime)
	{
		jobNumber = prb->getJobNumber();
		machineNumber = prb->getMachineNumber();
		makespan = 0;
		minimumTime = DBL_MAX;
		jobs = std::vector<std::vector<int>>(machineNumber);
		processingTimes = std::vector<double>(machineNumber);
		startingTimes = std::vector<double>(jobNumber);
		endingTimes = std::vector<double>(jobNumber);
		assignments = std::vector<int>(jobNumber);
		conflicts = std::vector<std::vector<int>>(jobNumber);
		auto t = prb->getLengths();
		auto conf = prb->getConflicts();

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

			startingTimes[i] = temp;
			endingTimes[i] =  temp + t[i];
			processingTimes[i] = endingTimes[i];
			assignments[i] = i;
			
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

	SolverOutput::SolverOutput(Problem* prb, const std::vector<int>& state, double runtime)
	{
		jobNumber = prb->getJobNumber();
		machineNumber = prb->getMachineNumber();        
        jobs = std::vector<std::vector<int>>(machineNumber);
		processingTimes = std::vector<double>();
		startingTimes = std::vector<double>(jobNumber);
		endingTimes = std::vector<double>(jobNumber);
		assignments = std::vector<int>(jobNumber);
		conflicts = std::vector<std::vector<int>>(jobNumber);
		auto T = prb->getLengths();
        auto conf = prb->getConflicts();
		std::vector<int> coreNums(machineNumber);
		std::vector<int> lastJobs(machineNumber);
		std::vector<int> order(machineNumber);

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
        jobs = std::vector<std::vector<int>>(machineNumber);
		processingTimes = std::vector<double>(machineNumber);
		startingTimes = std::vector<double>(jobNumber);
		endingTimes = std::vector<double>(jobNumber);
		assignments = std::vector<int>(jobNumber);
		conflicts = std::vector<std::vector<int>>(jobNumber);
        std::vector<int> coreCount(machineNumber);
		std::vector<int> tempLast(machineNumber);
        auto conf = prb->getConflicts();
		auto T = prb->getLengths();
		double temp;	
		int job;

		jobs = sch->getJobs();

		for(int i = 0; i < machineNumber; i++)
		{
			job = jobs[i][0];
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
			job = jobs[minCore][coreCount[minCore]];
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
	
	SolverOutput::SolverOutput(Problem* prb, const std::vector<int>& x, const std::vector<double>& s, double runtime)
		: runtime(runtime)
	{
		jobNumber = prb->getJobNumber();
        machineNumber = prb->getMachineNumber();
        jobs = std::vector<std::vector<int>>(machineNumber);
		processingTimes = std::vector<double>(machineNumber);
		startingTimes = std::vector<double>(jobNumber);
		endingTimes = std::vector<double>(jobNumber);
		assignments = std::vector<int>(jobNumber);
		conflicts = std::vector<std::vector<int>>(jobNumber);
		auto T = prb->getLengths();
        auto conf = prb->getConflicts();

		std::vector<double> scpy(jobNumber);
		std::vector<int> ind(jobNumber);
		std::vector<bool> smt(jobNumber);
        std::vector<int> lastJobs(machineNumber);
        int minimumCore = 0;

		for (int i = 0; i < jobNumber; i++)
		{
			scpy[i] = s[i];
			smt[i] = true;
            startingTimes[i] = s[i];
            assignments[i] = x[i];
		}

		std::sort(scpy.begin(), scpy.end());

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
	}

	SolverOutput::~SolverOutput() { }
}