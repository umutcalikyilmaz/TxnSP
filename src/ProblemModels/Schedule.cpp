#include "TxnSP/ProblemModels/Schedule.h"

namespace TxnSP
{
	Schedule::Schedule(Schedule* sch) : size_(sch->size_), jobNumber_(sch->jobNumber_), machineNumber_(sch->machineNumber_),
	makespan_(sch->makespan_), minimumTime_(sch->minimumTime_), minimumMachine_(sch->minimumMachine_), del_(false)
	{
		jobs_ = new int* [machineNumber_];
		jobNumbers_ = new int[machineNumber_];
		processingTimes_ = new double[machineNumber_];
		lastJobs_ = new int[machineNumber_];

		for (int i = 0; i < machineNumber_; i++)
		{
			jobs_[i] = new int[jobNumber_];
			jobNumbers_[i] = sch->jobNumbers_[i];
			processingTimes_[i] = sch->processingTimes_[i];
			lastJobs_[i] = sch->lastJobs_[i];

			for (int j = 0; j < jobNumber_; j++)
			{
				jobs_[i][j] = sch->jobs_[i][j];
			}
		}
	}

	Schedule::Schedule(Problem* prb, int job) : jobNumber_(prb->getJobNumber()), machineNumber_(prb->getMachineNumber()),
	size_(1), makespan_(prb->getLength(job)), minimumMachine_(1), minimumTime_(0), del_(false)
	{
		jobs_ = new int*[machineNumber_];
		processingTimes_ = new double[machineNumber_];
		lastJobs_ = new int[machineNumber_];
		jobNumbers_ = new int[machineNumber_];

		for (int i = 0; i < machineNumber_; i++)
		{
			jobs_[i] = new int[jobNumber_];
			processingTimes_[i] = 0;
			jobNumbers_[i] = 0;

			for (int j = 0; j < jobNumber_; j++)
			{
				jobs_[i][j] = -1;
			}
		}

		lastJobs_[0] = job;
		jobs_[0][0] = job;
		processingTimes_[0] = makespan_;
		jobNumbers_[0] = 1;
	}

	Schedule::Schedule(Problem* prb, Schedule* sch, int job) : jobNumber_(prb->getJobNumber()), machineNumber_(prb->getMachineNumber()),
	size_(sch->size_), minimumMachine_(sch->minimumMachine_), makespan_(sch->makespan_), minimumTime_(sch->minimumTime_),
	del_(false)
	{
		jobs_ = new int*[machineNumber_];
		jobNumbers_ = new int[machineNumber_];
		processingTimes_ = new double[machineNumber_];
		lastJobs_ = new int[machineNumber_];

		for (int i = 0; i < machineNumber_; i++)
		{
			jobs_[i] = new int[jobNumber_];
			jobNumbers_[i] = sch->jobNumbers_[i];
			processingTimes_[i] = sch->processingTimes_[i];
			lastJobs_[i] = sch->lastJobs_[i];

			for (int j = 0; j < jobNumber_; j++)
			{
				jobs_[i][j] = sch->jobs_[i][j];
			}
		}

		double temp = minimumTime_;
		int coun = (size_ < machineNumber_) ? size_ : machineNumber_;
		bool** conf = prb->getConflicts();

		for (int i = 0; i < coun; i++)
		{
			if (conf[job][lastJobs_[i]])
			{
				if (temp < processingTimes_[i])
				{
					temp = processingTimes_[i];
				}
			}
		}

		size_++;
		lastJobs_[minimumMachine_] = job;
		jobs_[minimumMachine_][jobNumbers_[minimumMachine_]] = job;
		jobNumbers_[minimumMachine_]++;
		processingTimes_[minimumMachine_] = temp + prb->getLength(job);
		minimumTime_ = 1.7976931e+308;

		if (processingTimes_[minimumMachine_] > makespan_)
		{
			makespan_ = processingTimes_[minimumMachine_];
		}

		for (int i = 0; i < machineNumber_; i++)
		{
			if (processingTimes_[i] < minimumTime_)
			{
				minimumTime_ = processingTimes_[i];
				minimumMachine_ = i;
			}
		}
	}

    Schedule::Schedule(Problem* prb, __uint128_t index, int* perm, int* a) : jobNumber_(prb->getJobNumber()),
	machineNumber_(prb->getMachineNumber()), size_(prb->getJobNumber()), minimumMachine_(0), del_(false)
    {
        decode(index, perm, a, jobNumber_, prb->getDivid());
		jobs_ = new int*[machineNumber_]; 
		processingTimes_ = new double[machineNumber_];
		lastJobs_ = new int[machineNumber_]; 
		jobNumbers_ = new int[machineNumber_]; 
        makespan_ = 0;
		minimumTime_ = 1.7976931e+308;

		for (int i = 0; i < machineNumber_; i++)
		{
			jobs_[i] = new int[jobNumber_];
			jobNumbers_[i] = 0;
			processingTimes_[i] = 0;

			for (int j = 0; j < jobNumber_; j++) 
			{
				jobs_[i][j] = -1;
			}
		}

        bool** conf = prb->getConflicts();

		for (int i = 0; i < machineNumber_; i++)
		{
			double temp = 0;

			jobs_[i][0] = perm[i];
			lastJobs_[i] = perm[i];
			jobNumbers_[i]++;            		

			for (int j = 0; j <= i; j++)
			{
				if (conf[perm[i]][lastJobs_[j]])
				{
					if (processingTimes_[j] > temp)
					{
						temp = processingTimes_[j];
					}
				}
			}

			processingTimes_[i] = temp + prb->getLength(perm[i]);

			if (processingTimes_[i] < minimumTime_)
			{
				minimumTime_ = processingTimes_[i];
				minimumMachine_ = i;
			}
		}

		for (int i = machineNumber_; i < jobNumber_; i++)
		{
			double temp = processingTimes_[minimumMachine_];

			jobs_[minimumMachine_][jobNumbers_[minimumMachine_]] = perm[i];
			lastJobs_[minimumMachine_] = perm[i];			
			jobNumbers_[minimumMachine_]++;

			for (int j = 0; j < machineNumber_; j++)
			{
				if (conf[perm[i]][lastJobs_[j]])
				{
					if (processingTimes_[j] > temp)
					{
						temp = processingTimes_[j];
					}
				}
			}

			processingTimes_[minimumMachine_] = temp + prb->getLength(perm[i]);
			minimumTime_ = 1.7976931e+308;

			for (int j = 0; j < machineNumber_; j++)
			{
				if (processingTimes_[j] < minimumTime_)
				{
					minimumTime_ = processingTimes_[j];
					minimumMachine_ = j;
				}
			}
		}		

		for (int i = 0; i < jobNumber_; i++)
		{
			if (processingTimes_[i] > makespan_)
			{
				makespan_ = processingTimes_[i];
			}
		}
    }

	Schedule::Schedule(Problem* prb, int* state) : jobNumber_(prb->getJobNumber()), machineNumber_(prb->getMachineNumber()),
	size_(prb->getJobNumber()), minimumMachine_(0), del_(true)
	{		
		jobs_ = new int*[machineNumber_];
		processingTimes_ = new double[machineNumber_];
		lastJobs_ = new int[machineNumber_];
		jobNumbers_ = new int[machineNumber_];
		order_ = new int[machineNumber_];
		bool** conf = prb->getConflicts();
		double* T = prb->getLengths();
		int job;
		double temp;
		
		for (int i = 0; i < machineNumber_; i++)
		{
			temp = 0;
			job = state[i];
			
			jobs_[i] = new int[jobNumber_];
			jobs_[i][0] = job;
			lastJobs_[i] = job;
			jobNumbers_[i] = 1;
			
			for (int j = i - 1; j > -1; j--)
			{
				if (conf[job][lastJobs_[order_[j]]])
				{
					temp = processingTimes_[order_[j]];
					break;
				}
			}

			processingTimes_[i] = temp + T[job];
			int ind = findPlace2(processingTimes_, order_, processingTimes_[i], i);
			shift(order_, ind, i);
			order_[ind] = i;
		}

		for (int i = machineNumber_; i < jobNumber_; i++)
		{
			temp = processingTimes_[order_[0]];
			job = state[i];

			jobs_[order_[0]][jobNumbers_[order_[0]]] = job;
			lastJobs_[order_[0]] = job;
			jobNumbers_[order_[0]]++;
			
			for (int j = machineNumber_ - 1; j > 0; j--)
			{
				if (conf[job][lastJobs_[order_[j]]])
				{
					temp = processingTimes_[order_[j]];
					break;
				}
			}

			processingTimes_[order_[0]] = temp + T[job];
			int ind = findPlace(processingTimes_, order_, processingTimes_[order_[0]], machineNumber_);
			int tempi = order_[0];
			shift(order_, ind);
			order_[ind] = tempi;
		}

		minimumTime_ = processingTimes_[order_[0]];
		makespan_ = processingTimes_[order_[machineNumber_ - 1]];
	}

	void Schedule::change(Schedule* sch)
	{	
		size_ = sch->size_;
		makespan_ = sch->makespan_;
		minimumTime_ = sch->minimumTime_;
		minimumMachine_ = sch->minimumMachine_;
			
		for (int i = 0; i < machineNumber_; i++)
		{
			jobNumbers_[i] = sch->jobNumbers_[i];
			processingTimes_[i] = sch->processingTimes_[i];
			lastJobs_[i] = sch->lastJobs_[i];
			
			for (int j = 0; j < jobNumber_; j++)
			{
				jobs_[i][j] = sch->jobs_[i][j];
			}
		}
	}

	void Schedule::change(Problem* prb, int job)
	{
		size_ = 1;
		minimumMachine_ = 1;
		minimumTime_ = 0;
		makespan_ = prb->getLength(job);

		for (int i = 0; i < machineNumber_; i++)
		{
			processingTimes_[i] = 0;
			jobNumbers_[i] = 0;

			for (int j = 0; j < jobNumber_; j++)
			{
				jobs_[i][j] = -1;
			}
		}

		lastJobs_[0] = job;
		jobs_[0][0] = job;
		processingTimes_[0] = makespan_;		
		jobNumbers_[0] = 1;
	}

	void Schedule::change(Problem* prb, Schedule* sch, int job)
	{
		change(sch);

		int ind = minimumMachine_;
		double temp = minimumTime_;
		int coun = (size_ < machineNumber_) ? size_ : machineNumber_;
		bool** conf = prb->getConflicts();

		for(int i = 0; i < coun; i++)
		{
			if(conf[job][lastJobs_[i]])
			{
				if (temp < processingTimes_[i])
				{
					temp = processingTimes_[i];
				}
			}
		}

		size_++;
		lastJobs_[ind] = job;
		jobs_[ind][jobNumbers_[ind]] = job;
		jobNumbers_[ind]++;
		processingTimes_[ind] = temp + prb->getLength(job);
		minimumTime_ = 1.7976931e+308;

		if (processingTimes_[ind] > makespan_)
		{
			makespan_ = processingTimes_[ind];
		}

		for (int i = 0; i < machineNumber_; i++)
		{
			if (processingTimes_[i] < minimumTime_)
			{
				minimumTime_ = processingTimes_[i];
				minimumMachine_ = i;
			}
		}	
	}

	void Schedule::change(Problem* prb, __uint128_t index, int* perm, int* a)
	{
		size_ = prb->getJobNumber();
		minimumMachine_ = 0;
		decode(index, perm, a, jobNumber_, prb->getDivid());
		minimumTime_ = 1.7976931e+308;

		for (int i = 0; i < machineNumber_; i++)
		{
			jobNumbers_[i] = 0;
			processingTimes_[i] = 0;

			for (int j = 0; j < jobNumber_; j++) 
			{
				jobs_[i][j] = -1;
			}
		}

		bool** conf = prb->getConflicts();
		double* t = prb->getLengths();

		for (int i = 0; i < machineNumber_; i++)
		{
			double temp = 0;

			jobs_[i][0] = perm[i];
			lastJobs_[i] = perm[i];
			jobNumbers_[i]++;

			for (int j = 0; j <= i; j++)
			{
				if (conf[perm[i]][lastJobs_[j]])
				{
					if (processingTimes_[j] > temp)
					{
						temp = processingTimes_[j];
					}
				}
			}

			processingTimes_[i] = temp + t[perm[i]];

			if (processingTimes_[i] < minimumTime_)
			{
				minimumTime_ = processingTimes_[i];
				minimumMachine_ = i;
			}
		}

		for (int i = machineNumber_; i < jobNumber_; i++)
		{
			double temp = processingTimes_[minimumMachine_];

			jobs_[minimumMachine_][jobNumbers_[minimumMachine_]] = perm[i];
			lastJobs_[minimumMachine_] = perm[i];
			jobNumbers_[minimumMachine_]++;

			for (int j = 0; j < machineNumber_; j++)
			{
				if (conf[perm[i]][lastJobs_[j]])
				{
					if (processingTimes_[j] > temp)
					{
						temp = processingTimes_[j];
					}
				}
			}

			processingTimes_[minimumMachine_] = temp + t[perm[i]];
			minimumTime_ = 1.7976931e+308;

			for (int j = 0; j < machineNumber_; j++)
			{
				if (processingTimes_[j] < minimumTime_)
				{
					minimumTime_ = processingTimes_[j];
					minimumMachine_ = j;
				}
			}
		}

		makespan_ = 0;

		for (int i = 0; i < machineNumber_; i++)
		{
			if (processingTimes_[i] > makespan_)
			{
				makespan_ = processingTimes_[i];
			}
		}
	}

	void Schedule::change(Problem* prb, int* state)
	{		
		bool** conf = prb->getConflicts();
		double* T = prb->getLengths();
		double temp;
		int job;

		for (int i = 0; i < machineNumber_; i++)
		{
			temp = 0;
			job = state[i];

			jobs_[i][0] = job;
			lastJobs_[i] = job;
			jobNumbers_[i] = 1;
			
			for (int j = i - 1; j > -1; j--)
			{
				if (conf[job][lastJobs_[order_[j]]])
				{
					temp = processingTimes_[order_[j]];
					break;
				}
			}

			processingTimes_[i] = temp + T[job];
			int ind = findPlace2(processingTimes_, order_, processingTimes_[i], i);
			shift(order_, ind, i);
			order_[ind] = i;	
			
		}

		for (int i = machineNumber_; i < jobNumber_; i++)
		{			
			temp = processingTimes_[order_[0]];
			job = state[i];

			jobs_[order_[0]][jobNumbers_[order_[0]]] = job;
			lastJobs_[order_[0]] = job;
			jobNumbers_[order_[0]]++;

			for (int j = machineNumber_ - 1; j > 0; j--)
			{
				if (conf[job][lastJobs_[order_[j]]])
				{
					temp = processingTimes_[order_[j]];
					break;
				}
			}

			processingTimes_[order_[0]] = temp + T[job];
			int tempi = order_[0];
			int ind = findPlace(processingTimes_, order_, processingTimes_[order_[0]], machineNumber_);			
			shift(order_, ind);
			order_[ind] = tempi;
			
		}

		makespan_ = processingTimes_[order_[machineNumber_ - 1]];
		minimumTime_ = processingTimes_[order_[0]];
	}

    int Schedule::getJobNumber() const
    {
        return jobNumber_;
    }

	int Schedule::getMachineNumber() const
    {
        return machineNumber_;
    }

	int Schedule::getSize() const
    {
        return size_;
    }

	double Schedule::getMakespan() const
    {
        return makespan_;
    }

	double Schedule::getMinimumTime() const
    {
        return minimumTime_;
    }

	int Schedule::getMinimumMachine() const
    {
        return minimumMachine_;
    }

	int** Schedule::getJobs() const
    {
        return jobs_;
    }    

	double* Schedule::getProcessingTimes() const
    {
        return processingTimes_;
    }

	int* Schedule::getLastJobs() const
    {
        return lastJobs_;
    }

	int* Schedule::getJobNumbers() const
    {
        return jobNumbers_;
    }

	bool Schedule::isEquivalent(Schedule* sch)
	{
		std::map<int,double> ptMap;
		
		for(int i = 0; i < size_; i++)
		{
			ptMap[jobs_[i][0]] = processingTimes_[i];
		}

		for(int i = 0; i < size_; i++)
		{
			std::map<int,double>::iterator item = ptMap.find(sch->jobs_[i][0]);

			if(item == ptMap.end())
			{
				return false;
			}

			if(item->second - processingTimes_[i] < 0.00001 && item->second - processingTimes_[i] > -0.00001)
			{
				return false;
			}
		}

		return true;
	}

    Schedule::~Schedule()
    {
        for (int i = 0; i < machineNumber_; i++)
		{
			delete[] jobs_[i];
		}

		delete[] jobs_;
		delete[] jobNumbers_;
		delete[] processingTimes_;
		delete[] lastJobs_;

		if(del_)
		{
			delete[] order_;
		}
    }
}