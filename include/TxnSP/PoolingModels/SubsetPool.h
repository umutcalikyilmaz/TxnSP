#pragma once
#include "TxnSP/ProblemModels/Subset.h"

namespace TxnSP
{
    class SubsetPool
    {
    public:

        SubsetPool(int problemSize, int max, SchedulePool* schedulePool);

        Subset* getSubset(int size, int scheduleNumber, Schedule** schedule);

        Subset* getSubset(int size, int scheduleNumber, Schedule** schedule, double makespan);

        void returnSubset(Subset* subset);

        ~SubsetPool();

    private:

        std::vector<Subset*> subsets_;
        std::queue<Subset*> subsetQueue_;
        SchedulePool* schedulePool_;
        int problemSize_;
        int inUse_;
    };
}