#include "TxnSP/Testers/Analyzer.h"

namespace TxnSP
{
    void ThreadFunction(Problem** problems, int n, int m, int prbNum, double* spaceSizes, double* optNums, double* per1, double* per5, double* per10, double* per20)
    {
        int* perm = new int[n];
		int* a = new int[n];
        __uint128_t size = problems[0]->getSize();

        for(int i = 0; i < prbNum; i++)
        {
            Problem* prb = problems[i];            
            SchedulePool schp(n, m, 1);
                       
            Schedule* sch = schp.getSchedule(prb, 0, perm, a);
            double opt = (double)((__uint128_t)(1000000 * sch->getMakespan())) / 1000000;
            set<double> valueSet{ opt };
            map<double,int> countMap;
            countMap[opt] = 1;
            

            for(__uint128_t j = 1; j < size; j++)
            {
                schp.returnSchedule(sch);
                sch = schp.getSchedule(prb, j, perm, a);
                double ms = (double)((__uint128_t)(1000000 * sch->getMakespan())) / 1000000;

                if(valueSet.find(ms) == valueSet.end())
                {
                    valueSet.insert(ms);
                    countMap[ms] = 1;

                    if(ms < opt)
                    {
                        opt = ms;                        
                    }                    
                }
                else
                {
                    countMap[ms]++;
                }
            }

            per1[i] = 0;
            per5[i] = 0;
            per10[i] = 0;
            per20[i] = 0;

            double lim1 = opt * 1.01;
            double lim5 = opt * 1.05;
            double lim10 = opt * 1.1;
            double lim20 = opt * 1.2;

            set<double>::iterator itr;

            for (itr = valueSet.begin(); itr != valueSet.end(); itr++) 
            {
                if(*itr < lim1)
                {
                    per1[i] += countMap[*itr];
                }
                else if(*itr < lim5)
                {
                    per5[i] += countMap[*itr];
                }
                else if(*itr < lim10)
                {
                    per10[i] += countMap[*itr];
                }
                else if(*itr < lim20)
                {
                    per20[i] += countMap[*itr];
                }                
            }

            per5[i] += per1[i];
            per10[i] += per5[i];
            per20[i] += per10[i];
            optNums[i] = countMap[opt];
            spaceSizes[i] = (double)valueSet.size();
            delete prb;
        }

        delete[] perm;
        delete[] a;
    }

    void Analyzer::analyze(const AnalyzerInput& inp)
    {        
        double para1 = inp.distributionParameter1;
        double para2 = inp.distributionParameter2;
        double cpStep = inp.conflictParityStepSize;
        int prbNum = inp.problemNumber;
        int stepNum = 1 + (int)(1 / cpStep);
        int totPrbNum = prbNum * (stepNum);
        int n = inp.jobNumber;
        int m = inp.machineNumber;
        int threadCount = inp.threadCount;
        int threadStep = totPrbNum / threadCount;
        ProbabilityDistribution dist = inp.distribution;
        
        SchedulePool schp(n, m, 1);
        thread* threads = new thread[threadCount];
        Problem*** problems = new Problem**[stepNum];
        Problem*** thProblems = new Problem**[threadCount];        
        double** spaceSizes = new double*[threadCount];
        double** optNums = new double*[threadCount];
        double** per1 = new double*[threadCount];
        double** per5 = new double*[threadCount];
        double** per10 = new double*[threadCount];
        double** per20 = new double*[threadCount];
        double* cps = new double[stepNum];
        int* prbNums = new int[threadCount];        
        double* finalSize = new double[stepNum];
        double* finalOptNum = new double[stepNum];
        double* finalPer1 = new double[stepNum];
        double* finalPer5 = new double[stepNum];
        double* finalPer10 = new double[stepNum];
        double* finalPer20 = new double[stepNum];

        for(int i = 0; i < threadCount - 1; i++)
        {
            prbNums[i] = threadStep;
            spaceSizes[i] = new double[threadStep];
            optNums[i] = new double[threadStep];
            per1[i] = new double[threadStep];
            per5[i] = new double[threadStep];
            per10[i] = new double[threadStep];
            per20[i] = new double[threadStep];
        }

        prbNums[threadCount - 1] = totPrbNum - (threadCount - 1) * threadStep;
        spaceSizes[threadCount - 1] = new double[prbNums[threadCount - 1]];
        optNums[threadCount - 1] = new double[prbNums[threadCount - 1]];
        per1[threadCount - 1] = new double[prbNums[threadCount - 1]];
        per5[threadCount - 1] = new double[prbNums[threadCount - 1]];
        per10[threadCount - 1] = new double[prbNums[threadCount - 1]];
        per20[threadCount - 1] = new double[prbNums[threadCount - 1]];

        int c = 0;
        double cp = 0;
        int threadInd = 0;
        int count = 0;
        thProblems[0] = new Problem*[prbNums[0]];

        while(cp <= 1)
        {
            problems[c] = new Problem*[prbNum];
            cps[c] = cp;

            for(int i = 0; i < prbNum; i++)
            {
                problems[c][i] = new Problem(n, m, dist, para1, para2, cp);
                thProblems[threadInd][count] = problems[c][i];
                count++;    

                if(count == threadStep && threadInd != threadCount - 1)
                {
                    count = 0;
                    threadInd++;
                    thProblems[threadInd] = new Problem*[prbNums[threadInd]];
                }            
            }

            cp += cpStep;
            c++;

            if(cp > 1 && cp < 1.000001)
            {
                cp = 1;
            }
        }

        double prbSize = (double)problems[0][0]->getSize();

        for(int i = 0; i < threadCount; i++)
        {
            threads[i] = thread(ThreadFunction, thProblems[i], n, m, prbNums[i], spaceSizes[i], optNums[i], per1[i], per5[i], per10[i], per20[i]);
        }

        for(int i = 0; i < threadCount; i++)
        {
            threads[i].join();
            delete[] thProblems[i];            
        }

        threadInd = 0;
        count = 0;

        fstream file;
        stringstream sstream;
        sstream.setf(std::ios::fixed);
        sstream.precision(2);
        sstream << inp.directory << n << "_" << m << "_" << (dist == ProbabilityDistribution::Normal ? "n" : "u") << "_" << para1 << "_" << para2 << "_" << cpStep << "_" << prbNum << "_" << ".csv";
        file.open(sstream.str(), fstream::out | fstream::trunc);
        
        sstream.str(string());
        sstream.precision(3);

        for(int i = 0; i < stepNum; i++)
        {
            finalSize[i] = 0;
            delete[] problems[i];

            for(int j = 0; j < prbNum; j++)
            {
                finalSize[i] += spaceSizes[threadInd][count];
                finalOptNum[i] += optNums[threadInd][count];
                finalPer1[i] += per1[threadInd][count];
                finalPer5[i] += per5[threadInd][count];
                finalPer10[i] += per10[threadInd][count];
                finalPer20[i] += per20[threadInd][count];
                count++;

                if(count == threadStep && threadInd != threadCount - 1)
                {
                    delete[] spaceSizes[threadInd];
                    delete[] optNums[threadInd];
                    delete[] per1[threadInd];
                    delete[] per5[threadInd];
                    delete[] per10[threadInd];
                    delete[] per20[threadInd];
                    threadInd ++;
                    count = 0;
                }                
            }

            finalSize[i] /= prbNum;
            finalOptNum[i] /= prbNum;
            finalPer1[i] /= prbNum;
            finalPer5[i] /= prbNum;
            finalPer10[i] /= prbNum;
            finalPer20[i] /= prbNum;

            sstream << cps[i] << "," << finalSize[i] << "," << to_string(finalSize[i] / (double)prbSize) << "," << finalOptNum[i] << "," << 
            to_string(finalOptNum[i] / (double)prbSize) << "," << finalPer1[i] << "," << to_string(finalPer1[i] / (double)prbSize) << "," << 
            finalPer5[i] << "," << to_string(finalPer5[i] / (double)prbSize) << "," << finalPer10[i] << "," << 
            to_string(finalPer10[i] / (double)prbSize) << "," << finalPer20[i] << "," << to_string(finalPer20[i] / (double)prbSize) << "\n";

            file << sstream.str();
            sstream.str(string());
            file.flush();
        }

        delete[] spaceSizes[threadCount - 1];
        delete[] optNums[threadCount - 1];
        delete[] per1[threadCount - 1];
        delete[] per5[threadCount - 1];
        delete[] per10[threadCount - 1];
        delete[] per20[threadCount - 1];
        delete[] problems;
        delete[] thProblems;
        delete[] cps;
        delete[] prbNums;
        delete[] threads;
        delete[] finalSize;
        delete[] spaceSizes;
        delete[] optNums;
        delete[] per1;
        delete[] per5;
        delete[] per10;
        delete[] per20;
        delete[] finalOptNum;
        delete[] finalPer1;
        delete[] finalPer5;
        delete[] finalPer10;
        delete[] finalPer20;

        file.close();
    }
}