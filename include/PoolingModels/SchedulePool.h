#pragma once
#include <queue>
#include <vector>
#include <ProblemModels/Schedule.h>

/*
namespace TransactionScheduling
{
    class SchedulePool
    {
    private:
        int n;
		int m;
		Schedule* schedules;
		Schedule** list;
		int maxNum;
		int inUse;
		int inStock;
		int total;

    public:

        SchedulePool(int n, int m, int max);

        Schedule* GetSchedule(Schedule* sch);

        Schedule* GetSchedule(Problem* prb, int job);

        Schedule* GetSchedule(Problem* prb, Schedule* sch, int job);

        Schedule* GetSchedule(Problem* prb, __uint128_t index, int* perm, int* a);

        Schedule* GetSchedule(Problem* prb, int* state);

        void ReturnSchedule(Schedule* sch);

        ~SchedulePool();
    };    
}
*/

namespace TransactionScheduling
{
    class SchedulePool
    {        
		vector<Schedule*> schedules;
		queue<Schedule*> schQueue;
		int inUse;
        int n;
		int m;

    public:

        SchedulePool(int n, int m, int max);

        Schedule* GetSchedule(Schedule* sch);

        Schedule* GetSchedule(Problem* prb, int job);

        Schedule* GetSchedule(Problem* prb, Schedule* sch, int job);

        Schedule* GetSchedule(Problem* prb, __uint128_t index, int* perm, int* a);

        Schedule* GetSchedule(Problem* prb, int* state);

        void ReturnSchedule(Schedule* sch);

        ~SchedulePool();
    };    
}