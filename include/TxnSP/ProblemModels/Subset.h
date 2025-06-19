#pragma once
#include "TxnSP/PoolingModels/SchedulePool.h"

namespace TxnSP
{
    class Subset
    {
    public:

		int getScheduleNumber();

        Schedule* getSchedule(int ind);

        friend class SubsetPool;

    private:

        Schedule** list_;
        SchedulePool* schedulePool_;
        int problemSize_;
        int size_;
        int scheduleNumber_;

        void clearList();

        void checkEquivalency();

        void finalize();

        void eliminate(double makespan);

        void eliminate(double makespan, double mintime);      

        Subset(int problemSize, int size, int scheduleNumber, Schedule** schedules, SchedulePool* schedulePool);

        Subset(int problemSize, int size, int scheduleNumber, Schedule** schedules, double makespan, SchedulePool* schedulePool);

        void change(int size, int scheduleNumber, Schedule** schedules);

        void change(int size, int scheduleNumber, double makespan, Schedule** schedules); 

        ~Subset();
    };
}