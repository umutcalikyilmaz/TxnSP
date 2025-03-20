#include <PoolingModels/SchedulePool.h>
/*
namespace TransactionScheduling
{
    SchedulePool::SchedulePool(int n, int m, int max) : n(n), m(m), maxNum(max), inUse(0), inStock(0), total(0)
	{
		schedules = (Schedule*)malloc(max * sizeof(Schedule));
		list = new Schedule* [max];
	}

    Schedule* SchedulePool::GetSchedule(Schedule* sch)
	{
		if (inStock == 0)
		{
			if (inUse == maxNum)
			{
				throw "Maximum number of objects is reached.";
			}
			else
			{
				new(schedules + total) Schedule(sch);
				inUse++;
				total++;
				return schedules + total - 1;
			}
		}
		else
		{
			inUse++;
			inStock--;
			list[inStock]->Change(sch);
			return list[inStock];
		}
	}

	Schedule* SchedulePool::GetSchedule(Problem* prb, int job)
	{
		if (inStock == 0)
		{
			if (inUse == maxNum)
			{
				throw "Maximum number of objects is reached.";
			}
			else
			{
				new(schedules + total) Schedule(prb, job);
				inUse++;
				total++;
				return schedules + total - 1;
			}
		}
		else
		{
			inUse++;
			inStock--;
			list[inStock]->Change(prb, job);
			return list[inStock];
		}
	}

	Schedule* SchedulePool::GetSchedule(Problem* prb, __uint128_t index, int* perm, int* a)
	{
		if (inStock == 0)
		{
			if (inUse == maxNum)
			{
				throw "Maximum number of objects is reached.";
			}
			else
			{
				new(schedules + total) Schedule(prb, index, perm, a);
				inUse++;
				total++;
				return schedules + total - 1;
			}
		}
		else
		{
			inUse++;
			inStock--;
			list[inStock]->Change(prb, index, perm, a);
			return list[inStock];
		}
	}

	Schedule* SchedulePool::GetSchedule(Problem* prb, Schedule* sch, int job)
	{
		if (inStock == 0)
		{
			if (inUse == maxNum)
			{
				throw "Maximum number of objects is reached.";
			}
			else
			{
				new(schedules + total) Schedule(prb, sch, job);
				inUse++;
				total++;
				return schedules + total - 1;
			}
		}
		else
		{
			inUse++;
			inStock--;
			list[inStock]->Change(prb, sch, job);
			return list[inStock];
		}
	}

	Schedule* SchedulePool::GetSchedule(Problem* prb, int* state)
	{
		if (inStock == 0)
		{
			if (inUse == maxNum)
			{
				throw "Maximum number of objects is reached.";
			}
			else
			{
				new(schedules + total) Schedule(prb, state);
				inUse++;
				total++;
				return schedules + total - 1;
			}
		}
		else
		{
			inUse++;
			inStock--;
			list[inStock]->Change(prb, state);
			return list[inStock];
		}
	}

	void SchedulePool::ReturnSchedule(Schedule* sch)
	{
		list[inStock] = sch;
		inStock++;
		inUse--;
	}

	SchedulePool::~SchedulePool()
	{
		for (int i = 0; i < total; i++)
		{
			schedules[i].~Schedule();
		}

		free(schedules);
		delete[] list;
	}
}
*/

namespace TransactionScheduling
{
    SchedulePool::SchedulePool(int n, int m, int max) : n(n), m(m), inUse(0), schedules(max) { }

    Schedule* SchedulePool::GetSchedule(Schedule* sch)
    {
        inUse++;
        Schedule* res;

        if(schQueue.empty())
        {
            res = new Schedule(sch);
            schedules.push_back(res);            
        }
        else
        {
            res = schQueue.front();
            schQueue.pop();
            res->Change(sch);
        }

        return res;
    }

    Schedule* SchedulePool::GetSchedule(Problem* prb, int job)
    {
        inUse++;
        Schedule* res;

        if(schQueue.empty())
        {
            res = new Schedule(prb, job);
            schedules.push_back(res);            
        }
        else
        {
            res = schQueue.front();
            schQueue.pop();
            res->Change(prb, job);
        }

        return res;
    }

    Schedule* SchedulePool::GetSchedule(Problem* prb, Schedule* sch, int job)
    {
        inUse++;
        Schedule* res;

        if(schQueue.empty())
        {
            res = new Schedule(prb, sch, job);
            schedules.push_back(res);            
        }
        else
        {
            res = schQueue.front();
            schQueue.pop();
            res->Change(prb, sch, job);
        }

        return res;
    }

    Schedule* SchedulePool::GetSchedule(Problem* prb, __uint128_t index, int* perm, int* a)
    {
        inUse++;
        Schedule* res;

        if(schQueue.empty())
        {
            res = new Schedule(prb, index, perm, a);
            schedules.push_back(res);            
        }
        else
        {
            res = schQueue.front();
            schQueue.pop();
            res->Change(prb, index, perm, a);
        }

        return res;
    }

    Schedule* SchedulePool::GetSchedule(Problem* prb, int* state)
    {
        inUse++;
        Schedule* res;

        if(schQueue.empty())
        {
            res = new Schedule(prb, state);
            schedules.push_back(res);            
        }
        else
        {
            res = schQueue.front();
            schQueue.pop();
            res->Change(prb, state);
        }

        return res;
    }

    void SchedulePool::ReturnSchedule(Schedule* sch)
    {
        inUse--;
        schQueue.push(sch);
    }

    SchedulePool::~SchedulePool()
    {
		while(!schQueue.empty())
		{
			schQueue.pop();
		}        

		while(!schedules.empty())
		{
			delete schedules.back();
			schedules.pop_back();
		}
    }
}