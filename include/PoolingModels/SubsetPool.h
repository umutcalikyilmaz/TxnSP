#pragma once
#include <ProblemModels/Subset.h>

namespace TransactionScheduling
{
    class SubsetPool
    {        
		vector<Subset*> subsets;
		queue<Subset*> ssQueue;
        SchedulePool* schp;
        int psize;
		int inUse;
		
    public:

        SubsetPool(int size, int max, SchedulePool* schp);

        Subset* GetSubset(int psize, int size, int schNum, Schedule** sch);

        Subset* GetSubset(int psize, int size, int schNum, Schedule** sch, double makespan);

        void ReturnSubset(Subset* ss);

        ~SubsetPool();
    };
}