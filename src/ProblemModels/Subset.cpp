#include "TxnSP/ProblemModels/Subset.h"

namespace TxnSP
{
    void Subset::clearList()
	{
        for(int i = 0; i < scheduleNumber_; i++)
        {
            schedulePool_->returnSchedule(list_[i]);
        }
	}

    void Subset::checkEquivalency()
    {
        int ind = 0;

		while (ind < scheduleNumber_)
		{
			int ind2 = ind + 1;
			int move = 0;

			while (ind2 < scheduleNumber_)
			{
				if (list_[ind]->isEquivalent(list_[ind2]))
				{
					move++;
					schedulePool_->returnSchedule(list_[ind2]);
				}
				else if (move > 0)
				{
					list_[ind2 - move] = list_[ind2];
				}

				ind2++;
			}

			ind++;
			scheduleNumber_ -= move;
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
				schedulePool_->returnSchedule(list_[ind]);
				ind = i;
				min = list_[i]->getMakespan();
			}
			else
			{
				schedulePool_->returnSchedule(list_[i]);
			}
		}

		Schedule* temp = list_[ind];
		delete[] list_;
		list_ = new Schedule*[1](temp);
		scheduleNumber_ = 1;
    }

    void Subset::eliminate(double makespan)
    {
        int move = 0;

		for (int i = 0; i < scheduleNumber_; i++)
		{
			if (list_[i]->getMinimumTime() >= makespan && move < scheduleNumber_ - 1)
			{
				move++;
				schedulePool_->returnSchedule(list_[i]);
			}
			else
			{
				list_[i - move] = list_[i];
			}
		}

		scheduleNumber_ -= move;

		if (size_ <= list_[0]->getMachineNumber())
		{
			checkEquivalency();
		}
    }

    Subset::Subset(int problemSize, int size, int scheduleNumber, Schedule** schedules, SchedulePool* schedulePool)
	: problemSize_(problemSize), size_(size), scheduleNumber_(scheduleNumber), list_(schedules), schedulePool_(schedulePool) { }

    Subset::Subset(int problemSize, int size, int scheduleNumber, Schedule** schedules, double makespan, SchedulePool* schedulePool)
	: problemSize_(problemSize), size_(size), scheduleNumber_(scheduleNumber), list_(schedules), schedulePool_(schedulePool)
    {
        if (size == problemSize)
		{
			finalize();
		}
		else
		{
			eliminate(makespan);
		}
    }

    void Subset::change(int size, int scheduleNumber, Schedule** sch)
    {   
        clearList();
        delete[] list_;
        list_ = sch;
		size_ = size;
        scheduleNumber_ = scheduleNumber;
		list_ = sch;
    }

	void Subset::change(int size, int scheduleNumber, double makespan, Schedule** schedules)
	{
		clearList();
		delete[] list_;
		list_ = schedules;
		size_ = size;
		scheduleNumber_ = scheduleNumber;

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
        return list_[ind];
    }

    Subset::~Subset()
    {
        delete[] list_;
    }
}