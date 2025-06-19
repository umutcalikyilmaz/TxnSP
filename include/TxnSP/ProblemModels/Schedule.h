#pragma once
#include <map>
#include "TxnSP/ProblemModels/Problem.h"
#include "TxnSP/Shared/SharedFunctions.h"

namespace TxnSP
{
    class Schedule
    {
    public:        

        int getJobNumber() const;

		int getMachineNumber() const;

		int getSize() const;

		double getMakespan() const;

		double getMinimumTime() const;

		int getMinimumMachine() const;

		int** getJobs() const;

		double* getProcessingTimes() const;

		int* getLastJobs() const;

		int* getJobNumbers() const;

        bool isEquivalent(Schedule* sch);

        friend class SchedulePool;

    private:

        double* processingTimes_;
        int** jobs_;
        int* lastJobs_;	
        int* jobNumbers_;
        int* order_;
        double makespan_;
        double minimumTime_;
        int minimumMachine_;
        int jobNumber_;
        int machineNumber_;
        int size_;
        bool del_;

        Schedule(Schedule* sch);

        Schedule(Problem* prb, int job);

        Schedule(Problem* prb, Schedule* sch, int job);

        Schedule(Problem* prb, __uint128_t index, int* perm, int* a);

        Schedule(Problem* prb, int* state);

        void change(Schedule* sch);

        void change(Problem* prb, int job);

        void change(Problem* prb, Schedule* sch, int job);

        void change(Problem* prb, __uint128_t index, int* perm, int* a);

        void change(Problem* prb, int* state);

        ~Schedule();
    };
}