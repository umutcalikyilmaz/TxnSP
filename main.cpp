#include <iostream>
#include "TxnSP/TxnSP.h"
#include "TxnSP/TxnSPTest.h"

int main(int, char**)
{
    TxnSP::Analyzer ana;
    TxnSP::AnalyzerInput ainp;

    ainp.threadCount = 10;
    ainp.conflictParityStepSize = 0.05;
    ainp.distribution = TxnSP::ProbabilityDistribution::Normal;
    ainp.distributionParameter1 = 100;
    ainp.distributionParameter2 = 25;
    ainp.jobNumber = 9;
    ainp.machineNumber = 3;
    ainp.problemNumber = 10;

    for(int i = 0; i < 10; i++)
    {
        ana.analyze(ainp);
    }
    

    TxnSP::Problem prb(10, 4, TxnSP::ProbabilityDistribution::Normal, 100, 25, 0.25);

    TxnSP::Evaluator evo;
    TxnSP::EvaluatorInput einp;
    einp.baseline = TxnSP::SolverType::ES;
    einp.problems.push_back(&prb);
    einp.conflictParities.push_back(0.25);
    einp.conflictParities.push_back(0.50);
    einp.conflictParities.push_back(0.75);
    einp.dp_approximate = false;
    einp.dp_exact = false;
    einp.es = true;
    einp.jobNumbers.push_back(8);
    einp.jobNumbers.push_back(9);
    //einp.jobNumbers.push_back(10);
    einp.machineNumbers.push_back(4);
    einp.machineNumbers.push_back(3);
    einp.mip = true;
    einp.preset = false;
    einp.normalParameters.push_back(std::make_pair(100,10));
    einp.normalParameters.push_back(std::make_pair(100,25));
    einp.preset = false;
    einp.problemNumber = 10;
    einp.SA_DecrementTypesAndParameters.push_back(std::make_pair(TxnSP::TemperatureEvolution::Exponential, 0.99));
    einp.SA_DecrementTypesAndParameters.push_back(std::make_pair(TxnSP::TemperatureEvolution::Exponential, 0.9));
    einp.SA_MaxTemperatures.push_back(1);
    einp.threadCount = 14;
    einp.uniformParameters.push_back(std::make_pair(10,50));

    for(int i = 0; i < 10; i++)
    {
        evo.evaluate(einp);
    }
    int assd = 0;
}