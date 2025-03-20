#pragma once
#include <vector>
#include <Shared/List.h>
#include <Shared/Math.h>

namespace TransactionScheduling
{
    int Encode(bool* list, int n);

    int Encode(bool* lst, int n, int r);

    bool* Decode(int code, int n, int r);

    void Decode(bool* list, int code, int n, int r);

    void Decode(__uint128_t index, int* list, int* a, int n, int* const divid);
}