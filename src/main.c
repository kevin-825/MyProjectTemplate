#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include "fact.h"

void exit_handler(void);
int main(int argc, char **argv)
{
    for (int i=0; i<argc; i++){
        printf("%d %s \n",i, argv[i]);
    }
    uint64_t n=6;
    uint64_t N = factorial(n);
    // printf("factorial of %llu is %llu \n ", n, N);
    printf("factorial of %llu is %llu\n", (unsigned long long)n, (unsigned long long)N);
    

    printf("2 factorial of %ju is %ju\n", (uintmax_t)n, (uintmax_t)N);

    printf("%llu\n", (unsigned long long)123);

    exit_handler();
}

void exit_handler(void) 
{
    printf("ERROR: main() has returned.\n");
    // while (1) {
    //     // spin forever or trigger a breakpoint
    // }
}

