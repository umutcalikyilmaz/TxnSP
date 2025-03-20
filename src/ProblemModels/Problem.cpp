#include <ProblemModels/Problem.h>

namespace TransactionScheduling
{
    Problem::Problem(int n, int m, double* t, bool** conf) : n(n), m(m), t(t), conf(conf), divid(new int[n]), size(1)
    {
        for (int i = 2; i <= n; i++)
		{
			size *= i;
		}

		for (int i = 0; i < n; i++)
		{
			divid[i] = 1;

			for (int j = 2; j < (n - i); j++)
			{
				divid[i] *= j;
			}
		}
    }

    Problem::Problem(int n, int m, double para1, double para2, double cp, Distribution dist) : n(n), m(m), size(1)
    {
        for (int i = 2; i <= n; i++)
		{
			size *= i;
		}

		t = new double[n];
		divid = new int[n];
		conf = new bool*[n];

        NormalRandomNumberGenerator* nrnd;
        UniformRandomDoubleGenerator* urnd;
        UniformRandomDoubleGenerator rnd2(0, 1);

        if(dist == NormalDistribution)
        {
            nrnd = new NormalRandomNumberGenerator(para1, para2);
        }
        else
        {
            urnd = new UniformRandomDoubleGenerator(para1, para2);
        }

        double p;

        for(int i = 0; i < n; i++)
        {
            do
            {
                t[i] = (dist == NormalDistribution) ? nrnd->Generate() : urnd->Generate();
            }
            while(t[i] < 0);
            
            conf[i] = new bool[n];
            divid[i] = 1;
            conf[i][i] = false;

            for(int j = 0; j < i; j++)
            {
                p = rnd2.Generate();
                conf[i][j] = p < cp;
            }

            for (int j = 2; j < (n - i); j++)
			{
				divid[i] *= j;
			}
        }

        for (int i = 0; i < n; i++)
		{
			for (int j = i + 1; j < n; j++)
			{
				conf[i][j] = conf[j][i];
			}
		}

        if(dist == NormalDistribution)
        {
            delete nrnd;
        }
        else
        {
            delete urnd;
        }
    }

    int Problem::GetN() const
    {
        return n;
    }
		
	int Problem::GetM() const
    {
        return m;
    }

	double* Problem::GetT() const
    {
        return t;
    }

    double Problem::GetT(int ind) const
    {
        return t[ind];
    }

    bool** Problem::GetConf() const
    {
        return conf;
    }

	int* Problem::GetDivid() const
    {
        return divid;
    }

	__uint128_t Problem::GetSize() const
    {
        return size;
    }

    void Problem::ArrangeConf()
    {
        for (int i = 0; i < n; i++)
		{
            conf[i][i] = 0;
            
			for (int j = i + 1; j < n; j++)
			{
				conf[i][j] = conf[j][i];
			}
		}
    }

    Problem::~Problem()
    {
        delete[] t;

        for(int i = 0; i < n; i++)
        {
            delete[] conf[i];
        }

        delete[] conf;
        delete[] divid;
    }
}