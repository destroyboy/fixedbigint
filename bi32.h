//
// Created by Alexis Farmer on 2019-04-08.
//

#ifndef BI32_H
#define BI32_H

#include "stdint.h"

# define BI32_SIZE (16)

typedef struct {
    uint32_t _[BI32_SIZE];
} BI32_t;

#define ASSIGN(dst,src) *(dst) = *(src)

BI32_t* FROMINT(BI32_t *a, int64_t n);
void NEGATE_INPLACE(BI32_t *n);
BI32_t* NEGATE(BI32_t *n, BI32_t *out);

int ISNEGATIVE(BI32_t* a);
int ISPOSITIVE(BI32_t* a);
int ISZERO(BI32_t* a);
int COMPARE(BI32_t* a, BI32_t* b);
int COMPARE_UNSIGNED(BI32_t* a, BI32_t* b);

void BI32_lshift_bit(BI32_t *a);
void BI32_rshift_bit(BI32_t *a);

BI32_t* ADD(BI32_t *a, BI32_t *b, BI32_t *c);
BI32_t* SUB(BI32_t *a, BI32_t *b, BI32_t *c);
BI32_t* MUL(BI32_t *a, BI32_t *b, BI32_t *c);
//void DIVx2(BI32_t *a);
void DIVMOD0(BI32_t* m0, BI32_t* n0, BI32_t* q, BI32_t* r);
BI32_t* DIV(BI32_t *m, BI32_t*n, BI32_t* q);
BI32_t* MOD0(BI32_t* m, BI32_t *n, BI32_t *r);
BI32_t* MOD(BI32_t* m, BI32_t *n, BI32_t *r);

void BI32_gcd(BI32_t *a0, BI32_t *b0, BI32_t *gcd);

void BI32_toString(BI32_t *n0, char *s);
BI32_t* FROMSTRING(BI32_t *n, char *s);

void PRINTHEX(BI32_t* a);
void PRINTD(BI32_t *n0);
void PRINTD_(BI32_t *n0);

#endif //BI32_H
