#pragma once
#include <random>
#include <cmath>
#include <cstdlib>
#include <chrono>
#include <boost/multiprecision/cpp_int.hpp>

namespace TxnSP
{
    class RandomNumberGenerator
	{
	protected:
		std::minstd_rand generator;

		RandomNumberGenerator();
	};

	class NormalRandomNumberGenerator : public RandomNumberGenerator
	{
	public:

		NormalRandomNumberGenerator(double mean, double std);

		double generate();

		void changeParameters(double mean, double std);

	private:
		
		std::normal_distribution<double> normalDistribution_;
	};

	class UniformRandomIntGenerator : public RandomNumberGenerator
	{
	public:

		UniformRandomIntGenerator(int min, int max);

		int generate();

		void changeParameters(int min, int max);

	private:

		std::uniform_int_distribution<int> uniformDistribution_;
	};

	class UniformRandomDoubleGenerator : public RandomNumberGenerator
	{
	public:

		UniformRandomDoubleGenerator(double min, double max);

		double generate();

		void changeParameters(double min, double max);

	private:

		std::uniform_real_distribution<double> uniformDistribution_;
	};
}