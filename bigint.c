//
// Created by Alexis Farmer on 2019-04-08.
//

#include "bigint.h"

#include <string.h>
#include <stdio.h>
#include <assert.h>

static void _lshift_bit(I *a) {
    uint32_t carry=0;
    for (int i=0; i<WORDS; i++) {
        uint32_t result = carry | (a->_[i]<<1);
        carry = a->_[i]>>31;
        a->_[i] = result;
    }
}

static void _lshift_bits(I *a, int n) {
    uint32_t carry=0;
    for (int i=0; i<WORDS; i++) {
        uint32_t result = carry | (a->_[i]<<n);
        carry = a->_[i]>>(32-n);
        a->_[i] = result;
    }
}

static void _rshift_bit(I *a) {
    uint32_t carry=0;
    for (int i=WORDS-1; i>=0; i--) {
        uint32_t result = carry | (a->_[i]>>1);
        carry = a->_[i]<<31;
        a->_[i] = result;
    }
}

I* FROMINT_NOSIGNEXTENSION(I *a, int64_t n) {
    memset(a, 0, sizeof(*a));
    a->_[0] = n;
    a->_[1] = n>>32;
    return a;
}

int ISNEGATIVE(I* a) {
    return (a->_[WORDS-1]>>31);
}

I* ADD(I *a, I *b, I *c) {
    uint64_t carry = 0;
    for (int i=0; i<WORDS; i++) {
        carry += (uint64_t)a->_[i] + (uint64_t)b->_[i];
        c->_[i] = carry;
        carry >>= 32;
    }
    return c;
}

void ADD_INT(I *a, uint64_t carry, int shift_left) {
    for (int i=shift_left; i<WORDS; i++) {
        carry += (uint64_t)a->_[i];
        a->_[i] = carry;
        carry >>= 32;
        if (carry==0)
            break;
    }
}

I* SUB(I *a, I *b, I *c) {
    uint64_t carry = 1;
    for (int i=0; i<WORDS; i++) {
        carry += (uint64_t)a->_[i] + (uint64_t)~b->_[i];
        c->_[i] = carry;
        carry >>= 32;
    }
    return c;
}

I* FROMINT(I *a, int64_t n) {
    memset(a, 0, sizeof(*a));
    a->_[0] = n;
    a->_[1] = n>>32;
    if (n<0)
        for (int i=2; i<WORDS; i++)
            a->_[i] = -1;
    return a;
}

void NEGATE_INPLACE(I *n) {
    I one;
    FROMINT(&one, 1);
    for (int i=0; i<WORDS; i++)
        n->_[i] = ~n->_[i];
    ADD(n, &one, n);
}

I* NEGATE(I *n0, I* out) {
    I n;
    ASSIGN(&n, n0);
    FROMINT(out, 1);
    for (int i=0; i<WORDS; i++)
        n._[i] = ~n._[i];
    ADD(&n, out, out);
    return out;
}

I* MUL(I *a, I *b, I *c) {
    I r;
    FROMINT(&r, 0);
    for (int i = 0; i < WORDS; ++i) {
        for (int j = 0; j < WORDS-i; ++j)
            ADD_INT(&r, (uint64_t )b->_[i] * (uint64_t)a->_[j], i+j);
    }

    ASSIGN(c, &r);
    return c;
}


int ISZERO(I* a) {
    for (int i=0; i<WORDS; i++)
        if (a->_[i]!=0)
            return 0;
    return 1;
}

int COMPARE(I* a, I* b) {
    I c;
    SUB(a, b, &c);
    return ISNEGATIVE(&c) ? -1 : (ISZERO(&c)?0:1);
}

void DIVx2(I *a) {
    uint32_t sign = a->_[WORDS-1];
    _rshift_bit(a);
    a->_[WORDS-1] |= sign;
}

void DIVMOD0(I* m0, I* n0, I* q, I* r) {
    I shift;
    I m,n;

    FROMINT(q, 0);
    ASSIGN(&m, m0);
    ASSIGN(&n, n0);
    FROMINT(&shift, 1);

    I test;
    SUB(&m, &n, &test);
    //printf("test:");PRINT(&test);

    while (COMPARE(&m, &n)>0) {
        _lshift_bit(&shift);
        _lshift_bit(&n);
    }
    for (;;) {
        if (COMPARE(&n,&m)<=0) {
            SUB(&m, &n, &m);
            ADD(q, &shift, q);
        }
        _rshift_bit(&shift);
        _rshift_bit(&n);
        if (ISZERO(&shift))
            break;
    }
    ASSIGN(r, &m);
}

I* MOD0(I* m, I *n, I *r) {
    I q;
    DIVMOD0(m, n, &q, r);
    return r;
}

I* MOD(I* m, I *n, I *r) {

    I w1, w2, w3;

    if (!ISNEGATIVE(m) && !ISNEGATIVE(n))
        return MOD0(m, n, r);

    if (!ISNEGATIVE(m) && ISNEGATIVE(n))
        return ADD(n, MOD0(m, NEGATE(n, &w2), &w1), r);

    if (ISNEGATIVE(m) && !ISNEGATIVE(n))
        return SUB(n,MOD0(NEGATE(m, &w2), n, &w1), r);

    // both negative
    return NEGATE( MOD0( NEGATE(m, &w1), NEGATE(n, &w2), &w3), r);
}

I* DIV(I *m0, I*n0, I* q) {

    I m, n, w1, w2, r;
    ASSIGN(&m, m0);
    ASSIGN(&n, n0);

    if (!ISNEGATIVE(&m) && !ISNEGATIVE(&n)) {
        DIVMOD0(&m, &n, q, &r);
    }
    else if (!ISNEGATIVE(&m) && ISNEGATIVE(&n)) {
        DIVMOD0(&m, NEGATE(&n, &w1), q, &r);
        NEGATE_INPLACE(q);
    }
    else if (ISNEGATIVE(&m) && !ISNEGATIVE(&n)) {
        DIVMOD0(NEGATE(&m, &w1), &n, q, &r);
        NEGATE_INPLACE(q);
    }
    else
        DIVMOD0(NEGATE(&m, &w1), NEGATE(&n, &w2), q, &r);

    return q;
}

I* MULx2(I *src, I* dst) {
    ASSIGN(dst, src);
    _lshift_bit(dst);
    return dst;
}

I* MULx4(I *src, I* dst) {
    ASSIGN(dst, src);
    _lshift_bits(dst,2);
    return dst;
}

I* MULx3(I *src, I*dst) {
    ASSIGN(dst, src);
    _lshift_bit(dst);
    ADD(dst, src, dst);
    return dst;
}

I* MULx27(I *src, I*dst) {
    ASSIGN(dst, src);
    _lshift_bits(dst, 3);
    ADD(dst, src, dst);
    I tmp;
    ASSIGN(&tmp, dst);
    _lshift_bit(dst);
    ADD(&tmp, dst, dst);
    return dst;
}

void GCD(I* a0, I* b0, I* gcd) {
    I a, b, r;
    ASSIGN(&a, a0);
    ASSIGN(&b, b0);
    while (!ISZERO(&b)) {
        MOD( &a, &b, &r );
        ASSIGN(&a, &b);
        ASSIGN(&b, &r);
    }
    ASSIGN(gcd, &a);
}



void TOSTRING(I *n0, char *s) {
    I n, ten;
    ASSIGN(&n, n0);
    FROMINT(&ten, 10);
    char buf[256], *ptr = &buf[255];

    *ptr = '\0';

    if (ISZERO(&n)) {
        strcpy(s, "0");
        return;
    }

    if (ISNEGATIVE(n0))
        NEGATE_INPLACE(&n);

    while (!ISZERO(&n)) {
        I q, r;
        DIVMOD0(&n, &ten, &q, &r);
        *--ptr = '0'+r._[0];
        ASSIGN(&n, &q);
    }
    if (ISNEGATIVE(n0))
        *--ptr = '-';

    strcpy(s, ptr);
}

I* FROMSTRING(I *n, char *s) {
    FROMINT(n, 0);
    I digit, ten, result;
    FROMINT(&digit, 0);
    FROMINT(&ten, 10);
    FROMINT(&result, 0);
    int negative = 0;

    if (*s=='-') {
        s++;
        negative = 1;
    }

    while (*s!='\0') {
        I temp;
        digit._[0] = *s++ -'0';
        //PRINT(&digit);
        MUL(&result, &ten, &temp);
        ADD(&temp, &digit, &result);
    }
    ASSIGN(n, &result);
    if (negative)
        NEGATE_INPLACE(n);
    return n;
}

void PRINTHEX(I* a) {
    printf("0x");
    for (int i=WORDS-1; i>=0; i--) {
        printf("%08X", a->_[i]);
    }
    printf("\n");
}

void PRINTD(I *n0) {
    char buf[256];
    TOSTRING(n0, buf);
    printf("%s\n", buf);
}

void PRINTD_(I *n0) {
    char buf[256];
    TOSTRING(n0, buf);
    printf("%s", buf);
}