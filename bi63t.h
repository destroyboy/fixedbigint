//
// Created by Alexis Farmer on 2019-04-11.
//

#ifndef BI63T_H
#define BI63T_H

#include <stdint.h>

# define BI63_SIZE (8)

typedef struct {
    uint64_t __[BI63_SIZE];
} BI63_t;

#define ASSIGN(dst,src) *(dst) = *(src)

uint64_t BI63_isNegative(BI63_t *a);
uint64_t BI63_isPositive(BI63_t *a);

BI63_t* BI63_fromInt(BI63_t *a, uint64_t n);
BI63_t* BI63_add(BI63_t *a, BI63_t *b, BI63_t *c);
BI63_t* BI63_sub(BI63_t *a, BI63_t *b, BI63_t *c);
BI63_t* BI63_negate(BI63_t *in, BI63_t *out);
void BI63_divmod(BI63_t *m0, BI63_t *n0, BI63_t *q, BI63_t *r);
BI63_t* BI63_mul(BI63_t *a, BI63_t *b, BI63_t *c);

void BI63_lshift_bit(BI63_t *a);
void BI63_rshift_bit(BI63_t *a);
void BI63_add_int(BI63_t *a, uint64_t carry, int shift_left);

void BI63_printRawHex(BI63_t* a);
void BI63_printHex(BI63_t *a);
void BI63_printDecimal(BI63_t *n0);
BI63_t* BI63_fromString(BI63_t *n, char *s);


void BI63_random_digits(int digits, BI63_t *out);
void BI63_lenstra_test(BI63_t *n0, int limit);

#endif //BI63T_H
