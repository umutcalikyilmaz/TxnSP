#pragma once
#include <random>
#include <cmath>
#include <cstdlib>
#include <chrono>
#include <boost/multiprecision/cpp_int.hpp>

namespace TransactionScheduling
{
    class RandomNumberGenerator
	{
	protected:
		std::minstd_rand generator;

		RandomNumberGenerator();
	};

	class NormalRandomNumberGenerator : public RandomNumberGenerator
	{
	private:
		std::normal_distribution<double> normalDistribution;

	public:

		NormalRandomNumberGenerator(double mean, double std);

		double Generate();

		void ChangeParams(double mean, double std);
	};

	class UniformRandomIntGenerator : public RandomNumberGenerator
	{
	private:

		std::uniform_int_distribution<int> uniformDistribution;

	public:

		UniformRandomIntGenerator(int min, int max);

		int Generate();

		void ChangeParams(int min, int max);
	};

	class UniformRandomDoubleGenerator : public RandomNumberGenerator
	{
	private:

		std::uniform_real_distribution<double> uniformDistribution;

	public:

		UniformRandomDoubleGenerator(double min, double max);

		double Generate();

		void ChangeParams(double min, double max);
	};

	
	class UniformRandomInt128Generator : public RandomNumberGenerator
	{
	private:

		std::uniform_int_distribution<__uint128_t> uniformDistribution;

	public:

		UniformRandomInt128Generator(__uint128_t min, __uint128_t max);

		__uint128_t Generate();

		void ChangeParams(__uint128_t min, __uint128_t max);
	};
}