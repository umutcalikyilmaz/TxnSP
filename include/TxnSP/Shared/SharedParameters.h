#pragma once
#include <cstdint>

namespace TxnSP
{
    enum class ProbabilityDistribution : uint8_t
	{
		Uniform,
		Normal
	};

	enum class SolutionType : uint8_t
	{
		Approximate,
		Exact
	};

	enum class TemperatureEvolution : uint8_t
	{
		Exponential,
		Linear,
		Slow
	};


	#ifdef ENABLE_MIP
	enum class SolverType : uint8_t
	{
		DP,
		ES,
		MIP,
		SA
	};
	#else
	enum class SolverType : uint8_t
	{
		DP,
		ES,
		SA
	};
	#endif
	
}