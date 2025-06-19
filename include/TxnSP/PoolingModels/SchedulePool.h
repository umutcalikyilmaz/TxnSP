#pragma once
#include <queue>
#include <vector>
#include "TxnSP/ProblemModels/Schedule.h"

namespace TxnSP
{
    class SchedulePool
    { 
    public:

        SchedulePool(int n, int m, int max);

        Schedule* getSchedule(Schedule* schedule);

        Schedule* getSchedule(Problem* prb, int job);

        Schedule* getSchedule(Problem* prb, Schedule* schedule, int job);

        Schedule* getSchedule(Problem* prb, __uint128_t index, int* perm, int* a);

        Schedule* getSchedule(Problem* prb, int* state);

        void returnSchedule(Schedule* schedule);

        ~SchedulePool();

    private:
    
        std::vector<Schedule*> schedules_;
        std::queue<Schedule*> scheduleQueue_;
        int inUse_;
        int jobNumber_;
        int machineNumber_;
    };    
}