#include "TxnSP/PoolingModels/SchedulePool.h"

namespace TxnSP
{
    SchedulePool::SchedulePool(int n, int m, int max) : jobNumber_(n), machineNumber_(m), inUse_(0), schedules_(max) { }

    Schedule* SchedulePool::getSchedule(Schedule* sch)
    {
        inUse_++;
        Schedule* res;

        if(scheduleQueue_.empty())
        {
            res = new Schedule(sch);
            schedules_.push_back(res);            
        }
        else
        {
            res = scheduleQueue_.front();
            scheduleQueue_.pop();
            res->change(sch);
        }

        return res;
    }

    Schedule* SchedulePool::getSchedule(Problem* prb, int job)
    {
        inUse_++;
        Schedule* res;

        if(scheduleQueue_.empty())
        {
            res = new Schedule(prb, job);
            schedules_.push_back(res);            
        }
        else
        {
            res = scheduleQueue_.front();
            scheduleQueue_.pop();
            res->change(prb, job);
        }

        return res;
    }

    Schedule* SchedulePool::getSchedule(Problem* prb, Schedule* sch, int job)
    {
        inUse_++;
        Schedule* res;

        if(scheduleQueue_.empty())
        {
            res = new Schedule(prb, sch, job);
            schedules_.push_back(res);            
        }
        else
        {
            res = scheduleQueue_.front();
            scheduleQueue_.pop();
            res->change(prb, sch, job);
        }

        return res;
    }

    Schedule* SchedulePool::getSchedule(Problem* prb, __uint128_t index, int* perm, int* a)
    {
        inUse_++;
        Schedule* res;

        if(scheduleQueue_.empty())
        {
            res = new Schedule(prb, index, perm, a);
            schedules_.push_back(res);            
        }
        else
        {
            res = scheduleQueue_.front();
            scheduleQueue_.pop();
            res->change(prb, index, perm, a);
        }

        return res;
    }

    Schedule* SchedulePool::getSchedule(Problem* prb, int* state)
    {
        inUse_++;
        Schedule* res;

        if(scheduleQueue_.empty())
        {
            res = new Schedule(prb, state);
            schedules_.push_back(res);            
        }
        else
        {
            res = scheduleQueue_.front();
            scheduleQueue_.pop();
            res->change(prb, state);
        }

        return res;
    }

    void SchedulePool::returnSchedule(Schedule* sch)
    {
        inUse_--;
        scheduleQueue_.push(sch);
    }

    SchedulePool::~SchedulePool()
    {
		while(!scheduleQueue_.empty())
		{
			scheduleQueue_.pop();
		}        

		while(!schedules_.empty())
		{
			delete schedules_.back();
			schedules_.pop_back();
		}
    }
}