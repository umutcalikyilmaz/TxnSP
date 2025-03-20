#pragma once
#include <vector>

namespace TransactionScheduling
{
    void DeleteElement(int* lst, int pos, int sz);

    void Copy(std::vector<bool> sub, std::vector<bool> obj);

    void Copy(int* sub, int* obj, int size);

    void Shift(int* list, int ind);

    void Shift(int* list, int ind, int size);

    int FindPlace(double* list, int* inds, double val, int size);

    int FindPlace2(double* list, int* inds, double val, int size);
}