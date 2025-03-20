#pragma once
#include <map>
#include <ProblemModels/Problem.h>
#include <Shared/Decode.h>


namespace TransactionScheduling
{
    class Schedule
    {
        double* coreTimes;
        int** cores;
        int* lastJobs;	
        int* coreNums;
        int* order;
        double makespan;
        double mintime;
        int minimumCore;
        int n;
        int m;
        int size;
        bool del;

        Schedule(Schedule* sch);        

        Schedule(Problem* prb, int job);

        Schedule(Problem* prb, Schedule* sch, int job);

        Schedule(Problem* prb, __uint128_t index, int* perm, int* a);

        Schedule(Problem* prb, int* state);

        void Change(Schedule* sch);

        void Change(Problem* prb, int job);

        void Change(Problem* prb, Schedule* sch, int job);

        void Change(Problem* prb, __uint128_t index, int* perm, int* a);

        void Change(Problem* prb, int* state);

        ~Schedule();

    public:        

        int GetN() const;

		int GetM() const;

		int GetSize() const;

		double GetMakespan() const;

		double GetMintime() const;

		int GetMinimumCore() const;

		int** GetJobs() const;

		double* GetCoreTimes() const;

		int* GetLastJobs() const;

		int* GetCoreNums() const;

        bool IsEquivalent(Schedule* sch);

        friend class SchedulePool;
    };
}