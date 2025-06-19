#include "TxnSP/Testers/Evaluator.h"

namespace TxnSP
{
    void ThreadFunction(Problem** problems, int prbNum, bool es, bool mip, bool dp_exact, bool dp_approximate,
    const std::vector<std::pair<TemperatureEvolution,double>> SA_DecrementTypesAndParameters, 
    const std::vector<double> SA_MaxTemperatures, double* dpeTime, double* dpeVal, double* dpaTime, double* dpaVal,
    double* esTime, double* esVal, double* mipTime, double* mipVal, double** saTime, double** saVal)
    {
        
        DPSolver dps;
        ESSolver ess;
        SASolver sas;
        SolverInput inp;    

        #ifdef ENABLE_MIP
            MIPSolver mips;
        #endif

        if(es)
        {
            for(int i = 0; i < prbNum; i++)
            {                
                inp.prb = problems[i];
                SolverOutput* out = ess.solve(inp);
                esTime[i] = out->runtime;
                esVal[i] = out->makespan;

                delete out;
            }
        }

        #ifdef ENABLE_MIP
            if(mip)
            {
                for(int i = 0; i < prbNum; i++)
                {                
                    inp.prb = problems[i];
                    SolverOutput* out = mips.solve(inp);
                    mipTime[i] = out->runtime;
                    mipVal[i] = out->makespan;

                    delete out;
                }
            }
        #endif
        

        if(dp_exact)
        {
            inp.DP_SolutionType = Exact;

            for(int i = 0; i < prbNum; i++)
            {                
                inp.prb = problems[i];
                SolverOutput* out = dps.solve(inp);
                dpeTime[i] = out->runtime;
                dpeVal[i] = out->makespan;

                delete out;
            }
        }

        if(dp_approximate)
        {
            inp.DP_SolutionType = Approximate;
            
            for(int i = 0; i < prbNum; i++)
            {                
                inp.prb = problems[i];
                SolverOutput* out = dps.solve(inp);
                dpaTime[i] = out->runtime;
                dpaVal[i] = out->makespan;

                delete out;
            }
        }

        int c = 0;

        for(std::pair<TemperatureEvolution,double> dtp : SA_DecrementTypesAndParameters)
        {
            inp.SA_DecrementType = dtp.first;
            inp.SA_DecrementParameter = dtp.second;

            for(double T : SA_MaxTemperatures)
            {
                inp.SA_MaxTemperature = T;

                for(int i = 0; i < prbNum; i++)
                {
                    inp.prb = problems[i];
                    SolverOutput* out = sas.solve(inp);
                    saTime[c][i] = out->runtime;
                    saVal[c][i] = out->makespan;

                    delete out;
                }

                c++;
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
        Problem*** thProblems = new Problem**[threadCount];
        thread* threads = new thread[threadCount];
        int* prbNums = new int[threadCount];
        double** dpeTime = new double*[threadCount];
        double** dpeVal = new double*[threadCount];
        double** dpaTime = new double*[threadCount];
        double** dpaVal = new double*[threadCount];
        double** esTime = new double*[threadCount];
        double** esVal = new double*[threadCount];
        double** mipTime = new double*[threadCount];
        double** mipVal = new double*[threadCount];
        double*** saTime = new double**[threadCount];
        double*** saVal = new double**[threadCount];
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
        double* totSaTime = new double[saNum];
        double* totSaVal = new double[saNum];
        double* totSaMatch = new double[saNum];
        double* totSaDiff = new double[saNum];

        for(int i = 0; i < saNum; i++)
        {
            totSaTime[i] = 0;
            totSaVal[i] = 0;
            totSaMatch[i] = 0;
            totSaDiff[i] = 0;
        }

        for(int i = 0; i < threadCount - 1; i++)
        {
            prbNums[i] = threadStep;
            esTime[i] = new double[threadStep];
            esVal[i] = new double[threadStep];
            mipTime[i] = new double[threadStep];
            mipVal[i] = new double[threadStep];
            dpeTime[i] = new double[threadStep];
            dpeVal[i] = new double[threadStep];
            dpaTime[i] = new double[threadStep];
            dpaVal[i] = new double[threadStep];            
            saTime[i] = new double*[saNum];
            saVal[i] = new double*[saNum];

            for(int j = 0; j < saNum; j++)
            {
                saTime[i][j] = new double[threadStep];
                saVal[i][j] = new double[threadStep];
            }
        }

        prbNums[threadCount - 1] = totPrbNum - (threadCount - 1) * threadStep;
        esTime[threadCount - 1] = new double[prbNums[threadCount - 1]];
        esVal[threadCount - 1] = new double[prbNums[threadCount - 1]];
        mipTime[threadCount - 1] = new double[prbNums[threadCount - 1]];
        mipVal[threadCount - 1] = new double[prbNums[threadCount - 1]];
        dpeTime[threadCount - 1] = new double[prbNums[threadCount - 1]];
        dpeVal[threadCount - 1] = new double[prbNums[threadCount - 1]];
        dpaTime[threadCount - 1] = new double[prbNums[threadCount - 1]];
        dpaVal[threadCount - 1] = new double[prbNums[threadCount - 1]];            
        saTime[threadCount - 1] = new double*[saNum];
        saVal[threadCount - 1] = new double*[saNum];

        for(int j = 0; j < saNum; j++)
        {
            saTime[threadCount - 1][j] = new double[prbNums[threadCount - 1]];
            saVal[threadCount - 1][j] = new double[prbNums[threadCount - 1]];
        }

        int threadInd = 0;
        int count = 0;
        int c = 0;
        thProblems[0] = new Problem*[prbNums[0]];

        for(Problem* prb : inp.problems)
        {
            thProblems[threadInd][count] = prb;
            count++;

            if(count == threadStep && threadInd != threadCount - 1)
            {
                count = 0;
                threadInd++;
                thProblems[threadInd] = new Problem*[prbNums[threadInd]];
            } 
        }

        for(int i = 0; i < threadCount; i++)
        {
            threads[i] = thread(ThreadFunction, thProblems[i], prbNums[i], inp.es, inp.mip, inp.dp_exact, inp.dp_approximate,
            inp.SA_DecrementTypesAndParameters, inp.SA_MaxTemperatures, dpeTime[i], dpeVal[i], dpaTime[i], dpaVal[i], 
            esTime[i], esVal[i], mipTime[i], mipVal[i], saTime[i], saVal[i]);
        }

        for(int i = 0; i < threadCount; i++)
        {
            threads[i].join();
        }

        for(int i = 0; i < threadCount; i++)
        {
            for(int j = 0; j < prbNums[i]; j++)
            {
                totDpeTime += dpeTime[i][j];
                totDpeVal += dpeVal[i][j];
                totDpaTime += dpaTime[i][j];
                totDpaVal += dpaVal[i][j];
                totEsTime += esTime[i][j];
                totEsVal += esVal[i][j];
                totMipTime += mipTime[i][j];
                totMipVal += mipVal[i][j];

                double bl;

                switch (baseline)
                {
                case DP:
                    bl = dpeVal[i][j];
                    break;

                case ES:
                    bl = esVal[i][j];
                    break;

                case MIP:
                    bl = mipVal[i][j];
                    break;
                }

                totDpeMatch += ((dpeVal[i][j] - bl) < 0.000001 && (dpeVal[i][j] - bl) > -0.000001) ? 1 : 0;
                totDpaMatch += ((dpaVal[i][j] - bl) < 0.000001 && (dpeVal[i][j] - bl) > -0.000001) ? 1 : 0;
                totMipMatch += ((mipVal[i][j] - bl) < 0.000001 && (dpeVal[i][j] - bl) > -0.000001) ? 1 : 0;

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
        case DP:
            bl = totDpeVal;
            break;

        case ES:
            bl = totEsVal;
            break;

        case MIP:
            bl = totMipVal;
            break;
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

        fstream file;
        auto time = std::chrono::system_clock::now();
        file.open(inp.directory + "/preset" + to_string(std::chrono::steady_clock::now().time_since_epoch().count()) + ".csv", fstream::out | fstream::trunc);

        c = 0;
        stringstream sstream;
        sstream.setf(std::ios::fixed);
        sstream.precision(2);
        sstream << "ES,DPE,DPA,MIP,";

        for(int i = 0; i < saDecNum; i++)
        {
            for(int j = 0; j < saTempNum; j++)
            {            
                {
                    sstream << "SA_" << inp.SA_DecrementTypesAndParameters[i].first << "_" << inp.SA_DecrementTypesAndParameters[i].second << "_" << inp.SA_MaxTemperatures[j] << ",";
                }
            }
        }
        

        string row = sstream.str();
        row.pop_back();
        file << sstream.str() << row << "\n";
        sstream.str(string());

        sstream << to_string(totEsTime) << "," << to_string(totDpeTime) << "," << to_string(totDpaTime)<< "," << 
        to_string(totMipTime) << ",";

        for(int m = 0; m < saNum; m++)
        {
            sstream << to_string(totSaTime[m]) << ",";
        }

        row = sstream.str();
        row.pop_back();
        file << row << "\n";
        sstream.str(string());


        sstream << to_string(totEsVal) << "," << to_string(totDpeVal) << "," << to_string(totDpaVal)<< "," << 
        to_string(totMipVal) << ",";

        for(int m = 0; m < saNum; m++)
        {
            sstream << to_string(totSaVal[m]) << ",";
        }

        row = sstream.str();
        row.pop_back();
        file << row << "\n";
        sstream.str(string());


        sstream << "1," << to_string(totDpeMatch) << "," << to_string(totDpaMatch)<< "," << to_string(totMipMatch) << ",";

        for(int m = 0; m < saNum; m++)
        {
            sstream << to_string(totSaMatch[m]) << ",";
        }

        row = sstream.str();
        row.pop_back();
        file << row << "\n";
        sstream.str(string());


        sstream << "0," << to_string(totDpeDiff) << "," << to_string(totDpaDiff)<< "," << to_string(totMipDiff) << ",";

        for(int m = 0; m < saNum; m++)
        {
            sstream << to_string(totSaDiff[m]) << ",";
        }

        row = sstream.str();
        row.pop_back();
        file << row << "\n";
        sstream.str(string());

        file.flush();
        file.close();  

        for(int i = 0; i < threadCount; i++)
        {
            delete[] thProblems[i];
            delete[] dpeTime[i];
            delete[] dpeVal[i];
            delete[] dpaTime[i];
            delete[] dpaVal[i];
            delete[] esTime[i];
            delete[] esVal[i];
            delete[] mipTime[i];
            delete[] mipVal[i];

            for(int j = 0; j < saNum; j++)
            {
                delete[] saTime[i][j];
                delete[] saVal[i][j];
            }

            delete[] saTime[i];
            delete[] saVal[i];
        }

        delete[] thProblems;
        delete[] threads;
        delete[] prbNums;
        delete[] dpeTime;
        delete[] dpeVal;
        delete[] dpaTime;
        delete[] dpaVal;
        delete[] esTime;
        delete[] esVal;
        delete[] mipTime;
        delete[] mipVal;
        delete[] saTime;
        delete[] saVal;
        delete[] totSaTime;
        delete[] totSaVal;
        delete[] totSaMatch;
        delete[] totSaDiff;
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
        Problem*** problems = new Problem**[prbConfNum];
        Problem*** thProblems = new Problem**[threadCount];
        thread* threads = new thread[threadCount];
        int* prbNums = new int[threadCount];
        int** prbInds = new int*[threadCount];
        double** dpeTime = new double*[threadCount];
        double** dpeVal = new double*[threadCount];
        double** dpaTime = new double*[threadCount];
        double** dpaVal = new double*[threadCount];
        double** esTime = new double*[threadCount];
        double** esVal = new double*[threadCount];
        double** mipTime = new double*[threadCount];
        double** mipVal = new double*[threadCount];
        double*** saTime = new double**[threadCount];
        double*** saVal = new double**[threadCount];
        fstream* files = new fstream[prbConfNum];
        double* totDpeTime = new double[prbConfNum];
        double* totDpeVal = new double[prbConfNum];
        double* totDpeMatch = new double[prbConfNum];
        double* totDpeDiff = new double[prbConfNum];
        double* totDpaTime = new double[prbConfNum];
        double* totDpaVal = new double[prbConfNum];
        double* totDpaMatch = new double[prbConfNum];
        double* totDpaDiff = new double[prbConfNum];
        double* totEsTime = new double[prbConfNum];
        double* totEsVal = new double[prbConfNum];
        double* totMipTime = new double[prbConfNum];
        double* totMipVal = new double[prbConfNum];
        double* totMipMatch = new double[prbConfNum];
        double* totMipDiff = new double[prbConfNum];
        double** totSaTime = new double*[saNum];
        double** totSaVal = new double*[saNum];
        double** totSaMatch = new double*[saNum];
        double** totSaDiff = new double*[saNum];

        for(int i = 0; i < prbConfNum; i++)
        {
            totDpeTime[i] = 0;
            totDpeVal[i] = 0;
            totDpeMatch[i] = 0;
            totDpeDiff[i] = 0;
            totDpaTime[i] = 0;
            totDpaVal[i] = 0;
            totDpaMatch[i] = 0;
            totDpaDiff[i] = 0;
            totEsTime[i] = 0;
            totEsVal[i] = 0;
            totMipTime[i] = 0;
            totMipVal[i] = 0;
            totMipMatch[i] = 0;
            totMipDiff[i] = 0;
        }

        for(int i = 0; i < saNum; i++)
        {
            totSaTime[i] = new double[prbConfNum];
            totSaVal[i] = new double[prbConfNum];
            totSaMatch[i] = new double[prbConfNum];
            totSaDiff[i] = new double[prbConfNum];

            for(int j = 0; j < prbConfNum; j++)
            {
                totSaTime[i][j] = 0;
                totSaVal[i][j] = 0;
                totSaMatch[i][j] = 0;
                totSaDiff[i][j] = 0;
            }
        }

        for(int i = 0; i < threadCount - 1; i++)
        {
            prbNums[i] = threadStep;
            prbInds[i] = new int[threadStep];
            esTime[i] = new double[threadStep];
            esVal[i] = new double[threadStep];
            mipTime[i] = new double[threadStep];
            mipVal[i] = new double[threadStep];
            dpeTime[i] = new double[threadStep];
            dpeVal[i] = new double[threadStep];
            dpaTime[i] = new double[threadStep];
            dpaVal[i] = new double[threadStep];            
            saTime[i] = new double*[saNum];
            saVal[i] = new double*[saNum];

            for(int j = 0; j < saNum; j++)
            {
                saTime[i][j] = new double[threadStep];
                saVal[i][j] = new double[threadStep];
            }
        }

        prbNums[threadCount - 1] = totPrbNum - (threadCount - 1) * threadStep;
        prbInds[threadCount - 1] = new int[prbNums[threadCount - 1]];
        esTime[threadCount - 1] = new double[prbNums[threadCount - 1]];
        esVal[threadCount - 1] = new double[prbNums[threadCount - 1]];
        mipTime[threadCount - 1] = new double[prbNums[threadCount - 1]];
        mipVal[threadCount - 1] = new double[prbNums[threadCount - 1]];
        dpeTime[threadCount - 1] = new double[prbNums[threadCount - 1]];
        dpeVal[threadCount - 1] = new double[prbNums[threadCount - 1]];
        dpaTime[threadCount - 1] = new double[prbNums[threadCount - 1]];
        dpaVal[threadCount - 1] = new double[prbNums[threadCount - 1]];            
        saTime[threadCount - 1] = new double*[saNum];
        saVal[threadCount - 1] = new double*[saNum];

        for(int j = 0; j < saNum; j++)
        {
            saTime[threadCount - 1][j] = new double[prbNums[threadCount - 1]];
            saVal[threadCount - 1][j] = new double[prbNums[threadCount - 1]];
        }

        int threadInd = 0;
        int count = 0;
        int c = 0;
        thProblems[0] = new Problem*[prbNums[0]];
        
        for(int i = 0; i < nNum; i++)
        {
            for(int j = 0; j < mNum; j++)
            {
                for(int k = 0; k < cpNum; k++)
                {
                    for(int l = 0; l < uniformNum; l++)
                    {
                        problems[c] = new Problem*[prbNum];

                        for(int m = 0; m < prbNum; m++)
                        {
                            problems[c][m] = new Problem(inp.jobNumbers[i], inp.machineNumbers[j], UniformDistribution, inp.uniformParameters[l].first, inp.uniformParameters[l].second, inp.conflictParities[k]);
                            thProblems[threadInd][count] = problems[c][m];
                            prbInds[threadInd][count] = c;
                            count++;

                            if(count == threadStep && threadInd != threadCount - 1)
                            {
                                count = 0;
                                threadInd++;
                                thProblems[threadInd] = new Problem*[prbNums[threadInd]];
                            } 
                        }

                        c++;
                    }

                    for(int l = 0; l < normalNum; l++)
                    {
                        problems[c] = new Problem*[prbNum];

                        for(int m = 0; m < prbNum; m++)
                        {
                            problems[c][m] = new Problem(inp.jobNumbers[i], inp.machineNumbers[j], NormalDistribution, inp.normalParameters[l].first, inp.normalParameters[l].second, inp.conflictParities[k]);
                            thProblems[threadInd][count] = problems[c][m];
                            prbInds[threadInd][count] = c;
                            count++;

                            if(count == threadStep && threadInd != threadCount - 1)
                            {
                                count = 0;
                                threadInd++;
                                thProblems[threadInd] = new Problem*[prbNums[threadInd]];
                            }
                        }

                        c++;
                    }
                }
            }
        }

        for(int i = 0; i < threadCount; i++)
        {
            threads[i] = thread(ThreadFunction, thProblems[i], prbNums[i], inp.es, inp.mip, inp.dp_exact, inp.dp_approximate,
            inp.SA_DecrementTypesAndParameters, inp.SA_MaxTemperatures, dpeTime[i], dpeVal[i], dpaTime[i], dpaVal[i], 
            esTime[i], esVal[i], mipTime[i], mipVal[i], saTime[i], saVal[i]);
        }

        for(int i = 0; i < threadCount; i++)
        {
            threads[i].join();
        }        

        for(int i = 0; i < threadCount; i++)
        {
            for(int j = 0; j < prbNums[i]; j++)
            {
                totDpeTime[prbInds[i][j]] += dpeTime[i][j];
                totDpeVal[prbInds[i][j]] += dpeVal[i][j];
                totDpaTime[prbInds[i][j]] += dpaTime[i][j];
                totDpaVal[prbInds[i][j]] += dpaVal[i][j];
                totEsTime[prbInds[i][j]] += esTime[i][j];
                totEsVal[prbInds[i][j]] += esVal[i][j];
                totMipTime[prbInds[i][j]] += mipTime[i][j];
                totMipVal[prbInds[i][j]] += mipVal[i][j];

                double bl;

                switch (baseline)
                {
                case DP:
                    bl = dpeVal[i][j];
                    break;

                case ES:
                    bl = esVal[i][j];
                    break;

                case MIP:
                    bl = mipVal[i][j];
                    break;
                }

                totDpeMatch[prbInds[i][j]] += ((dpeVal[i][j] - bl) < 0.000001 && (dpeVal[i][j] - bl) > -0.000001) ? 1 : 0;
                totDpaMatch[prbInds[i][j]] += ((dpaVal[i][j] - bl) < 0.000001 && (dpeVal[i][j] - bl) > -0.000001) ? 1 : 0;
                totMipMatch[prbInds[i][j]] += ((mipVal[i][j] - bl) < 0.000001 && (dpeVal[i][j] - bl) > -0.000001) ? 1 : 0;

                for(int k = 0; k < saNum; k++)
                {
                    totSaTime[k][prbInds[i][j]] += saTime[i][k][j];
                    totSaVal[k][prbInds[i][j]] += saVal[i][k][j];
                    totSaMatch[k][prbInds[i][j]] += ((saVal[i][k][j] - bl) < 0.000001 && (saVal[i][k][j] - bl) > -0.000001) ? 1 : 0;
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
            case DP:
                bl = totDpeVal[i];
                break;

            case ES:
                bl = totEsVal[i];
                break;

            case MIP:
                bl = totMipVal[i];
                break;
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

            for(int j = 0; j < prbNum; j++)
            {
                delete problems[i][j];
            }

            delete[] problems[i];
        }

        c = 0;
        stringstream sstream;
        sstream.setf(std::ios::fixed);
        sstream.precision(2);
        sstream << "ES,DPE,DPA,MIP,";

        for(int i = 0; i < saTempNum; i++)
        {
            for(int j = 0; j < saDecNum; j++)
            {
                sstream << "SA_" << inp.SA_DecrementTypesAndParameters[j].first << "_" << inp.SA_DecrementTypesAndParameters[j].second << "_" << inp.SA_MaxTemperatures[i] << ",";
            }
        }

        string firstRow = sstream.str();
        firstRow.pop_back();
        sstream.str(string());
        
        for(int i = 0; i < nNum; i++)
        {
            for(int j = 0; j < mNum; j++)
            {
                for(int l = 0; l < uniformNum; l++)
                {
                    for(int k = 0; k < cpNum; k++)
                    {
                        sstream << inp.directory << "/" << inp.jobNumbers[i] << "_" << inp.machineNumbers[j] << "_u_" << 
                        inp.uniformParameters[l].first << "_" << inp.uniformParameters[l].second << "_" << inp.conflictParities[k] << "_" 
                        << prbNum << ".csv";

                        files[c].open(sstream.str(), fstream::out | fstream::trunc);
                        sstream.str(string());
                        files[c] << firstRow << "\n";

                        sstream << to_string(totEsTime[c]) << "," << to_string(totDpeTime[c]) << "," << to_string(totDpaTime[c])
                        << "," << to_string(totMipTime[c]) << ",";

                        for(int m = 0; m < saNum; m++)
                        {
                            sstream << to_string(totSaTime[m][c]) << ",";
                        }

                        string row = sstream.str();
                        row.pop_back();
                        files[c] << row << "\n";
                        sstream.str(string());

                        sstream << to_string(totEsVal[c]) << "," << to_string(totDpeVal[c]) << "," << to_string(totDpaVal[c])
                        << "," << to_string(totMipVal[c]) << ",";

                        for(int m = 0; m < saNum; m++)
                        {
                            sstream << to_string(totSaVal[m][c]) << ",";
                        }

                        row = sstream.str();
                        row.pop_back();
                        files[c] << row << "\n";
                        sstream.str(string());

                        sstream << "1," << to_string(totDpeMatch[c]) << "," << to_string(totDpaMatch[c]) << "," <<
                        to_string(totMipMatch[c]) << ",";

                        for(int m = 0; m < saNum; m++)
                        {
                            sstream << to_string(totSaMatch[m][c]) << ",";
                        }

                        row = sstream.str();
                        row.pop_back();
                        files[c] << row << "\n";
                        sstream.str(string());

                        sstream << "0," << to_string(totDpeDiff[c]) << "," << to_string(totDpaDiff[c]) << "," <<
                        to_string(totMipDiff[c]) << ",";

                        for(int m = 0; m < saNum; m++)
                        {
                            sstream << to_string(totSaDiff[m][c]) << ",";
                        }

                        row = sstream.str();
                        row.pop_back();
                        files[c] << row << "\n";
                        sstream.str(string());
                        
                        files[c].flush();
                        files[c].close();

                        c++;
                    }
                }

                for(int l = 0; l < normalNum; l++)
                {
                    for(int k = 0; k < cpNum; k++)
                    {
                        sstream << "../data/Evaluator/" << inp.jobNumbers[i] << "_" << inp.machineNumbers[j] << "_n_" << 
                        inp.normalParameters[l].first << "_" << inp.normalParameters[l].second << "_" << inp.conflictParities[k] << "_" 
                        << prbNum << ".csv";

                        files[c].open(sstream.str(), fstream::out | fstream::trunc);
                        sstream.str(string());
                        files[c] << firstRow << "\n";

                        sstream << to_string(totEsTime[c]) << "," << to_string(totDpeTime[c]) << "," << to_string(totDpaTime[c])
                        << "," << to_string(totMipTime[c]) << ",";

                        for(int m = 0; m < saNum; m++)
                        {
                            sstream << to_string(totSaTime[m][c]) << ",";
                        }

                        string row = sstream.str();
                        row.pop_back();
                        files[c] << row << "\n";
                        sstream.str(string());

                        sstream << to_string(totEsVal[c]) << "," << to_string(totDpeVal[c]) << "," << to_string(totDpaVal[c])
                        << "," << to_string(totMipVal[c]) << ",";

                        for(int m = 0; m < saNum; m++)
                        {
                            sstream << to_string(totSaVal[m][c]) << ",";
                        }

                        row = sstream.str();
                        row.pop_back();
                        files[c] << row << "\n";
                        sstream.str(string());

                        sstream << "1," << to_string(totDpeMatch[c]) << "," << to_string(totDpaMatch[c]) << "," <<
                        to_string(totMipMatch[c]) << ",";

                        for(int m = 0; m < saNum; m++)
                        {
                            sstream << to_string(totSaMatch[m][c]) << ",";
                        }

                        row = sstream.str();
                        row.pop_back();
                        files[c] << row << "\n";
                        sstream.str(string());

                        sstream << "0," << to_string(totDpeDiff[c]) << "," << to_string(totDpaDiff[c]) << "," <<
                        to_string(totMipDiff[c]) << ",";

                        for(int m = 0; m < saNum; m++)
                        {
                            sstream << to_string(totSaDiff[m][c]) << ",";
                        }

                        row = sstream.str();
                        row.pop_back();
                        files[c] << row << "\n";
                        sstream.str(string());

                        files[c].flush();
                        files[c].close();

                        c++;
                    }
                }                
            }
        }     
               

        for(int i = 0; i < threadCount; i++)
        {
            delete[] thProblems[i];
            delete[] prbInds[i];
            delete[] dpeTime[i];
            delete[] dpeVal[i];
            delete[] dpaTime[i];
            delete[] dpaVal[i];
            delete[] esTime[i];
            delete[] esVal[i];
            delete[] mipTime[i];
            delete[] mipVal[i];

            for(int j = 0; j < saNum; j++)
            {
                delete[] saTime[i][j];
                delete[] saVal[i][j];
            }

            delete[] saTime[i];
            delete[] saVal[i];
        }

        for(int i = 0; i < saNum; i++)
        {
            delete[] totSaTime[i];
            delete[] totSaVal[i];
            delete[] totSaMatch[i];
            delete[] totSaDiff[i];          
        }

        delete[] problems;
        delete[] thProblems;
        delete[] threads;
        delete[] prbNums;
        delete[] prbInds;
        delete[] dpeTime;
        delete[] dpeVal;
        delete[] dpaTime;
        delete[] dpaVal;
        delete[] esTime;
        delete[] esVal;
        delete[] mipTime;
        delete[] mipVal;
        delete[] saTime;
        delete[] saVal;
        delete[] files;
        delete[] totDpeTime;
        delete[] totDpeVal;
        delete[] totDpeMatch;
        delete[] totDpeDiff;
        delete[] totDpaTime;
        delete[] totDpaVal;
        delete[] totDpaMatch;
        delete[] totDpaDiff;
        delete[] totEsTime;
        delete[] totEsVal;
        delete[] totMipTime;
        delete[] totMipVal;
        delete[] totMipMatch;
        delete[] totMipDiff;
        delete[] totSaTime;
        delete[] totSaVal;
        delete[] totSaMatch;
        delete[] totSaDiff;
    }

    void Evaluator::evaluate(const EvaluatorInput& inp)
    {
        #ifndef ENABLE_MIP
            if(inp.mip)
            {
                throw std::runtime_error("MIPSolver is not available in this build. Rebuild with -DENABLE_MIP=ON to use the MIPSolver");
            }
        #endif

        if(inp.baseline == SA)
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