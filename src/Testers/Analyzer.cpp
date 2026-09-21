#include "TxnSP/Testers/Analyzer.h"

namespace TxnSP
{
    bool Analyzer::pullProblem(std::unique_ptr<Problem>& prb, int& ind)
    {
        std::lock_guard<std::mutex> lock(problemLock_);

        if(problemQueue_.empty())
        {
            return false;
        }

        prb = std::move(problemQueue_.front());
        ind = indexQueue_.front();
        problemQueue_.pop();
        indexQueue_.pop();
        return true;
    }

    void Analyzer::threadFunction(int n, int m, std::vector<int>& indices, std::vector<double>& spaceSizes,
        std::vector<double>& optNums, std::vector<double>& per1, std::vector<double>& per5,
        std::vector<double>& per10, std::vector<double>& per20)
    {
        std::vector<int> perm(n);
		std::vector<int> a(n);
        std::unique_ptr<Problem> prb;
        int i = 0;
        int ind;

        while(pullProblem(prb, ind))
        {
            SchedulePool schp(n, m);
            std::unique_ptr<Schedule> sch = schp.getSchedule(prb.get(), 0, perm, a);
            double opt = (double)((__uint128_t)(1000000 * sch->getMakespan())) / 1000000;

            std::set<double> valueSet{ opt };
            std::map<double,int> countMap;
            countMap[opt] = 1;
            LargeInt size = prb->getSize();

            for(LargeInt j = 1; j < size; j++)
            {
                schp.returnSchedule(std::move(sch));
                sch = schp.getSchedule(prb.get(), j, perm, a);
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

            per1.push_back(0);
            per5.push_back(0);
            per10.push_back(0);
            per20.push_back(0);

            double lim1 = opt * 1.01;
            double lim5 = opt * 1.05;
            double lim10 = opt * 1.1;
            double lim20 = opt * 1.2;

            std::set<double>::iterator itr;

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
            optNums.push_back(countMap[opt]);
            spaceSizes.push_back((double)valueSet.size());
            indices.push_back(ind);
            i++;
        }
    }

    void Analyzer::analyze(const AnalyzerInput& inp)
    {        
        double para1 = inp.distributionParameter1;
        double para2 = inp.distributionParameter2;
        double cpStep = inp.conflictParityStepSize;
        int prbNum = inp.problemNumber;
        int stepNum = 1 + (int)(1 / cpStep);
        int totPrbNum = prbNum * stepNum;
        int n = inp.jobNumber;
        int m = inp.machineNumber;
        int threadCount = inp.threadCount;
        int threadStep = totPrbNum / threadCount;
        ProbabilityDistribution dist = inp.distribution;
        
        SchedulePool schp(n, m);
        std::vector<std::jthread> threads;
        std::vector<std::vector<int>> indices(threadCount);
        std::vector<std::vector<double>> spaceSizes(threadCount);
        std::vector<std::vector<double>> optNums(threadCount);
        std::vector<std::vector<double>> per1(threadCount);
        std::vector<std::vector<double>> per5(threadCount);
        std::vector<std::vector<double>> per10(threadCount);
        std::vector<std::vector<double>> per20(threadCount);
        std::vector<double> cps(stepNum);
        std::vector<double> finalSize(stepNum);
        std::vector<double> finalOptNum(stepNum);
        std::vector<double> finalPer1(stepNum);
        std::vector<double> finalPer5(stepNum);
        std::vector<double> finalPer10(stepNum);
        std::vector<double> finalPer20(stepNum);

        int c = 0;
        double cp = 0;
        int threadInd = 0;

        while(cp <= 1)
        {
            cps[c] = cp;

            for(int i = 0; i < prbNum; i++)
            {
                problemQueue_.emplace(std::make_unique<Problem>(n, m, dist, para1, para2, cp));
                indexQueue_.push(c);       
            }

            cp += cpStep;
            c++;

            if(cp > 1 && cp < 1.000001)
            {
                cp = 1;
            }
        }

        double prbSize = (double)problemQueue_.front()->getSize();

        for(int i = 0; i < threadCount; i++)
        {
            threads.emplace_back(std::jthread([this, nn = n, mm = m, &ind = indices[i], &spc = spaceSizes[i],
                &opt = optNums[i], &p1 = per1[i], &p5 = per5[i], &p10 = per10[i], &p20 = per20[i]]()
            {
                this->threadFunction(nn, mm, ind, spc, opt, p1, p5, p10, p20);
            }));
        }

        threads.clear();

        const std::filesystem::path filePath = std::filesystem::path(std::getenv("HOME")) / ".TxnSP";
        std::filesystem::create_directory(filePath);
        std::filesystem::create_directory(filePath / "analysis");

        std::fstream file;
        std::stringstream sstream;
        sstream.setf(std::ios::fixed);
        sstream.precision(2);
        sstream << n << "_" << m << "_" << (dist == ProbabilityDistribution::Normal ? "n" : "u")
        << "_" << para1 << "_" << para2 << "_" << cpStep << "_" << prbNum << "_" << ".csv";
        file.open(filePath / "analysis" / sstream.str(), std::fstream::out | std::fstream::trunc);

        sstream.str(std::string());
        sstream.precision(3);

        for(int i = 0; i < threadCount; i++)
        {
            for(int j = 0; j < spaceSizes[i].size(); j++)
            {
                int ind = indices[i][j];
                finalSize[ind] += spaceSizes[i][j];
                finalOptNum[ind] += optNums[i][j];
                finalPer1[ind] += per1[i][j];
                finalPer5[ind] += per5[i][j];
                finalPer10[ind] += per10[i][j];
                finalPer20[ind] += per20[i][j];
            }            
        }

        for(int i = 0; i < stepNum; i++)
        {
            finalSize[i] /= prbNum;
            finalOptNum[i] /= prbNum;
            finalPer1[i] /= prbNum;
            finalPer5[i] /= prbNum;
            finalPer10[i] /= prbNum;
            finalPer20[i] /= prbNum;

            sstream << cps[i] << "," << finalSize[i] << "," << std::to_string(finalSize[i] / (double)prbSize)
            << "," << finalOptNum[i] << "," << std::to_string(finalOptNum[i] / (double)prbSize) << "," <<
            finalPer1[i] << "," << std::to_string(finalPer1[i] / (double)prbSize) << "," << finalPer5[i]
            << "," << std::to_string(finalPer5[i] / (double)prbSize) << "," << finalPer10[i] << "," << 
            std::to_string(finalPer10[i] / (double)prbSize) << "," << finalPer20[i] << "," <<
            std::to_string(finalPer20[i] / (double)prbSize) << "\n";

            file << sstream.str();
            sstream.str(std::string());
            file.flush();
        }

        file.close();
    }
}