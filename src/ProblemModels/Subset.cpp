#include <ProblemModels/Subset.h>

namespace TransactionScheduling
{
    void Subset::ClearList()
	{
        for(int i = 0; i < schNum; i++)
        {
            schp->ReturnSchedule(list[i]);
        }
	}

    void Subset::CheckEquivalency()
    {
        int ind = 0;

		while (ind < schNum)
		{
			int ind2 = ind + 1;
			int move = 0;

			while (ind2 < schNum)
			{
				if (list[ind]->IsEquivalent(list[ind2]))
				{
					move++;
					schp->ReturnSchedule(list[ind2]);
				}
				else if (move > 0)
				{
					list[ind2 - move] = list[ind2];
				}

				ind2++;
			}

			ind++;
			schNum -= move;
		}
    }

    void Subset::Finalize()
    {
        double min = list[0]->GetMakespan();
		int ind = 0;

		for (int i = 1; i < schNum; i++)
		{
			if (list[i]->GetMakespan() < min)
			{
				schp->ReturnSchedule(list[ind]);
				ind = i;
				min = list[i]->GetMakespan();
			}
			else
			{
				schp->ReturnSchedule(list[i]);
			}
		}

		Schedule* temp = list[ind];
		delete[] list;
		list = new Schedule*[1](temp);
		schNum = 1;
    }

    void Subset::Eliminate(double makespan)
    {
        int move = 0;

		for (int i = 0; i < schNum; i++)
		{
			if (list[i]->GetMintime() >= makespan)
			{
				move++;
				schp->ReturnSchedule(list[i]);
			}
			else
			{
				list[i - move] = list[i];
			}
		}

		schNum -= move;

		if (size <= list[0]->GetM())
		{
			CheckEquivalency();
		}
    }

    Subset::Subset(int psize, int size, int schNum, Schedule** sch, SchedulePool* schp) : psize(psize), size(size), schNum(schNum), schSize(schNum), list(sch), schp(schp) { }

    Subset::Subset(int psize, int size, int schNum, Schedule** sch, double makespan, SchedulePool* schp) : psize(psize), size(size), schNum(schNum), schSize(schNum), list(sch), schp(schp)
    {
        if (size == psize)
		{
			Finalize();
		}
		else
		{
			Eliminate(makespan);
		}
    }

    void Subset::Change(int psize, int size, int schNum, Schedule** sch)
    {   
        ClearList();
        delete[] list;
        list = sch;
		this->size = size;
        this->schNum = schNum;
        this->schSize = schNum;
		list = sch;
    }

	void Subset::Change(int psize, int size, int schNum, double makespan, Schedule** sch)
	{
		ClearList();
		delete[] list;
		list = sch;
		this->size = size;
		this->schNum = schNum;
		this->schSize = schNum;

		if (size == psize)
		{
			Finalize();
		}
		else
		{
			Eliminate(makespan);
		}
	}

    int Subset::GetScheduleNum()
    {
        return schNum;
    }

    Schedule* Subset::GetSchedule(int ind)
    {
        return list[ind];
    }

    Subset::~Subset()
    {
        //ClearList();
        delete[] list;
    }
}