#pragma once
#include <queue>
#include <vector>
#include "TxnSP/ProblemModels/Schedule.h"

namespace TxnSP
{
    class SchedulePool
    { 
    public:

        SchedulePool(int n, int m);

        std::unique_ptr<Schedule> getSchedule(Schedule* schedule);

        std::unique_ptr<Schedule> getSchedule(Problem* prb, int job);

        std::unique_ptr<Schedule> getSchedule(Problem* prb, Schedule* schedule, int job);

        std::unique_ptr<Schedule> getSchedule(Problem* prb, LargeInt index, std::vector<int>& perm, std::vector<int>& a);

        std::unique_ptr<Schedule> getSchedule(Problem* prb, const std::vector<int>& state);

        void returnSchedule(std::unique_ptr<Schedule> schedule);

    private:
    
        std::queue<std::unique_ptr<Schedule>> scheduleQueue_;
        int inUse_;
        int jobNumber_;
        int machineNumber_;
    };    
}