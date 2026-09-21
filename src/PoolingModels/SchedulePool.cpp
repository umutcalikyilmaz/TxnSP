#include "TxnSP/PoolingModels/SchedulePool.h"

namespace TxnSP
{
    SchedulePool::SchedulePool(int n, int m)
        : jobNumber_(n),
          machineNumber_(m),
          inUse_(0) { }

    std::unique_ptr<Schedule> SchedulePool::getSchedule(Schedule* sch)
    {
        inUse_++;

        if(scheduleQueue_.empty())
        {
            return std::make_unique<Schedule>(sch);           
        }
        else
        {
            std::unique_ptr<Schedule> res = move(scheduleQueue_.front());
            scheduleQueue_.pop();
            res->change(sch);
            return res;
        }
    }

    std::unique_ptr<Schedule> SchedulePool::getSchedule(Problem* prb, int job)
    {
        inUse_++;

        if(scheduleQueue_.empty())
        {
            return std::make_unique<Schedule>(prb, job);           
        }
        else
        {
            std::unique_ptr<Schedule> res = std::move(scheduleQueue_.front());
            scheduleQueue_.pop();
            res->change(prb, job);
            return res;
        }
    }

    std::unique_ptr<Schedule> SchedulePool::getSchedule(Problem* prb, Schedule* sch, int job)
    {
        inUse_++;

        if(scheduleQueue_.empty())
        {
            return std::make_unique<Schedule>(prb, sch, job);       
        }
        else
        {
            std::unique_ptr<Schedule> res = std::move(scheduleQueue_.front());
            scheduleQueue_.pop();
            res->change(prb, sch, job);
            return res;
        }
    }

    std::unique_ptr<Schedule> SchedulePool::getSchedule(Problem* prb, LargeInt index, std::vector<int>& perm, std::vector<int>& a)
    {
        inUse_++;

        if(scheduleQueue_.empty())
        {
            return std::make_unique<Schedule>(prb, index, perm, a);        
        }
        else
        {
            std::unique_ptr<Schedule> res = std::move(scheduleQueue_.front());
            scheduleQueue_.pop();
            res->change(prb, index, perm, a);
            return res;
        }
    }

    std::unique_ptr<Schedule> SchedulePool::getSchedule(Problem* prb, const std::vector<int>& state)
    {
        inUse_++;

        if(scheduleQueue_.empty())
        {
            return std::make_unique<Schedule>(prb, state);          
        }
        else
        {
            std::unique_ptr<Schedule> res = std::move(scheduleQueue_.front());
            scheduleQueue_.pop();
            res->change(prb, state);
            return res;
        }
    }

    void SchedulePool::returnSchedule(std::unique_ptr<Schedule> sch)
    {
        inUse_--;
        scheduleQueue_.push(move(sch));
    }
}