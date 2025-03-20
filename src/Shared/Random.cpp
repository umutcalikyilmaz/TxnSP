#include <Shared/Random.h>

namespace TransactionScheduling
{
	RandomNumberGenerator::RandomNumberGenerator() : generator(std::random_device{}()) { }

	NormalRandomNumberGenerator::NormalRandomNumberGenerator(double mean, double std) : normalDistribution(mean, std) { }

	double NormalRandomNumberGenerator::Generate()
	{
		return normalDistribution(generator);
	}

	void NormalRandomNumberGenerator::ChangeParams(double mean, double std)
	{
		normalDistribution.param(std::normal_distribution<double>::param_type(mean, std));
	}
	
	UniformRandomIntGenerator::UniformRandomIntGenerator(int min, int max) : uniformDistribution(min, max) { }

	int UniformRandomIntGenerator::Generate()
	{
		return uniformDistribution(generator);
	}

	void UniformRandomIntGenerator::ChangeParams(int min, int max)
	{
		uniformDistribution.param(std::uniform_int_distribution<int>::param_type(min, max));
	}

	UniformRandomDoubleGenerator::UniformRandomDoubleGenerator(double min, double max) : uniformDistribution(min, max) { }

	double UniformRandomDoubleGenerator::Generate()
	{
		return uniformDistribution(generator);
	}

	void UniformRandomDoubleGenerator::ChangeParams(double min, double max)
	{
		uniformDistribution.param(std::uniform_real_distribution<double>::param_type(min, max));
	}

	
	UniformRandomInt128Generator::UniformRandomInt128Generator(__uint128_t min, __uint128_t max) : uniformDistribution(min, max) { }

	__uint128_t UniformRandomInt128Generator::Generate()
	{
		return uniformDistribution(generator);
	}

	void UniformRandomInt128Generator::ChangeParams(__uint128_t min, __uint128_t max)
	{
		uniformDistribution.param(std::uniform_int_distribution<__uint128_t>::param_type(min, max));
	}
}