#include "TxnSP/PoolingModels/SubsetPool.h"

namespace TxnSP
{
    SubsetPool::SubsetPool(int problemSize, SchedulePool* schedulePool)
        : problemSize_(problemSize),
          inUse_(0),
          schedulePool_(schedulePool) { }

    std::unique_ptr<Subset> SubsetPool::getSubset(int size, std::span<std::unique_ptr<Schedule>> schedules)
    {
        inUse_++;

        if(subsetQueue_.empty())
        {
            return std::make_unique<Subset>(problemSize_, size, schedules, schedulePool_);
        }
        else
        {
            std::unique_ptr<Subset> res = std::move(subsetQueue_.front());
            subsetQueue_.pop();
            res->change(size, schedules);
            return res;
        }
    }

    std::unique_ptr<Subset> SubsetPool::getSubset(int size, std::span<std::unique_ptr<Schedule>> schedules,
        double makespan)
    {
        inUse_++;

        if(subsetQueue_.empty())
        {
            return std::make_unique<Subset>(problemSize_, size, schedules, makespan, schedulePool_);
        }
        else
        {
            std::unique_ptr<Subset> res = std::move(subsetQueue_.front());
            subsetQueue_.pop();
            res->change(size, schedules, makespan);
            return res;
        }
    }

    void SubsetPool::returnSubset(std::unique_ptr<Subset> subset)
    {
        inUse_--;
        subset->clearSchedules();
        subsetQueue_.push(std::move(subset));
    }
}