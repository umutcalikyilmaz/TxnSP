#include "TxnSP/ProblemModels/Schedule.h"

namespace TxnSP
{
	void Schedule::initialize(Problem* prb, const std::vector<int>& state)
	{
		auto conf = prb->getConflicts();
		auto T = prb->getLengths();
		int job;
		double temp;
		
		for (int i = 0; i < machineNumber_; i++)
		{
			temp = 0;
			job = state[i];
			jobs_[i].push_back(job);
			lastJobs_[i] = job;
			
			for (int j = i - 1; j > -1; j--)
			{
				if (conf[job][lastJobs_[order_[j]]] == 1)
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

			jobs_[order_[0]].push_back(job);
			lastJobs_[order_[0]] = job;
			
			for (int j = machineNumber_ - 1; j > 0; j--)
			{
				if (conf[job][lastJobs_[order_[j]]] == 1)
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

	Schedule::Schedule(Schedule* sch)
		: size_(sch->size_),
		  jobNumber_(sch->jobNumber_),
	  	  machineNumber_(sch->machineNumber_),
		  makespan_(sch->makespan_),
		  minimumTime_(sch->minimumTime_),
		  minimumMachine_(sch->minimumMachine_),
		  processingTimes_(sch->processingTimes_),
		  jobs_(sch->jobs_),
		  lastJobs_(sch->lastJobs_),
		  order_(sch->order_) {	}

	Schedule::Schedule(Problem* prb, int job)
		: jobNumber_(prb->getJobNumber()),
		  machineNumber_(prb->getMachineNumber()),
		  size_(1),
		  makespan_(prb->getLength(job)),
		  minimumMachine_(1),
		  minimumTime_(0),
		  processingTimes_(prb->getMachineNumber()),
		  jobs_(prb->getMachineNumber()),
		  lastJobs_(prb->getMachineNumber())
	{
		for (int i = 0; i < machineNumber_; i++)
		{
			processingTimes_[i] = 0;
		}

		lastJobs_[0] = job;
		jobs_[0].push_back(job);
		processingTimes_[0] = makespan_;
	}

	Schedule::Schedule(Problem* prb, Schedule* sch, int job)
		: jobNumber_(prb->getJobNumber()),
		  machineNumber_(prb->getMachineNumber()),
		  size_(sch->size_),
		  minimumMachine_(sch->minimumMachine_),
		  makespan_(sch->makespan_),
		  minimumTime_(sch->minimumTime_),
		  processingTimes_(sch->processingTimes_),
		  jobs_(sch->jobs_),
		  lastJobs_(sch->lastJobs_)
	{
		double temp = minimumTime_;
		int coun = (size_ < machineNumber_) ? size_ : machineNumber_;
		auto conf = prb->getConflicts();

		for (int i = 0; i < coun; i++)
		{
			if (conf[job][lastJobs_[i]] == 1)
			{
				if (temp < processingTimes_[i])
				{
					temp = processingTimes_[i];
				}
			}
		}

		size_++;
		lastJobs_[minimumMachine_] = job;
		jobs_[minimumMachine_].push_back(job);
		processingTimes_[minimumMachine_] = temp + prb->getLength(job);
		minimumTime_ = DBL_MAX;

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

    Schedule::Schedule(Problem* prb, LargeInt index, std::vector<int>& perm, std::vector<int>& a)
		: jobNumber_(prb->getJobNumber()),
		  machineNumber_(prb->getMachineNumber()),
		  size_(prb->getJobNumber()),
		  minimumMachine_(0),
		  processingTimes_(prb->getMachineNumber()),
		  jobs_(prb->getMachineNumber()),
		  lastJobs_(prb->getMachineNumber()),
		  order_(prb->getMachineNumber())
    {
        decode(index, perm, a, jobNumber_, prb->getDivid());
        initialize(prb, perm);
    }

	Schedule::Schedule(Problem* prb, const std::vector<int>&  state)
		: jobNumber_(prb->getJobNumber()),
		  machineNumber_(prb->getMachineNumber()),
		  size_(prb->getJobNumber()),
		  minimumMachine_(0),
		  processingTimes_(prb->getMachineNumber()),
		  jobs_(prb->getMachineNumber()),
		  lastJobs_(prb->getMachineNumber()),
		  order_(prb->getMachineNumber())
	{
		initialize(prb, state);
	}

	void Schedule::change(Schedule* sch)
	{	
		size_ = sch->size_;
		makespan_ = sch->makespan_;
		minimumTime_ = sch->minimumTime_;
		minimumMachine_ = sch->minimumMachine_;
		processingTimes_ = sch->processingTimes_;
		jobs_ = sch->jobs_;
		lastJobs_ = sch->lastJobs_;
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
			jobs_[i].clear();
		}

		lastJobs_[0] = job;
		jobs_[0].push_back(job);
		processingTimes_[0] = makespan_;
	}

	void Schedule::change(Problem* prb, Schedule* sch, int job)
	{
		change(sch);

		int ind = minimumMachine_;
		double temp = minimumTime_;
		int coun = (size_ < machineNumber_) ? size_ : machineNumber_;
		auto conf = prb->getConflicts();

		for(int i = 0; i < coun; i++)
		{
			if(conf[job][lastJobs_[i]] == 1)
			{
				if (temp < processingTimes_[i])
				{
					temp = processingTimes_[i];
				}
			}
		}

		size_++;
		lastJobs_[ind] = job;
		jobs_[ind].push_back(job);
		processingTimes_[ind] = temp + prb->getLength(job);
		minimumTime_ = DBL_MAX;

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

	void Schedule::change(Problem* prb, LargeInt index, std::vector<int>& perm, std::vector<int>& a)
	{
		size_ = prb->getJobNumber();
		minimumMachine_ = 0;
		decode(index, perm, a, jobNumber_, prb->getDivid());
		minimumTime_ = DBL_MAX;

		for (int i = 0; i < machineNumber_; i++)
		{
			processingTimes_[i] = 0;
			jobs_[i].clear();
		}

		initialize(prb, perm);
	}

	void Schedule::change(Problem* prb, const std::vector<int>& state)
	{
		size_ = prb->getJobNumber();
		minimumMachine_ = 0;

		for (int i = 0; i < machineNumber_; i++)
		{
			processingTimes_[i] = 0;
			jobs_[i].clear();
		}

		initialize(prb, state);
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

	const std::vector<std::vector<int>>& Schedule::getJobs() const
    {
        return jobs_;
    }    

	const std::vector<double>& Schedule::getProcessingTimes() const
    {
        return processingTimes_;
    }

	const std::vector<int>& Schedule::getLastJobs() const
    {
        return lastJobs_;
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
}