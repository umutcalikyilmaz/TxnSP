#pragma once
#include <random>
#include <cmath>
#include <cstdlib>
#include <chrono>
#include <boost/multiprecision/cpp_int.hpp>

using namespace std;

namespace TxnSP
{
    class RandomNumberGenerator
	{
	protected:
		minstd_rand generator;

		RandomNumberGenerator();
	};

	class NormalRandomNumberGenerator : public RandomNumberGenerator
	{
	public:

		NormalRandomNumberGenerator(double mean, double std);

		double generate();

		void changeParameters(double mean, double std);

	private:
		
		normal_distribution<double> normalDistribution_;
	};

	class UniformRandomIntGenerator : public RandomNumberGenerator
	{
	public:

		UniformRandomIntGenerator(int min, int max);

		int generate();

		void changeParameters(int min, int max);

	private:

		uniform_int_distribution<int> uniformDistribution_;
	};

	class UniformRandomDoubleGenerator : public RandomNumberGenerator
	{
	public:

		UniformRandomDoubleGenerator(double min, double max);

		double generate();

		void changeParameters(double min, double max);

	private:

		uniform_real_distribution<double> uniformDistribution_;
	};
}