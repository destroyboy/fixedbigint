#include <stdio.h>
#include <sys/time.h>

#include "bigint.h"
#include "lenstra.h"

#include "varint.h"

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

void test1v() {

    V c;
    for (int i=0; i<500; i++) {
        //printf("X:%d\n", i);
        v_random_digits(20, &c);
        //FROMSTRING(&c, "1009012267184813");
        //_seed = 3410633414;
        printf("%d ", i); V_PRINTD(&c);
        V_LENSTRA_TEST(&c, 500);
    }
}

void test2() {
    V a, b, c, a3, b3, c3, result;
    V_fromInt(&a, 8866128975287528);
    V_fromInt(&b, 8778405442862239);
    V_fromInt(&c, 2736111468807040);
    V_MUL(&a, &a, &a3);
    V_MUL(&a, &a3, &a3);
    V_MUL(&b, &b, &b3);
    V_MUL(&b, &b3, &b3);
    V_MUL(&c, &c, &c3);
    V_MUL(&c, &c3, &c3);
    /*V_PRINTD(&a);
    V_PRINTD(&b);
    V_PRINTD(&c);
    V_PRINTD(&a3);
    V_PRINTD(&b3);
    V_PRINTD(&c3);*/
    V_add(&a3, &b3, &result);
    V_add(&result, &c3, &result);
    V_PRINTD(&result);

}

void test3() {
    I a, b, c, a3, b3, c3, result;
    FROMINT(&a, 8866128975287528);
    FROMINT(&b, 8778405442862239);
    FROMINT(&c, 2736111468807040);
    MUL(&a, &a, &a3);
    MUL(&a, &a3, &a3);
    MUL(&b, &b, &b3);
    MUL(&b, &b3, &b3);
    MUL(&c, &c, &c3);
    MUL(&c, &c3, &c3);
    /*V_PRINTD(&a);
    V_PRINTD(&b);
    V_PRINTD(&c);
    V_PRINTD(&a3);
    V_PRINTD(&b3);
    V_PRINTD(&c3);*/
    ADD(&a3, &b3, &result);
    ADD(&result, &c3, &result);
    PRINTD(&result);

}


void test5() {
    V a,b,c;
    V_fromInt(&a, 8866128975287528);
    V_fromInt(&b, 8866128975287528);
    V_MUL(&a, &b, &c);
    V_PRINTD(&a);
    V_PRINTD(&b);
    V_PRINTD(&c);


}

int main() {

    struct timeval stop, start;
    gettimeofday(&start, NULL);
    test1v();
    //test2();

    //printf("count=%llu\n", count);
    //for (int i=0; i <1; i++)
    //    test2();

    gettimeofday(&stop, NULL);
    printf("took %lu\n", stop.tv_sec - start.tv_sec);



    return 0;
}