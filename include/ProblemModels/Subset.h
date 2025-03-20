#pragma once
#include <PoolingModels/SchedulePool.h>
namespace TransactionScheduling
{
    class Subset
    {
        Schedule** list;
		SchedulePool* schp;
        int psize;
		int size;
		int schNum;
		int schSize;

        void ClearList();

        void CheckEquivalency();

        void Finalize();

        void Eliminate(double makespan);

        void Eliminate(double makespan, double mintime);      

        Subset(int psize, int size, int schNum, Schedule** sch, SchedulePool* schp);

        Subset(int psize, int size, int schNum, Schedule** sch, double makespan, SchedulePool* sp);

        void Change(int psize, int size, int schNum, Schedule** sch);

        void Change(int psize, int size, int schNum, double makespan, Schedule** sch); 

        ~Subset();

    public:

		int GetScheduleNum();

        Schedule* GetSchedule(int ind);

        friend class SubsetPool;
    };
}