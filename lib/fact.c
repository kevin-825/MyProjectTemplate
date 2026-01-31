#include "fact.h"


uint64_t factorial(uint64_t n)
{
    if ((n == 0 ) || (n==1)) {
        return 1;
    }
    uint64_t FactNext = factorial(n - 1);
    uint64_t Fact = n * FactNext;
    return Fact;
}