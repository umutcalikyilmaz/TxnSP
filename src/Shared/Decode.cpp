#include <Shared/Decode.h>

namespace TransactionScheduling
{
    int Encode(bool* list, int n)
    {
        int res = 0;
        int dig = 1;

        for(int i = 0; i < n; i++)
        {
            if(list[i])
            {
                res += dig;
            }

            dig *= 2;
        }

        return res;
    }

    int Encode(bool* lst, int n, int r)
    {
        int res = 0;
        int u = n - 1;
        int d = r - 1;

        for (int i = 0; i < n; i++)
        {
            if (lst[i])
            {
                d--;
            }
            else
            {
                res += Combination(u, d);
            }

            u--;
        }

        return res;
    }

    bool* Decode(int code, int n, int r)
    {
        bool* res = new bool[n];
        int c = 0;
        int u = n - 1;
        int d = r - 1;

        for (int i = 0; i < n; i++)
        {
            if (c < r)
            {
                int temp = Combination(u, d);

                if (code < temp)
                {
                    res[i] = true;
                    d--;
                    c++;
                }
                else
                {
                    res[i] = false;
                    code -= temp;
                }

                u--;
            }
            else
            {
                res[i] = false;
            }
        }

        return res;
    }


    void Decode(bool* list, int code, int n, int r)
    {
        int c = 0;
        int u = n - 1;
        int d = r - 1;

        for (int i = 0; i < n; i++)
        {
            if (c < r)
            {
                int temp = Combination(u, d);

                if (code < temp)
                {
                    list[i] = true;
                    d--;
                    c++;
                }
                else
                {
                    list[i] = false;
                    code -= temp;
                }

                u--;
            }
            else
            {
                list[i] = false;
            }
        }
    }

    void Decode(__uint128_t index, int* list, int* a, int n, int* const divid)
    {
        __uint128_t nom = index;

        for (int i = 0; i < n; i++)
        {
            a[i] = i;
        }

        for (int i = 0; i < n; i++)
        {
            int ind = nom / divid[i];
            nom %= divid[i];
            list[i] = a[ind];
            DeleteElement(a, ind, n - i);
        }
    }
}