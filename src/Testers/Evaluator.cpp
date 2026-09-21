#include "TxnSP/Testers/Evaluator.h"

namespace TxnSP
{
    bool Evaluator::pullProblem(Problem*& prb, int& ind)
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

    void Evaluator::threadFunction(bool es, bool mip, bool dp_exact,bool dp_approximate,
        const std::vector<std::pair<TemperatureEvolution,double>>& SA_DecrementTypesAndParameters, 
        const std::vector<double>& SA_MaxTemperatures, std::vector<int>& indices,
        std::vector<double>& dpeTime, std::vector<double>& dpeVal, std::vector<double>& dpaTime,
        std::vector<double>& dpaVal, std::vector<double>& esTime, std::vector<double>& esVal,
        std::vector<double>& mipTime, std::vector<double>& mipVal,
        std::vector<std::vector<double>>& saTime, std::vector<std::vector<double>>& saVal)
    {
        
        DPSolver dps;
        ESSolver ess;
        SASolver sas;
        SolverInput inp;

        #ifdef ENABLE_MIP
        MIPSolver mips;
        #endif

        Problem* prb;
        int ind;

        while(pullProblem(prb, ind))
        {
            indices.push_back(ind);

            if(es)
            {
                inp.prb = prb;
                SolverOutput out = ess.solve(inp);
                esTime.push_back(out.runtime);
                esVal.push_back(out.makespan);
            }

            #ifdef ENABLE_MIP
            if(mip)
            {
                inp.prb = prb;
                SolverOutput out = mips.solve(inp);
                mipTime.push_back(out.runtime);
                mipVal.push_back(out.makespan);
            }
            #endif

            if(dp_exact)
            {
                inp.DP_SolutionType = SolutionType::Exact;
                inp.prb = prb;
                SolverOutput out = dps.solve(inp);
                dpeTime.push_back(out.runtime);
                dpeVal.push_back(out.makespan);
            }

            if(dp_approximate)
            {
                inp.DP_SolutionType = SolutionType::Approximate;
                inp.prb = prb;
                SolverOutput out = dps.solve(inp);
                dpaTime.push_back(out.runtime);
                dpaVal.push_back(out.makespan);
            }

            int c = 0;

            for(std::pair<TemperatureEvolution,double> dtp : SA_DecrementTypesAndParameters)
            {
                inp.SA_DecrementType = dtp.first;
                inp.SA_DecrementParameter = dtp.second;
    
                for(double T : SA_MaxTemperatures)
                {
                    inp.SA_MaxTemperature = T;    
                    inp.prb = prb;
                    SolverOutput out = sas.solve(inp);
                    saTime[c].push_back(out.runtime);
                    saVal[c].push_back(out.makespan);    
                    c++;
                }
            }
        }
    }

    void Evaluator::evaluatePreset(const EvaluatorInput& inp)
    {
        int nNum = inp.jobNumbers.size();
        int mNum = inp.machineNumbers.size();
        int cpNum = inp.conflictParities.size();
        int uniformNum = inp.uniformParameters.size();
        int normalNum = inp.normalParameters.size();
        int distSize = uniformNum + normalNum;
        double prbNum = inp.problems.size();
        int threadCount = inp.threadCount;
        int prbConfNum = nNum * mNum * cpNum * distSize;
        int totPrbNum = inp.problems.size();
        int threadStep = totPrbNum / threadCount;
        int saTempNum = inp.SA_MaxTemperatures.size();
        int saDecNum = inp.SA_DecrementTypesAndParameters.size();
        int saNum = saTempNum * saDecNum;
        SolverType baseline = inp.baseline;
        std::vector<std::jthread> threads;
        std::vector<std::vector<int>> indices(threadCount);
        std::vector<std::vector<double>> dpeTime(threadCount);
        std::vector<std::vector<double>> dpeVal(threadCount);
        std::vector<std::vector<double>> dpaTime(threadCount);
        std::vector<std::vector<double>> dpaVal(threadCount);
        std::vector<std::vector<double>> esTime(threadCount);
        std::vector<std::vector<double>> esVal(threadCount);
        std::vector<std::vector<double>> mipTime(threadCount);
        std::vector<std::vector<double>> mipVal(threadCount);
        std::vector<std::vector<std::vector<double>>> saTime(threadCount);
        std::vector<std::vector<std::vector<double>>> saVal(threadCount);
        double totDpeTime = 0; 
        double totDpeVal = 0;
        double totDpeMatch = 0;
        double totDpeDiff = 0;
        double totDpaTime = 0;
        double totDpaVal = 0;
        double totDpaMatch = 0;
        double totDpaDiff = 0;
        double totEsTime = 0;
        double totEsVal = 0;
        double totMipTime = 0;
        double totMipVal = 0;
        double totMipMatch = 0;
        double totMipDiff = 0;
        std::vector<double> totSaTime(saNum);
        std::vector<double> totSaVal(saNum);
        std::vector<double> totSaMatch(saNum);
        std::vector<double> totSaDiff(saNum);

        int c = 0;

        for(Problem* prb : inp.problems)
        {
            problemQueue_.emplace(prb);
            indexQueue_.push(c);
            c++;
        }

        for(int i = 0; i < threadCount; i++)
        {
            saTime[i].reserve(saNum);
            saVal[i].reserve(saNum);

            threads.emplace_back(std::jthread([this, es = inp.es, mip = inp.mip, dp_exact = inp.dp_exact,
                dp_approximate = inp.dp_approximate, &saDecParam = inp.SA_DecrementTypesAndParameters,
                &saMaxTemp = inp.SA_MaxTemperatures, &ind = indices[i], &dpet = dpeTime[i], &dpev = dpeVal[i],
                &dpat = dpaTime[i], &dpav = dpaVal[i], &est = esTime[i], &esv = esVal[i], &mipt = mipTime[i],
                &mipv = mipVal[i], &sat = saTime[i], &sav = saVal[i]]()
            {
                threadFunction(es, mip, dp_exact, dp_approximate, saDecParam, saMaxTemp, ind, dpet, dpev,
                    dpat, dpav, est, esv, mipt, mipv, sat, sav);
            }));
        }

        threads.clear();

        for(int i = 0; i < threadCount; i++)
        {
            for(int j = 0; j < indices[i].size(); j++)
            {
                if(inp.dp_exact)
                {
                    totDpeTime += dpeTime[i][j];
                    totDpeVal += dpeVal[i][j];
                }
                
                if(inp.dp_approximate)
                {
                    totDpaTime += dpaTime[i][j];
                    totDpaVal += dpaVal[i][j];
                }

                if(inp.es)
                {
                    totEsTime += esTime[i][j];
                    totEsVal += esVal[i][j];
                }
                
                if(inp.mip)
                {
                    totMipTime += mipTime[i][j];
                    totMipVal += mipVal[i][j];
                }                

                double bl;

                switch (baseline)
                {
                case SolverType::DP:
                    bl = dpeVal[i][j];
                    break;

                case SolverType::ES:
                    bl = esVal[i][j];
                    break;
                
                #ifdef ENABLE_MIP
                case SolverType::MIP:                
                    bl = mipVal[i][j];
                    break;
                #endif
                }

                if(inp.dp_exact)
                {
                    totDpeMatch += ((dpeVal[i][j] - bl) < 0.000001 && (dpeVal[i][j] - bl) > -0.000001) ? 1 : 0;
                }

                if(inp.dp_approximate)
                {
                    totDpaMatch += ((dpaVal[i][j] - bl) < 0.000001 && (dpaVal[i][j] - bl) > -0.000001) ? 1 : 0;
                }
                
                if(inp.mip)
                {
                    totMipMatch += ((mipVal[i][j] - bl) < 0.000001 && (mipVal[i][j] - bl) > -0.000001) ? 1 : 0;
                }
                
                for(int k = 0; k < saNum; k++)
                {
                    totSaTime[k] += saTime[i][k][j];
                    totSaVal[k] += saVal[i][k][j];
                    totSaMatch[k] += ((saVal[i][k][j] - bl) < 0.000001 && (saVal[i][k][j] - bl) > -0.000001) ? 1 : 0;
                }
            }
        }

        totDpeTime /= prbNum;
        totDpeVal /= prbNum;
        totDpeMatch /= prbNum;
        totDpaTime /= prbNum;
        totDpaVal /= prbNum;
        totDpaMatch /= prbNum;
        totEsTime /= prbNum;
        totEsVal /= prbNum;
        totMipTime /= prbNum;
        totMipVal /= prbNum;
        totMipMatch /= prbNum;

        double bl;

        switch (baseline)
        {
        case SolverType::DP:
            bl = totDpeVal;
            break;

        case SolverType::ES:
            bl = totEsVal;
            break;
        
        #ifdef ENABLE_MIP
        case SolverType::MIP:
            bl = totMipVal;
            break;
        #endif
        }

        totDpeDiff = (totDpeVal - bl) / bl;
        totDpaDiff = (totDpaVal - bl) / bl;
        totMipDiff = (totMipVal - bl) / bl;

        for(int i = 0; i < saNum; i++)
        {
            totSaTime[i] /= prbNum;
            totSaVal[i] /= prbNum;
            totSaMatch[i] /= prbNum;
            totSaDiff[i] = (totSaVal[i] - bl) / bl;
        }

        std::fstream file;
        auto time = std::chrono::system_clock::now();

        const std::filesystem::path filePath = std::filesystem::path(std::getenv("HOME")) / ".TxnSP";

        try
        {
            std::filesystem::create_directory(filePath);
            std::filesystem::create_directory(filePath / "evaluation");
            std::filesystem::create_directory(filePath / "evaluation" / "preset");
            file.open(filePath / "evaluation" / "preset" / (std::to_string(std::chrono::steady_clock::now().time_since_epoch().count())
            + ".csv"), std::fstream::out | std::fstream::trunc);
        }
        catch(const std::filesystem::filesystem_error& e)
        {
            std::cerr << "Cannot open result file: " << e.what() << '\n';
        }

        c = 0;
        std::stringstream sstream;
        sstream.setf(std::ios::fixed);
        sstream.precision(2);
        sstream << "ES,DPE,DPA,MIP,";

        for(int i = 0; i < saDecNum; i++)
        {
            for(int j = 0; j < saTempNum; j++)
            {            
                {
                    sstream << "SA_" << static_cast<int>(inp.SA_DecrementTypesAndParameters[i].first) << "_" << inp.SA_DecrementTypesAndParameters[i].second << "_" << inp.SA_MaxTemperatures[j] << ",";
                }
            }
        }
        

        std::string row = sstream.str();
        row.pop_back();
        file << sstream.str() << row << "\n";
        sstream.str(std::string());

        sstream << std::to_string(totEsTime) << "," << std::to_string(totDpeTime) << "," <<
            std::to_string(totDpaTime)<< "," << std::to_string(totMipTime) << ",";

        for(int m = 0; m < saNum; m++)
        {
            sstream << std::to_string(totSaTime[m]) << ",";
        }

        row = sstream.str();
        row.pop_back();
        file << row << "\n";
        sstream.str(std::string());


        sstream << std::to_string(totEsVal) << "," << std::to_string(totDpeVal) << "," << std::to_string(totDpaVal)
            << "," << std::to_string(totMipVal) << ",";

        for(int m = 0; m < saNum; m++)
        {
            sstream << std::to_string(totSaVal[m]) << ",";
        }

        row = sstream.str();
        row.pop_back();
        file << row << "\n";
        sstream.str(std::string());


        sstream << "1," << std::to_string(totDpeMatch) << "," << std::to_string(totDpaMatch)<< "," <<
            std::to_string(totMipMatch) << ",";

        for(int m = 0; m < saNum; m++)
        {
            sstream << std::to_string(totSaMatch[m]) << ",";
        }

        row = sstream.str();
        row.pop_back();
        file << row << "\n";
        sstream.str(std::string());


        sstream << "0," << std::to_string(totDpeDiff) << "," << std::to_string(totDpaDiff)<< "," <<
            std::to_string(totMipDiff) << ",";

        for(int m = 0; m < saNum; m++)
        {
            sstream << std::to_string(totSaDiff[m]) << ",";
        }

        row = sstream.str();
        row.pop_back();
        file << row << "\n";
        sstream.str(std::string());

        file.flush();
        file.close();
    }

    void Evaluator::evaluateRandom(const EvaluatorInput& inp)
    {
        int nNum = inp.jobNumbers.size();
        int mNum = inp.machineNumbers.size();
        int cpNum = inp.conflictParities.size();
        int uniformNum = inp.uniformParameters.size();
        int normalNum = inp.normalParameters.size();
        int distSize = uniformNum + normalNum;
        int prbNum = inp.problemNumber;
        int threadCount = inp.threadCount;
        int prbConfNum = nNum * mNum * cpNum * distSize;
        int totPrbNum = prbNum * prbConfNum;
        int threadStep = totPrbNum / threadCount;
        int saTempNum = inp.SA_MaxTemperatures.size();
        int saDecNum = inp.SA_DecrementTypesAndParameters.size();
        int saNum = saTempNum * saDecNum;
        SolverType baseline = inp.baseline;
        std::vector<std::jthread> threads;
        std::vector<std::vector<int>> indices(threadCount);
        std::vector<std::vector<double>> dpeTime(threadCount);
        std::vector<std::vector<double>> dpeVal(threadCount);
        std::vector<std::vector<double>> dpaTime(threadCount);
        std::vector<std::vector<double>> dpaVal(threadCount);
        std::vector<std::vector<double>> esTime(threadCount);
        std::vector<std::vector<double>> esVal(threadCount);
        std::vector<std::vector<double>> mipTime(threadCount);
        std::vector<std::vector<double>> mipVal(threadCount);
        std::vector<std::vector<std::vector<double>>> saTime(threadCount);
        std::vector<std::vector<std::vector<double>>> saVal(threadCount);
        std::vector<std::fstream> files(prbConfNum);
        std::vector<double> totDpeTime(prbConfNum);
        std::vector<double> totDpeVal(prbConfNum);
        std::vector<double> totDpeMatch(prbConfNum);
        std::vector<double> totDpeDiff(prbConfNum);
        std::vector<double> totDpaTime(prbConfNum);
        std::vector<double> totDpaVal(prbConfNum);
        std::vector<double> totDpaMatch(prbConfNum);
        std::vector<double> totDpaDiff(prbConfNum);
        std::vector<double> totEsTime(prbConfNum);
        std::vector<double> totEsVal(prbConfNum);
        std::vector<double> totMipTime(prbConfNum);
        std::vector<double> totMipVal(prbConfNum);
        std::vector<double> totMipMatch(prbConfNum);
        std::vector<double> totMipDiff(prbConfNum);
        std::vector<std::vector<double>> totSaTime(saNum);
        std::vector<std::vector<double>> totSaVal(saNum);
        std::vector<std::vector<double>> totSaMatch(saNum);
        std::vector<std::vector<double>> totSaDiff(saNum);
        std::vector<std::unique_ptr<Problem>> problems;

        for(int i = 0; i < saNum; i++)
        {
            totSaTime[i].assign(prbConfNum, 0);
            totSaVal[i].assign(prbConfNum, 0);
            totSaMatch[i].assign(prbConfNum, 0);
            totSaDiff[i].assign(prbConfNum, 0);
        }

        int c = 0;
        
        for(int i = 0; i < nNum; i++)
        {
            for(int j = 0; j < mNum; j++)
            {
                for(int k = 0; k < cpNum; k++)
                {
                    for(int l = 0; l < uniformNum; l++)
                    {
                        for(int m = 0; m < prbNum; m++)
                        {
                            std::unique_ptr<Problem> prb = std::make_unique<Problem>(inp.jobNumbers[i],
                                inp.machineNumbers[j], ProbabilityDistribution::Uniform,
                                inp.uniformParameters[l].first, inp.uniformParameters[l].second,
                                inp.conflictParities[k]);
                            problemQueue_.push(prb.get());
                            problems.push_back(move(prb));                            
                            indexQueue_.push(c);
                        }

                        c++;
                    }

                    for(int l = 0; l < normalNum; l++)
                    {
                        for(int m = 0; m < prbNum; m++)
                        {
                            std::unique_ptr<Problem> prb = std::make_unique<Problem>(inp.jobNumbers[i],
                                inp.machineNumbers[j], ProbabilityDistribution::Normal ,
                                inp.normalParameters[l].first, inp.normalParameters[l].second,
                                inp.conflictParities[k]);
                            problemQueue_.push(prb.get());
                            problems.push_back(move(prb));                            
                            indexQueue_.push(c);
                        }

                        c++;
                    }
                }
            }
        }

        for(int i = 0; i < threadCount; i++)
        {
            saTime[i].reserve(saNum);
            saVal[i].reserve(saNum);

            threads.emplace_back(std::jthread([this, es = inp.es, mip = inp.mip, dp_exact = inp.dp_exact,
                dp_approximate = inp.dp_approximate, &saDecParam = inp.SA_DecrementTypesAndParameters,
                &saMaxTemp = inp.SA_MaxTemperatures, &ind = indices[i], &dpet = dpeTime[i], &dpev = dpeVal[i],
                &dpat = dpaTime[i], &dpav = dpaVal[i], &est = esTime[i], &esv = esVal[i], &mipt = mipTime[i],
                &mipv = mipVal[i], &sat = saTime[i], &sav = saVal[i]]()
            {
                threadFunction(es, mip, dp_exact, dp_approximate, saDecParam, saMaxTemp, ind, dpet, dpev,
                    dpat, dpav, est, esv, mipt, mipv, sat, sav);
            }));
        }

        threads.clear();     

        for(int i = 0; i < threadCount; i++)
        {
            for(int j = 0; j < indices[i].size(); j++)
            {
                if(inp.dp_exact)
                {
                    totDpeTime[indices[i][j]] += dpeTime[i][j];
                    totDpeVal[indices[i][j]] += dpeVal[i][j];
                }
                
                if(inp.dp_approximate)
                {
                    totDpaTime[indices[i][j]] += dpaTime[i][j];
                    totDpaVal[indices[i][j]] += dpaVal[i][j];
                }
                
                if(inp.es)
                {
                    totEsTime[indices[i][j]] += esTime[i][j];
                    totEsVal[indices[i][j]] += esVal[i][j];
                }
                
                if(inp.mip)
                {
                    totMipTime[indices[i][j]] += mipTime[i][j];
                    totMipVal[indices[i][j]] += mipVal[i][j];
                }

                double bl;

                switch (baseline)
                {
                case SolverType::DP:
                    bl = dpeVal[i][j];
                    break;

                case SolverType::ES:
                    bl = esVal[i][j];
                    break;
                
                #ifdef ENABLE_MIP
                case SolverType::MIP:
                    bl = mipVal[i][j];
                    break;
                #endif
                }

                if(inp.dp_exact)
                {
                    totDpeMatch[indices[i][j]] += ((dpeVal[i][j] - bl) < 0.000001 && (dpeVal[i][j] - bl) > -0.000001) ? 1 : 0;
                }

                if(inp.dp_approximate)
                {
                    totDpaMatch[indices[i][j]] += ((dpaVal[i][j] - bl) < 0.000001 && (dpaVal[i][j] - bl) > -0.000001) ? 1 : 0;
                }
                
                if(inp.mip)
                {
                    totMipMatch[indices[i][j]] += ((mipVal[i][j] - bl) < 0.000001 && (mipVal[i][j] - bl) > -0.000001) ? 1 : 0;
                }
                
                for(int k = 0; k < saNum; k++)
                {
                    totSaTime[k][indices[i][j]] += saTime[i][k][j];
                    totSaVal[k][indices[i][j]] += saVal[i][k][j];
                    totSaMatch[k][indices[i][j]] += ((saVal[i][k][j] - bl) < 0.000001 && (saVal[i][k][j] - bl) > -0.000001) ? 1 : 0;
                }
            }
        }

        for(int i = 0; i < prbConfNum; i++)
        {
            totDpeTime[i] /= (double)prbNum;
            totDpeVal[i] /= (double)prbNum;
            totDpeMatch[i] /= (double)prbNum;
            totDpaTime[i] /= (double)prbNum;
            totDpaVal[i] /= (double)prbNum;
            totDpaMatch[i] /= (double)prbNum;
            totEsTime[i] /= (double)prbNum;
            totEsVal[i] /= (double)prbNum;
            totMipTime[i] /= (double)prbNum;
            totMipVal[i] /= (double)prbNum;
            totMipMatch[i] /= (double)prbNum;

            double bl;

            switch (baseline)
            {
            case SolverType::DP:
                bl = totDpeVal[i];
                break;

            case SolverType::ES:
                bl = totEsVal[i];
                break;

            #ifdef ENABLE_MIP
            case SolverType::MIP:
                bl = totMipVal[i];
                break;
            #endif
            }

            totDpeDiff[i] = (totDpeVal[i] - bl) / bl;
            totDpaDiff[i] = (totDpaVal[i] - bl) / bl;
            totMipDiff[i] = (totMipVal[i] - bl) / bl;

            for(int j = 0; j < saNum; j++)
            {
                totSaTime[j][i] /= prbNum;
                totSaVal[j][i] /= prbNum;
                totSaMatch[j][i] /= prbNum;
                totSaDiff[j][i] = (totSaVal[j][i] - bl) / bl;
            }
        }

        c = 0;
        std::stringstream sstream;
        sstream.setf(std::ios::fixed);
        sstream.precision(2);
        sstream << "ES,DPE,DPA,MIP,";

        for(int i = 0; i < saTempNum; i++)
        {
            for(int j = 0; j < saDecNum; j++)
            {
                sstream << "SA_" << static_cast<int>(inp.SA_DecrementTypesAndParameters[j].first) << "_" << inp.SA_DecrementTypesAndParameters[j].second << "_" << inp.SA_MaxTemperatures[i] << ",";
            }
        }

        std::string firstRow = sstream.str();
        firstRow.pop_back();
        sstream.str(std::string());
        
        const std::filesystem::path filePath = std::filesystem::path(std::getenv("HOME")) / ".TxnSP";
        std::filesystem::create_directory(filePath);
        std::filesystem::create_directory(filePath / "evaluation");
        std::filesystem::create_directory(filePath / "evaluation" / "random");

        

        for(int i = 0; i < nNum; i++)
        {
            for(int j = 0; j < mNum; j++)
            {
                for(int l = 0; l < uniformNum; l++)
                {
                    for(int k = 0; k < cpNum; k++)
                    {
                        sstream << inp.jobNumbers[i] << "_" << inp.machineNumbers[j] << "_u_" << 
                        inp.uniformParameters[l].first << "_" << inp.uniformParameters[l].second << "_" << inp.conflictParities[k] << "_" 
                        << prbNum << ".csv";

                        try
                        {
                            files[c].open(filePath / "evaluation" / "random" / sstream.str(), std::fstream::out | std::fstream::trunc);
                        }
                        catch(const std::filesystem::filesystem_error& e)
                        {
                            std::cerr << "Cannot open result file: " << e.what() << '\n';
                        }

                        sstream.str(std::string());
                        files[c] << firstRow << "\n";

                        sstream << std::to_string(totEsTime[c]) << "," << std::to_string(totDpeTime[c]) << "," <<
                            std::to_string(totDpaTime[c]) << "," << std::to_string(totMipTime[c]) << ",";

                        for(int m = 0; m < saNum; m++)
                        {
                            sstream << std::to_string(totSaTime[m][c]) << ",";
                        }

                        std::string row = sstream.str();
                        row.pop_back();
                        files[c] << row << "\n";
                        sstream.str(std::string());

                        sstream << std::to_string(totEsVal[c]) << "," << std::to_string(totDpeVal[c]) << "," <<
                            std::to_string(totDpaVal[c]) << "," << std::to_string(totMipVal[c]) << ",";

                        for(int m = 0; m < saNum; m++)
                        {
                            sstream << std::to_string(totSaVal[m][c]) << ",";
                        }

                        row = sstream.str();
                        row.pop_back();
                        files[c] << row << "\n";
                        sstream.str(std::string());

                        sstream << "1," << std::to_string(totDpeMatch[c]) << "," << std::to_string(totDpaMatch[c])
                            << "," << std::to_string(totMipMatch[c]) << ",";

                        for(int m = 0; m < saNum; m++)
                        {
                            sstream << std::to_string(totSaMatch[m][c]) << ",";
                        }

                        row = sstream.str();
                        row.pop_back();
                        files[c] << row << "\n";
                        sstream.str(std::string());

                        sstream << "0," << std::to_string(totDpeDiff[c]) << "," << std::to_string(totDpaDiff[c])
                            << "," << std::to_string(totMipDiff[c]) << ",";

                        for(int m = 0; m < saNum; m++)
                        {
                            sstream << std::to_string(totSaDiff[m][c]) << ",";
                        }

                        row = sstream.str();
                        row.pop_back();
                        files[c] << row << "\n";
                        sstream.str(std::string());
                        
                        files[c].flush();
                        files[c].close();

                        c++;
                    }
                }

                for(int l = 0; l < normalNum; l++)
                {
                    for(int k = 0; k < cpNum; k++)
                    {
                        sstream << inp.jobNumbers[i] << "_" << inp.machineNumbers[j] << "_n_" << 
                        inp.normalParameters[l].first << "_" << inp.normalParameters[l].second << "_" << inp.conflictParities[k] << "_" 
                        << prbNum << ".csv";

                        try
                        {
                            files[c].open(filePath / "evaluation" / "random" / sstream.str(), std::fstream::out | std::fstream::trunc);
                        }
                        catch(const std::filesystem::filesystem_error& e)
                        {
                            std::cerr << "Cannot open result file: " << e.what() << '\n';
                        }
                        
                        sstream.str(std::string());
                        files[c] << firstRow << "\n";

                        sstream << std::to_string(totEsTime[c]) << "," << std::to_string(totDpeTime[c]) << "," <<
                            std::to_string(totDpaTime[c]) << "," << std::to_string(totMipTime[c]) << ",";

                        for(int m = 0; m < saNum; m++)
                        {
                            sstream << std::to_string(totSaTime[m][c]) << ",";
                        }

                        std::string row = sstream.str();
                        row.pop_back();
                        files[c] << row << "\n";
                        sstream.str(std::string());

                        sstream << std::to_string(totEsVal[c]) << "," << std::to_string(totDpeVal[c]) << "," <<
                            std::to_string(totDpaVal[c]) << "," << std::to_string(totMipVal[c]) << ",";

                        for(int m = 0; m < saNum; m++)
                        {
                            sstream << std::to_string(totSaVal[m][c]) << ",";
                        }

                        row = sstream.str();
                        row.pop_back();
                        files[c] << row << "\n";
                        sstream.str(std::string());

                        sstream << "1," << std::to_string(totDpeMatch[c]) << "," << std::to_string(totDpaMatch[c])
                            << "," <<   std::to_string(totMipMatch[c]) << ",";

                        for(int m = 0; m < saNum; m++)
                        {
                            sstream << std::to_string(totSaMatch[m][c]) << ",";
                        }

                        row = sstream.str();
                        row.pop_back();
                        files[c] << row << "\n";
                        sstream.str(std::string());

                        sstream << "0," << std::to_string(totDpeDiff[c]) << "," << std::to_string(totDpaDiff[c])
                            << "," <<   std::to_string(totMipDiff[c]) << ",";

                        for(int m = 0; m < saNum; m++)
                        {
                            sstream << std::to_string(totSaDiff[m][c]) << ",";
                        }

                        row = sstream.str();
                        row.pop_back();
                        files[c] << row << "\n";
                        sstream.str(std::string());

                        files[c].flush();
                        files[c].close();

                        c++;
                    }
                }                
            }
        }
    }

    void Evaluator::evaluate(const EvaluatorInput& inp)
    {
        #ifndef ENABLE_MIP
            if(inp.mip)
            {
                throw std::runtime_error("MIPSolver is not available in this build. Rebuild with -DENABLE_MIP=ON to use the MIPSolver");
            }
        #endif

        if(inp.baseline == SolverType::SA)
        {
            throw "Simulated Annealing solutions cannot be used as baselines.";
        }

        if(inp.preset)
        {
            evaluatePreset(inp);
        }
        else
        {
            evaluateRandom(inp);
        }
    }
}