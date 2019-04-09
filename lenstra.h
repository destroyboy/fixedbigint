//
// Created by Alexis Farmer on 2019-04-08.
//

#ifndef UNTITLED_LENSTRA_H
#define UNTITLED_LENSTRA_H

#include "bigint.h"

#define PRIMEMAX (1000)

void PRIMES();
void LENSTRA(I* n, int limit, I* out);
void LENSTRA_TEST(I *n, int limit);
void ELLIPTIC_ADD(I p[3], I q[3], I* a, I* b, I* m, I out[3]);

void k_random_digits(int digits, I* out);

#endif //UNTITLED_LENSTRA_H
