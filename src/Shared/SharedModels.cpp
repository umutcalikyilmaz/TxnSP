#include "TxnSP/Shared/SharedModels.h"

namespace TxnSP
{
	RandomNumberGenerator::RandomNumberGenerator() : generator(std::random_device{}()) { }

	NormalRandomNumberGenerator::NormalRandomNumberGenerator(double mean, double std) : normalDistribution_(mean, std) { }

	double NormalRandomNumberGenerator::generate()
	{
		return normalDistribution_(generator);
	}

	void NormalRandomNumberGenerator::changeParameters(double mean, double std)
	{
		normalDistribution_.param(std::normal_distribution<double>::param_type(mean, std));
	}
	
	UniformRandomIntGenerator::UniformRandomIntGenerator(int min, int max) : uniformDistribution_(min, max) { }

	int UniformRandomIntGenerator::generate()
	{
		return uniformDistribution_(generator);
	}

	void UniformRandomIntGenerator::changeParameters(int min, int max)
	{
		uniformDistribution_.param(std::uniform_int_distribution<int>::param_type(min, max));
	}

	UniformRandomDoubleGenerator::UniformRandomDoubleGenerator(double min, double max) : uniformDistribution_(min, max) { }

	double UniformRandomDoubleGenerator::generate()
	{
		return uniformDistribution_(generator);
	}

	void UniformRandomDoubleGenerator::changeParameters(double min, double max)
	{
		uniformDistribution_.param(std::uniform_real_distribution<double>::param_type(min, max));
	}
}