#include "TxnSP/Solvers/DPSolver.h"

namespace TxnSP
{
    SolverOutput DPSolver::solveExact(Problem* prb)
    {
        double beg = std::chrono::steady_clock::now().time_since_epoch().count();

        if(prb->getJobNumber() <= prb->getMachineNumber())
        {
            double end = std::chrono::steady_clock::now().time_since_epoch().count();
            return SolverOutput(prb, (end - beg) / 1000000000);
        }

        std::vector<std::vector<std::unique_ptr<Subset>>> subsets(2);
        std::vector<uint8_t> blist(jobNumber_);

		for (int i = 0; i < jobNumber_; i++)
		{            	
			std::vector<std::unique_ptr<Schedule>> slist(1);
			slist[0] = move(schedulePool_->getSchedule(prb, i));
            subsets[0].push_back(move(subsetPool_->getSubset(1, slist)));
		}

		int oind;
		int nind;       

        for(int i = 2; i <= jobNumber_; i++)
        {
			int curNum = combination(jobNumber_, i);
			oind = i % 2;
			nind = 1 - oind;

            for (int j = 0; j < curNum; j++)
            {
                decode(blist, j, jobNumber_, i);
                double minimax = DBL_MAX;
				std::vector<std::unique_ptr<Schedule>> slist;

                for(int k = 0; k < jobNumber_; k++)
                {
                    if(blist[k])
                    {
                        blist[k] = 0;
                        int ind = encode(blist, jobNumber_, i - 1);
                        blist[k] = 1;

                        for(int l = 0; l < subsets[oind][ind]->getScheduleNumber(); l++)
                        {
                            std::unique_ptr<Schedule> temp = schedulePool_->getSchedule(prb,
                                subsets[oind][ind]->getSchedule(l), k);

                            if(temp->getMakespan() < minimax)
							{
								minimax = temp->getMakespan();
								slist.push_back(move(temp));
							}
							else if(temp->getMinimumTime() < minimax)
							{
								slist.push_back(move(temp));
							}
							else
							{
								schedulePool_->returnSchedule(move(temp));
							}
                        }
                    }
                }

                subsets[nind].push_back(subsetPool_->getSubset(i, slist, minimax));
            }

            for(auto& subset : subsets[oind])
            {
                subsetPool_->returnSubset(move(subset));
            }

            subsets[oind].clear();
        }

        double end = std::chrono::steady_clock::now().time_since_epoch().count();
        SolverOutput res(prb, subsets[nind][0]->getSchedule(0), (end - beg) / 1000000000);
		subsetPool_->returnSubset(move(subsets[nind][0]));

        subsetPool_ = nullptr;
        schedulePool_ = nullptr;

		return res;
    }

	SolverOutput DPSolver::solveApproximate(Problem* prb)
    {
        double beg = std::chrono::steady_clock::now().time_since_epoch().count();

        if(prb->getJobNumber() <= prb->getMachineNumber())
        {
            double end = std::chrono::steady_clock::now().time_since_epoch().count();
            return SolverOutput(prb, (end - beg) / 1000000000);
        }

        std::vector<std::vector<std::unique_ptr<Subset>>> subsets(2);
        std::vector<uint8_t> blist(jobNumber_);

		for (int i = 0; i < jobNumber_; i++)
		{            	
			std::vector<std::unique_ptr<Schedule>> slist(1);
			slist[0] = move(schedulePool_->getSchedule(prb, i));
            subsets[0].push_back(move(subsetPool_->getSubset(1, slist)));
		}

		int oind;
		int nind;

        for(int i = 2; i <= jobNumber_; i++)
        {
			int curNum = combination(jobNumber_, i);
			oind = i % 2;
			nind = 1 - oind;

            for (int j = 0; j < curNum; j++)
            {
                decode(blist, j, jobNumber_, i);
                double minimax = DBL_MAX;
				bool full = false;
				std::vector<std::unique_ptr<Schedule>> slist;

                for(int k = 0; k < jobNumber_; k++)
                {
                    if(blist[k])
                    {
                        blist[k] = 0;
                        int ind = encode(blist, jobNumber_, i - 1);                        
                        blist[k] = 1;

                        std::unique_ptr<Schedule> temp = std::move(schedulePool_->getSchedule(prb,
                            subsets[oind][ind]->getSchedule(0), k));

                        if (temp->getMakespan() < minimax)
						{
							if (full)
							{
								schedulePool_->returnSchedule(std::move(slist[0]));
                                slist[0] = move(temp);
							}
                            else
                            {
                                full = true;
                                minimax = temp->getMakespan();
                                slist.push_back(std::move(temp));
                            }							
						}
						else
						{
							schedulePool_->returnSchedule(std::move(temp));
						}                        
                    }
                }

                subsets[nind].push_back(subsetPool_->getSubset(i, slist)); 
            }

            for(auto& subset : subsets[oind])
            {
                subsetPool_->returnSubset(move(subset));
            }

            subsets[oind].clear();
        }

        double end = std::chrono::steady_clock::now().time_since_epoch().count();
        SolverOutput res(prb, subsets[nind][0]->getSchedule(0), (end - beg) / 1000000000);
		subsetPool_->returnSubset(std::move(subsets[nind][0]));

        subsetPool_ = nullptr;
        schedulePool_ = nullptr;

		return res;
    }

    SolverOutput DPSolver::solve(const SolverInput& input)
    {
        jobNumber_ = input.prb->getJobNumber();
        machineNumber_ = input.prb->getMachineNumber();
        type_ = input.DP_SolutionType;
        schedulePool_ = std::make_unique<SchedulePool>(jobNumber_, machineNumber_);
        subsetPool_ = std::make_unique<SubsetPool>(jobNumber_, schedulePool_.get());

        return type_ == SolutionType::Approximate ? solveApproximate(input.prb) : solveExact(input.prb);
    }
}