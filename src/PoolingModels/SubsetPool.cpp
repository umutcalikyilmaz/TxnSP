#include <PoolingModels/SubsetPool.h>

namespace TransactionScheduling
{
    SubsetPool::SubsetPool(int size, int max, SchedulePool* schp) : psize(size), inUse(0), schp(schp), subsets(max) { }

    Subset* SubsetPool::GetSubset(int psize, int size, int schNum, Schedule** sch)
    {
        inUse++;
        Subset* res;

        if(ssQueue.empty())
        {
            res = new Subset(psize, size, schNum, sch, schp);
            subsets.push_back(res);
        }
        else
        {
            res = ssQueue.front();
            ssQueue.pop();
            res->Change(psize, size, schNum, sch);
        }

        return res;
    }

    Subset* SubsetPool::GetSubset(int psize, int size, int schNum, Schedule** sch, double makespan)
    {
        inUse++;
        Subset* res;

        if(ssQueue.empty())
        {
            res = new Subset(psize, size, schNum, sch, makespan, schp);
            subsets.push_back(res);
        }
        else
        {
            res = ssQueue.front();
            ssQueue.pop();
            res->Change(psize, size, schNum, makespan, sch);
        }

        return res;
    }

    void SubsetPool::ReturnSubset(Subset* ss)
    {
        inUse--;
        ssQueue.push(ss);
    }

    SubsetPool::~SubsetPool()
    {
        while (ssQueue.empty())
        {
            ssQueue.pop();
        }

        while (!subsets.empty())
        {
            delete subsets.back();
            subsets.pop_back();
        }
    }
}