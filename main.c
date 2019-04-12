#include <stdio.h>
#include <sys/time.h>

#include "bi32.h"
#include "lenstra.h"

#include "bi63t.h"

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

    BI32_t c;
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

    BI63_t c;
    for (int i=0; i<500; i++) {
        //printf("X:%d\n", i);
        BI63_random_digits(20, &c);
        //FROMSTRING(&c, "1009012267184813");
        //_seed = 3410633414;
        printf("%d ", i);
        BI63_printDecimal(&c);
        BI63_lenstra_test(&c, 500);
    }
}

void test2() {
    BI63_t a, b, c, a3, b3, c3, result;
    BI63_fromInt(&a, 8866128975287528);
    BI63_fromInt(&b, 8778405442862239);
    BI63_fromInt(&c, 2736111468807040);
    BI63_mul(&a, &a, &a3);
    BI63_mul(&a, &a3, &a3);
    BI63_mul(&b, &b, &b3);
    BI63_mul(&b, &b3, &b3);
    BI63_mul(&c, &c, &c3);
    BI63_mul(&c, &c3, &c3);
    /*V_PRINTD(&a);
    V_PRINTD(&b);
    V_PRINTD(&c);
    V_PRINTD(&a3);
    V_PRINTD(&b3);
    BI63_printDecimal(&c3);*/
    BI63_add(&a3, &b3, &result);
    BI63_add(&result, &c3, &result);
    BI63_printDecimal(&result);

}

void test3() {
    BI32_t a, b, c, a3, b3, c3, result;
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
    BI63_printDecimal(&c3);*/
    ADD(&a3, &b3, &result);
    ADD(&result, &c3, &result);
    PRINTD(&result);

}


void test5() {
    BI63_t a,b,c;
    BI63_fromInt(&a, 8866128975287528);
    BI63_fromInt(&b, 8866128975287528);
    BI63_mul(&a, &b, &c);
    BI63_printDecimal(&a);
    BI63_printDecimal(&b);
    BI63_printDecimal(&c);


}

void test6() {
    BI63_t a,b,c, d;
    BI63_fromString(&a, "100000000000000000000000000000000000000000000000");
    BI63_fromInt(&b, 7);
    BI63_divmod(&a, &b, &c, &d);
    //BI63_printDecimal(&a);
    //BI63_printDecimal(&b);
    BI63_printDecimal(&c);
}

extern uint64_t shifts;
extern uint64_t divs;

int main() {

    struct timeval stop, start;
    gettimeofday(&start, NULL);
    //test6();
    test1();
    //test2();

    gettimeofday(&stop, NULL);
    printf("took %lu\n", stop.tv_sec - start.tv_sec);



    return 0;
}