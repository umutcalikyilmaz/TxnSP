#include <Shared/List.h>

namespace TransactionScheduling
{
    void DeleteElement(int* lst, int pos, int sz)
    {
        for (int i = pos; i < sz - 1; i++)
        {
            lst[i] = lst[i + 1];
        }
    }

    void Copy(std::vector<bool> sub, std::vector<bool> obj)
    {
        int c = 0;

        for(bool b : sub)
        {
            obj[c] = b;
            c++;
        }
    }

    void Copy(int* sub, int* obj, int size)
    {
        for(int i = 0; i < size; i++)
        {
            obj[i] = sub[i];
        }
    }

    void Shift(int* list, int ind)
	{
		for(int i = 0; i < ind; i++)
		{
			list[i] = list[i + 1];
		}
	}

    void Shift(int* list, int ind, int size)
	{
		for(int i = size - 1; i >= ind; i--)
		{
			list[i + 1] = list[i];
		}	
	}

    int FindPlace(double* list, int* inds, double val, int size)
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

	int FindPlace2(double* list, int* inds, double val, int size)
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

}