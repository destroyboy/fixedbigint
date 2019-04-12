//
// Created by Alexis Farmer on 2019-04-08.
//

#ifndef UNTITLED_LENSTRA_H
#define UNTITLED_LENSTRA_H

#include "bi32.h"

#define PRIMEMAX (1000)

void PRIMES();
void LENSTRA(BI32_t* n, int limit, BI32_t* out);
void LENSTRA_TEST(BI32_t *n, int limit);
void ELLIPTIC_ADD(BI32_t p[3], BI32_t q[3], BI32_t* a, BI32_t* b, BI32_t* m, BI32_t out[3]);

void k_random_digits(int digits, BI32_t* out);

#endif //UNTITLED_LENSTRA_H
