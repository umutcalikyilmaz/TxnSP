#pragma once
#include <map>
#include "TxnSP/ProblemModels/Problem.h"
#include "TxnSP/Shared/SharedFunctions.h"

namespace TxnSP
{
    class Schedule
    {
    public:    
    
        Schedule(Schedule* sch);

        Schedule(Problem* prb, int job);

        Schedule(Problem* prb, Schedule* sch, int job);

        Schedule(Problem* prb, LargeInt index, std::vector<int>& perm, std::vector<int>& a);

        Schedule(Problem* prb, const std::vector<int>& state);

        int getJobNumber() const;

		int getMachineNumber() const;

		int getSize() const;

		double getMakespan() const;

		double getMinimumTime() const;

		int getMinimumMachine() const;

		const std::vector<std::vector<int>>& getJobs() const;

		const std::vector<double>& getProcessingTimes() const;

		const std::vector<int>& getLastJobs() const;

        bool isEquivalent(Schedule* sch);

    private:

        std::vector<double> processingTimes_;
        std::vector<std::vector<int>> jobs_;
        std::vector<int> lastJobs_;
        std::vector<int> order_;
        double makespan_;
        double minimumTime_;
        int minimumMachine_;
        int jobNumber_;
        int machineNumber_;
        int size_;

        void initialize(Problem* prb, const std::vector<int>& state);

        void change(Schedule* sch);

        void change(Problem* prb, int job);

        void change(Problem* prb, Schedule* sch, int job);

        void change(Problem* prb, LargeInt index, std::vector<int>& perm, std::vector<int>& a);

        void change(Problem* prb, const std::vector<int>& state);

        friend class SchedulePool;
    };
}