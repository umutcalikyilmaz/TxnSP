#pragma once

namespace TxnSP
{
    enum ProbabilityDistribution
	{
		UniformDistribution,
		NormalDistribution
	};

	enum SolutionType
	{
		Approximate,
		Exact
	};

	enum TemperatureEvolution
	{
		Exponential,
		Linear,
		Slow
	};


	#ifdef ENABLE_MIP
	enum SolverType
	{
		DP,
		ES,
		MIP,
		SA
	};
	#else
	enum SolverType
	{
		DP,
		ES,
		SA
	};
	#endif
	
}