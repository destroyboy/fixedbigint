#include <stdio.h>
#include <sys/time.h>

#include "bi32.h"
#include "lenstra.h"

#include "bi63t.h"

void test_BI32_speed(int print) {
    BI32_t c;
    for (int i=0; i<500; i++) {
        k_random_digits(20, &c);
        if (print) {
            printf("%d ", i); PRINTD(&c);
        }
        LENSTRA_TEST(&c, 500, print);
    }
}

void test_BI63_speed(int print) {
    BI63_t c;
    for (int i=0; i<500; i++) {
        BI63_random_digits(20, &c);
        if (print) {
            printf("%d ", i); BI63_printDecimal(&c);
        }
        BI63_lenstra_test(&c, 500, print);
    }
}

void testBI63_33() {
    BI63_t a, b, c, a3, b3, c3, result;
    BI63_fromInt(&a, 8866128975287528);
    BI63_fromInt(&b, -8778405442862239);
    BI63_fromInt(&c, -2736111468807040);
    BI63_mul(&a, &a, &a3);
    BI63_mul(&a, &a3, &a3);
    BI63_mul(&b, &b, &b3);
    BI63_mul(&b, &b3, &b3);
    BI63_mul(&c, &c, &c3);
    BI63_mul(&c, &c3, &c3);
    BI63_add(&a3, &b3, &result);
    BI63_add(&result, &c3, &result);
    BI63_printDecimal(&result);

}

void testBI32_33() {
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

int main() {

    struct timeval stop, start;

    printf("BI32 test...");
    fflush(stdout);
    gettimeofday(&start, NULL);
    test_BI32_speed(0);
    gettimeofday(&stop, NULL);
    printf("took %lu\n", stop.tv_sec - start.tv_sec);

    printf("BI63 test...");
    fflush(stdout);
    gettimeofday(&start, NULL);
    test_BI63_speed(0);
    gettimeofday(&stop, NULL);
    printf("took %lu\n", stop.tv_sec - start.tv_sec);

    return 0;
}