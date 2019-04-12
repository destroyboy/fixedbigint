//
// Created by Alexis Farmer on 2019-04-08.
//

#ifndef UNTITLED_BIGINT_H
#define UNTITLED_BIGINT_H

#include "stdint.h"

# define BIGINT_WORDS (16)

typedef struct {
    uint32_t _[BIGINT_WORDS];
} I;

#define ASSIGN(dst,src) *(dst) = *(src)

I* FROMINT(I *a, int64_t n);
void NEGATE_INPLACE(I *n);
I* NEGATE(I *n, I *out);

int ISNEGATIVE(I* a);
int ISZERO(I* a);
int COMPARE(I* a, I* b);
int COMPARE_UNSIGNED(I* a, I* b);

I* ADD(I *a, I *b, I *c);
I* SUB(I *a, I *b, I *c);
I* MUL(I *a, I *b, I *c);
I* MUL_INT(I *a, uint64_t b, I *c);
I* MULx2(I *src, I* dst);
I* MULx3(I *src, I*dst);
I* MULx4(I *src, I* dst);
I* MULx27(I *src, I*dst);
void DIVx2(I *a);
void DIVMOD0(I* m0, I* n0, I* q, I* r);
I* DIV(I *m, I*n, I* q);
I* MOD0(I* m, I *n, I *r);
I* MOD(I* m, I *n, I *r);

void GCD(I* a, I* b, I* gcd);

void TOSTRING(I *n0, char *s);
I* FROMSTRING(I *n, char *s);

void PRINTHEX(I* a);
void PRINTD(I *n0);
void PRINTD_(I *n0);

#endif //UNTITLED_BIGINT_H
