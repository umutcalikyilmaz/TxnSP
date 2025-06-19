#include "TxnSP/Solvers/DPSolver.h"

namespace TxnSP
{
    SolverOutput* DPSolver::solveExact(Problem* prb)
    {
        double beg = std::chrono::steady_clock::now().time_since_epoch().count();

        if(prb->getJobNumber() <= prb->getMachineNumber())
        {
            double end = std::chrono::steady_clock::now().time_since_epoch().count();
            SolverOutput* res = new SolverOutput(prb, (end - beg) / 1000000000);
        }

        Subset** subsets[2];
		subsets[0] = new Subset*[subsetNumber_];
		subsets[1] = new Subset*[subsetNumber_];
        bool* blist = new bool[jobNumber_];

		for (int i = 0; i < jobNumber_; i++)
		{            	
			Schedule** slist = new Schedule*[1];
			slist[0] = schedulePool_->getSchedule(prb, i);
            subsets[0][i] = subsetPool_->getSubset(1, 1, slist);
		}

        int maxSch = 1;
        int maxNum;
		int oind;
		int nind;
		int preNum;
		int curNum = jobNumber_;        

        for(int i = 2; i <= jobNumber_; i++)
        {            
            maxNum = maxSch * i;
			maxSch = 0;
			preNum = curNum;
			curNum = combination(jobNumber_, i);
			oind = i % 2;
			nind = 1 - oind;

            for (int j = 0; j < curNum; j++)
            {
                decode(blist, j, jobNumber_, i);
                double minimax = 1.7976931e+308;
				int schCount = 0;
				Schedule** slist = new Schedule*[maxNum];

                for(int k = 0; k < jobNumber_; k++)
                {
                    if(blist[k])
                    {
                        blist[k] = false;
                        int ind = encode(blist, jobNumber_, i - 1);
                        blist[k] = true;
                        Subset* currentSS = subsets[oind][ind];

                        for(int l = 0; l < currentSS->getScheduleNumber(); l++)
                        {
                            Schedule* temp = schedulePool_->getSchedule(prb, currentSS->getSchedule(l), k);

                            if(temp->getMakespan() < minimax)
							{
								minimax = temp->getMakespan();
								slist[schCount] = temp;
								schCount++;
							}
							else if(temp->getMinimumTime() < minimax)
							{
								slist[schCount] = temp;
								schCount++;
							}
							else
							{
								schedulePool_->returnSchedule(temp);
							}
                        }
                    }
                }

                subsets[nind][j] = subsetPool_->getSubset(i, schCount, slist, minimax);

				if (schCount > maxSch)
				{
					maxSch = schCount;
				}
            }

            for (int x = 0; x < preNum; x++)
			{
				subsetPool_->returnSubset(subsets[oind][x]);
			}
        }

        double end = std::chrono::steady_clock::now().time_since_epoch().count();
        SolverOutput* res = new SolverOutput(prb, subsets[nind][0]->getSchedule(0), (end - beg) / 1000000000);
		subsetPool_->returnSubset(subsets[nind][0]);

		delete[] blist;
        delete[] subsets[0];
        delete[] subsets[1];

		return res;
    }

	SolverOutput* DPSolver::solveApproximate(Problem* prb)
    {
        double beg = std::chrono::steady_clock::now().time_since_epoch().count();

        if(prb->getJobNumber() <= prb->getMachineNumber())
        {
            double end = std::chrono::steady_clock::now().time_since_epoch().count();
            return new SolverOutput(prb, (end - beg) / 1000000000);
        }

        bool* blist = new bool[jobNumber_];
        Subset** subsets[2];
		subsets[0] = new Subset*[subsetNumber_];
		subsets[1] = new Subset*[subsetNumber_];

        for(int i = 0; i < jobNumber_; i++)
        {
            blist[i] = false;
        }

		for (int i = 0; i < jobNumber_; i++)
		{            	
			Schedule** slist = new Schedule*[1];
			slist[0] = schedulePool_->getSchedule(prb, i);
            subsets[0][i] = subsetPool_->getSubset(1, 1, slist);
		}

        int maxSch = 1;
        int maxNum;
		int oind;
		int nind;
		int preNum;
		int curNum = jobNumber_;

        for(int i = 2; i <= jobNumber_; i++)
        {
			preNum = curNum;
			curNum = combination(jobNumber_, i);
			oind = i % 2;
			nind = 1 - oind;

            for (int j = 0; j < curNum; j++)
            {
                decode(blist, j, jobNumber_, i);
                double minimax = 1.7976931e+308;
				bool full = false;
				Schedule** slist = new Schedule*[1];

                for(int k = 0; k < jobNumber_; k++)
                {
                    if(blist[k])
                    {
                        blist[k] = false;
                        int ind = encode(blist, jobNumber_, i - 1);                        
                        blist[k] = true;
                        
                        Subset* currentSS = subsets[oind][ind];
                        Schedule* temp = schedulePool_->getSchedule(prb, currentSS->getSchedule(0), k);

                        if (temp->getMakespan() < minimax)
						{
							if (full)
							{
								schedulePool_->returnSchedule(*slist);
							}

							full = true;
							*slist = temp;
							minimax = temp->getMakespan();
						}
						else
						{
							schedulePool_->returnSchedule(temp);
						}                        
                    }
                }

                subsets[nind][j] = subsetPool_->getSubset(i, 1, slist);                
            }

            for (int x = 0; x < preNum; x++)
			{
				subsetPool_->returnSubset(subsets[oind][x]);
			}           
        }

        double end = std::chrono::steady_clock::now().time_since_epoch().count();
        SolverOutput* res = new SolverOutput(prb, subsets[nind][0]->getSchedule(0), (end - beg) / 1000000000);
		subsetPool_->returnSubset(subsets[nind][0]);

		delete[] blist;
        delete[] subsets[0];
        delete[] subsets[1];

		return res;
    }

    DPSolver::DPSolver() : jobNumber_(0), machineNumber_(0), init_(false) { }

    SolverOutput* DPSolver::solve(const SolverInput& input)
    {
        if(input.prb->getJobNumber() != jobNumber_ || input.prb->getMachineNumber() != machineNumber_ || input.DP_SolutionType != type_)
        {
            jobNumber_ = input.prb->getJobNumber();
            machineNumber_ = input.prb->getMachineNumber();
            type_ = input.DP_SolutionType;
            subsetNumber_ = combination(jobNumber_, jobNumber_ / 2);
            scheduleNumber_ = type_ == Approximate ? 2.5 * subsetNumber_ : log(jobNumber_) * pow(jobNumber_ * subsetNumber_, 2) / factorial(machineNumber_ - 1);

            if(init_)
            {                
                delete subsetPool_;
                delete schedulePool_;
            }

            schedulePool_ = new SchedulePool(jobNumber_, machineNumber_, scheduleNumber_);
            subsetPool_ = new SubsetPool(jobNumber_, subsetNumber_, schedulePool_);
            init_ = true;
        }

        return type_ == Approximate ? solveApproximate(input.prb) : solveExact(input.prb);
    }

    DPSolver::~DPSolver()
    {
        if(init_)
        {
            delete schedulePool_;
            delete subsetPool_;
        }
    }
}