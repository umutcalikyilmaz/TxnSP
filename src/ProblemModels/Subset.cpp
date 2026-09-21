#include "TxnSP/ProblemModels/Subset.h"

namespace TxnSP
{
    void Subset::clearList()
	{
        for(int i = 0; i < scheduleNumber_; i++)
        {
            schedulePool_->returnSchedule(move(list_[i]));
        }
	}

    void Subset::checkEquivalency()
    {
        int ind = 0;

		while (ind < scheduleNumber_)
		{
			int ind2 = ind + 1;
			int shiftCount = 0;			

			while (ind2 < scheduleNumber_)
			{
				if (list_[ind]->isEquivalent(list_[ind2].get()))
				{
					shiftCount++;
					schedulePool_->returnSchedule(std::move(list_[ind2]));
				}
				else if (shiftCount > 0)
				{
					list_[ind2 - shiftCount] = std::move(list_[ind2]);
				}

				ind2++;
			}

			ind++;
			scheduleNumber_ -= shiftCount;
		}
    }

    void Subset::finalize() 	
    {
        double min = list_[0]->getMakespan();
		int ind = 0;

		for (int i = 1; i < scheduleNumber_; i++)
		{
			if (list_[i]->getMakespan() < min)
			{
				schedulePool_->returnSchedule(std::move(list_[ind]));
				ind = i;
				min = list_[i]->getMakespan();
			}
			else
			{
				schedulePool_->returnSchedule(std::move(list_[i]));
			}
		}

		list_[0] = move(list_[ind]);
		scheduleNumber_ = 1;
    }

    void Subset::eliminate(double makespan)
    {
        int shiftCount = 0;

		for (int i = 0; i < scheduleNumber_; i++)
		{
			if (list_[i]->getMinimumTime() >= makespan && shiftCount < scheduleNumber_ - 1)
			{
				shiftCount++;
				schedulePool_->returnSchedule(std::move(list_[i]));
			}
			else
			{
				list_[i - shiftCount] = std::move(list_[i]);
			}
		}

		scheduleNumber_ -= shiftCount;

		if (size_ <= list_[0]->getMachineNumber())
		{
			checkEquivalency();
		}
    }

    Subset::Subset(int problemSize, int size, std::span<std::unique_ptr<Schedule>> schedules,
		SchedulePool* schedulePool)
		: problemSize_(problemSize),
		  size_(size),
		  scheduleNumber_(schedules.size()),
		  schedulePool_(schedulePool)
	{
		
		for(auto& sch : schedules)
		{
			list_.push_back(move(sch));
		}

		if(list_.empty())
		{
			int asf = 0;
		}
	}

    Subset::Subset(int problemSize, int size, std::span<std::unique_ptr<Schedule>> schedules, double makespan,
		SchedulePool* schedulePool)
		: problemSize_(problemSize),
		  size_(size),
		  scheduleNumber_(schedules.size()),
		  schedulePool_(schedulePool)
    {
		for(auto& sch : schedules)
		{
			list_.push_back(move(sch));
		}

        if (size == problemSize)
		{
			finalize();
		}
		else
		{
			eliminate(makespan);
		}
    }

    void Subset::change(int size, std::span<std::unique_ptr<Schedule>> schedules)
    {
		size_ = size;	
        scheduleNumber_ = schedules.size();

		for(auto& sch : schedules)
		{
			list_.push_back(move(sch));
		}
    }

	void Subset::change(int size, std::span<std::unique_ptr<Schedule>> schedules, double makespan)
	{
		size_ = size;	
		scheduleNumber_ = schedules.size();

		for(auto& sch : schedules)
		{
			list_.push_back(move(sch));
		}

		if (size == problemSize_)
		{
			finalize();
		}
		else
		{
			eliminate(makespan);
		}
	}

    int Subset::getScheduleNumber()
    {
        return scheduleNumber_;
    }

	Schedule* Subset::getSchedule(int ind)
	{
		return list_[ind].get();
	}

	void Subset::clearSchedules()
	{
		for(int i = 0; i < scheduleNumber_; i++)
		{
			schedulePool_->returnSchedule(move(list_[i]));
		}

		list_.clear();
	}
}