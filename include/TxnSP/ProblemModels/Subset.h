#pragma once
#include "TxnSP/PoolingModels/SchedulePool.h"

namespace TxnSP
{
    class Subset
    {
    public:

        Subset(int problemSize, int size, std::span<std::unique_ptr<Schedule>> schedules,
            SchedulePool* schedulePool);

        Subset(int problemSize, int size, std::span<std::unique_ptr<Schedule>> schedules, double makespan,
            SchedulePool* schedulePool);

		int getScheduleNumber();

        Schedule* getSchedule(int ind);

    private:

        std::vector<std::unique_ptr<Schedule>> list_;
        SchedulePool* schedulePool_;
        int problemSize_;
        int size_;
        int scheduleNumber_;

        void clearList();

        void checkEquivalency();

        void finalize();

        void eliminate(double makespan);

        void eliminate(double makespan, double mintime);

        void change(int size, std::span<std::unique_ptr<Schedule>> schedules);

        void change(int size, std::span<std::unique_ptr<Schedule>> schedules, double makespan);

        void clearSchedules();

        friend class SubsetPool;
    };
}