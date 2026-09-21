#pragma once
#include <vector>

namespace TxnSP
{
    inline int combination(int a, int b)
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

    inline int factorial(int a)
    {
        int res = 1;

        for (int i = 2; i <= a; i++)
        {
            res *= i;
        }

        return res;
    }

    inline void deleteElement(std::vector<int>& lst, int pos, int sz)
    {
        for (int i = pos; i < sz - 1; i++)
        {
            lst[i] = lst[i + 1];
        }
    }    
   
    inline void copy(const std::vector<bool>& sub, std::vector<bool>& obj)
    {
        int c = 0;

        for(bool b : sub)
        {
            obj[c] = b;
            c++;
        }
    }    
    
    inline void copy(const std::vector<int>& sub, std::vector<int>& obj, int size)
    {
        for(int i = 0; i < size; i++)
        {
            obj[i] = sub[i];
        }
    }    

    inline void shift(std::vector<int>& list, int ind)
    {
        for(int i = 0; i < ind; i++)
		{
			list[i] = list[i + 1];
		}
    }

    inline void shift(std::vector<int>& list, int ind, int size)
    {
        for(int i = size - 1; i >= ind; i--)
		{
			list[i + 1] = list[i];
		}	
    }

    inline int findPlace(const std::vector<double>& list, const std::vector<int>& inds, double val, int size)
    {
        int ul = size - 1;
		int ll = 0; 

		if(val >= list[inds[ul]])
		{
			return ul;
		}

		int diff = ul - ll;

		while(diff > 1)
		{
			int curr = diff / 2 + ll;

			if(val < list[inds[curr]])
			{
				ul = curr;
			}
			else if(val > list[inds[curr]])
			{
				ll = curr;
			}
			else
			{
				return curr;
			}

			diff = ul - ll;
		}		

		return ll;
    }

    inline int findPlace2(const std::vector<double>& list, const std::vector<int>& inds, double val, int size)
    {
        if(size == 0)
		{
			return 0;
		}
		
		int ul = size;
		int ll = 0; 
		int diff = ul - ll;		

		while(diff > 1)
		{
			int curr = diff / 2 + ll;

			if(val < list[inds[curr]])
			{
				ul = curr;
			}
			else if(val > list[inds[curr]])
			{
				ll = curr;
			}
			else
			{
				return curr;
			}

			diff = ul - ll;
		}

		if(val > list[inds[ll]])
		{
			return ul;
		}

		return ll;
    }

    inline int encode(const std::vector<uint8_t>& lst, int n, int r)
    {
        int res = 0;
        int u = n - 1;
        int d = r - 1;

        for (int i = 0; i < n; i++)
        {
            if (lst[i] == 1)
            {
                d--;
            }
            else
            {
                res += combination(u, d);
            }

            u--;
        }

        return res;
    }

    inline bool* decode(int code, int n, int r)
    {
        bool* res = new bool[n];
        int c = 0;
        int u = n - 1;
        int d = r - 1;

        for (int i = 0; i < n; i++)
        {
            if (c < r)
            {
                int temp = combination(u, d);

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

    inline void decode(std::vector<uint8_t>& list, int code, int n, int r)
    {
        int c = 0;
        int u = n - 1;
        int d = r - 1;

        for (int i = 0; i < n; i++)
        {
            if (c < r)
            {
                int temp = combination(u, d);

                if (code < temp)
                {
                    list[i] = 1;
                    d--;
                    c++;
                }
                else
                {
                    list[i] = 0;
                    code -= temp;
                }

                u--;
            }
            else
            {
                list[i] = 0;
            }
        }
    }

    inline void decode(LargeInt index, std::vector<int>& list, std::vector<int>& a, int n,
        const std::vector<LargeInt>& divid)
    {
        LargeInt nom = index;

        for (int i = 0; i < n; i++)
        {
            a[i] = i;
        }

        for (int i = 0; i < n; i++)
        {
            int ind = (int)(nom / divid[i]);
            nom %= divid[i];
            list[i] = a[ind];
            deleteElement(a, ind, n - i);
        }
    }
}