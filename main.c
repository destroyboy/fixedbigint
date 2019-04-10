#include <stdio.h>
#include <sys/time.h>

#include "bigint.h"
#include "lenstra.h"

void testdiv(uint64_t N, uint64_t D, uint64_t *Q, uint64_t *R) {
    *R = N;

    D <<= 32u;
    *Q = 0;

    for (int n=31; n>=0; n--) {
        if (R >= 0) {
           *Q|=(1u<<(unsigned)n);
           *R = (*R<<1u)-D;
        }
        else {
            *R = (*R<<1u)+D;
        }
    }
}

extern long _seed;

void test1() {

    I c;
    for (int i=0; i<500; i++) {
        //printf("X:%d\n", i);
        k_random_digits(20, &c);
        //FROMSTRING(&c, "1009012267184813");
        //_seed = 3410633414;
        printf("%d ", i); PRINTD(&c);
        LENSTRA_TEST(&c, 500);
    }
}

//extern uint64_t count;

int main() {

    struct timeval stop, start;
    gettimeofday(&start, NULL);
//    count = 0;
    test1();

    //printf("count=%llu\n", count);

    gettimeofday(&stop, NULL);
    printf("took %lu\n", stop.tv_sec - start.tv_sec);

    //

    return 0;
}