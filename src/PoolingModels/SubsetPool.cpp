#include "TxnSP/PoolingModels/SubsetPool.h"

namespace TxnSP
{
    SubsetPool::SubsetPool(int problemSize, int max, SchedulePool* schedulePool) : problemSize_(problemSize), inUse_(0),
    schedulePool_(schedulePool), subsets_(max) { }

    Subset* SubsetPool::getSubset(int size, int scheduleNumber, Schedule** schedule)
    {
        inUse_++;
        Subset* res;

        if(subsetQueue_.empty())
        {
            res = new Subset(problemSize_, size, scheduleNumber, schedule, schedulePool_);
            subsets_.push_back(res);
        }
        else
        {
            res = subsetQueue_.front();
            subsetQueue_.pop();
            res->change(problemSize_, size, scheduleNumber, schedule);
        }

        return res;
    }

    Subset* SubsetPool::getSubset(int size, int schNum, Schedule** sch, double makespan)
    {
        inUse_++;
        Subset* res;

        if(subsetQueue_.empty())
        {
            res = new Subset(problemSize_, size, schNum, sch, makespan, schedulePool_);
            subsets_.push_back(res);
        }
        else
        {
            res = subsetQueue_.front();
            subsetQueue_.pop();
            res->change(size, schNum, makespan, sch);
        }

        return res;
    }

    void SubsetPool::returnSubset(Subset* subset)
    {
        inUse_--;
        subsetQueue_.push(subset);
    }

    SubsetPool::~SubsetPool()
    {
        while (subsetQueue_.empty())
        {
            subsetQueue_.pop();
        }

        while (!subsets_.empty())
        {
            delete subsets_.back();
            subsets_.pop_back();
        }
    }
}