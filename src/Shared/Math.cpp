#include <Shared/Math.h>

namespace TransactionScheduling
{
    int Combination(int a, int b)
    {
        if (b < 0)
        {
            return 0;
        }

        __uint128_t res = 1;

        if (b > a - b)
        {
            for (int i = a; i > b; i--)
            {
                res *= i;
            }

            for (int i = 2; i <= a - b; i++)
            {
                res /= i;
            }
        }
        else
        {
            for (int i = a; i > a - b; i--)
            {
                res *= i;
            }

            for (int i = 2; i <= b; i++)
            {
                res /= i;
            }
        }

        return (int)res;
    }

    int Factorial(int a)
    {
        int res = 1;

        for (int i = 2; i <= a; i++)
        {
            res *= i;
        }

        return res;
    }
}