#pragma once
#include "TxnSP/ProblemModels/Subset.h"

namespace TxnSP
{
    class SubsetPool
    {
    public:

        SubsetPool(int problemSize, SchedulePool* schedulePool);

        std::unique_ptr<Subset> getSubset(int size, std::span<std::unique_ptr<Schedule>> schedule);

        std::unique_ptr<Subset> getSubset(int size, std::span<std::unique_ptr<Schedule>> schedule, double makespan);

        void returnSubset(std::unique_ptr<Subset> subset);

    private:

        std::queue<std::unique_ptr<Subset>> subsetQueue_;
        SchedulePool* schedulePool_;
        int problemSize_;
        int inUse_;
    };
}