//
// Created by Alexis Farmer on 2019-04-08.
//

#include "bi32.h"

#include <string.h>
#include <stdio.h>
#include <assert.h>

void BI32_lshift_bit(BI32_t *a) {
    uint32_t carry=0;
    for (int i=0; i<BI32_SIZE; i++) {
        uint32_t result = carry | (a->_[i]<<1);
        carry = a->_[i]>>31;
        a->_[i] = result;
    }
}

void BI32_rshift_bit(BI32_t *a) {
    uint32_t carry=0;
    for (int i=BI32_SIZE-1; i>=0; i--) {
        uint32_t result = carry | (a->_[i]>>1);
        carry = a->_[i]<<31;
        a->_[i] = result;
    }
}

int ISNEGATIVE(BI32_t* a) {
    return (a->_[BI32_SIZE-1]>>31);
}

int ISPOSITIVE(BI32_t* a) {
    return !(a->_[BI32_SIZE-1]>>31);
}

BI32_t* ADD(BI32_t *a, BI32_t *b, BI32_t *c) {
    uint64_t carry = 0;
    for (int i=0; i<BI32_SIZE; i++) {
        carry += (uint64_t)a->_[i] + (uint64_t)b->_[i];
        c->_[i] = carry;
        carry >>= 32u;
    }
    return c;
}

void ADD_INT(BI32_t *a, uint64_t carry, int shift_left) {
    for (int i=shift_left; i<BI32_SIZE; i++) {
        carry += (uint64_t)a->_[i];
        a->_[i] = carry;
        carry >>= 32u;
        if (carry==0)
            break;
    }
}

BI32_t* SUB(BI32_t *a, BI32_t *b, BI32_t *c) {
    uint64_t carry = 1;
    for (int i=0; i<BI32_SIZE; i++) {
        carry += (uint64_t)a->_[i] + (uint64_t)~b->_[i];
        c->_[i] = carry;
        carry >>= 32u;
    }
    return c;
}

BI32_t* FROMINT(BI32_t *a, int64_t n) {
    memset(a, 0, sizeof(*a));
    a->_[0] = n;
    a->_[1] = n>>32;
    if (n<0)
        for (int i=2; i<BI32_SIZE; i++)
            a->_[i] = -1;
    return a;
}

void NEGATE_INPLACE(BI32_t *n) {
    BI32_t one;
    FROMINT(&one, 1);
    for (int i=0; i<BI32_SIZE; i++)
        n->_[i] = ~n->_[i];
    ADD(n, &one, n);
}

BI32_t* NEGATE(BI32_t *n0, BI32_t* out) {
    BI32_t n;
    ASSIGN(&n, n0);
    FROMINT(out, 1);
    for (int i=0; i<BI32_SIZE; i++)
        n._[i] = ~n._[i];
    ADD(&n, out, out);
    return out;
}

BI32_t* MUL(BI32_t *a, BI32_t *b, BI32_t *c) {
    BI32_t r;
    FROMINT(&r, 0);
    for (int i = 0; i < BI32_SIZE; ++i) {
        uint64_t bb = (uint64_t )b->_[i];
        if (bb==0)
            continue;
        for (int j = 0; j < BI32_SIZE-i; ++j) {
            uint64_t aa = (uint64_t )a->_[j];
            if (aa==0)
                continue;
            ADD_INT(&r, bb * aa, i+j);
        }
    }
    ASSIGN(c, &r);
    return c;
}


int ISZERO(BI32_t* a) {
    for (int i=0; i<BI32_SIZE; i++)
        if (a->_[i]!=0)
            return 0;
    return 1;
}

int COMPARE(BI32_t* a, BI32_t* b) {
    BI32_t c;
    SUB(a, b, &c);
    return ISNEGATIVE(&c) ? -1 : (ISZERO(&c)?0:1);
}

int COMPARE_UNSIGNED(BI32_t* a, BI32_t* b) {
    for (int i = BI32_SIZE-1; i>=0; i--) {
        int64_t c = (uint64_t )a->_[i] - (uint64_t )b->_[i];
        if (c==0)
            continue;
        if (c<0)
            return -1;
        else
            return 1;
    }
    return 0;
}

void DIVMOD0(BI32_t* m0, BI32_t* n0, BI32_t* q, BI32_t* r) {
    //BI32_t shift;
    BI32_t n;

    int32_t s0 = 0;

    memset(q, 0, sizeof(BI32_t));
    ASSIGN(r, m0);
    ASSIGN(&n, n0);

    while (COMPARE_UNSIGNED( r, &n ) > 0) {
        s0++;
        BI32_lshift_bit(&n);
    }

    for (;;) {
        BI32_t t;
        SUB(r, &n, &t);
        if (ISPOSITIVE(&t)) {
            ASSIGN(r, &t);
            ADD_INT(q, 1u<<(s0&31u), s0>>5u);
        }
        if (s0==0)
            break;
        s0--;
        BI32_rshift_bit(&n);
    }
}

BI32_t* MOD0(BI32_t* m, BI32_t *n, BI32_t *r) {
    BI32_t q;
    DIVMOD0(m, n, &q, r);
    return r;
}

BI32_t* MOD(BI32_t* m, BI32_t *n, BI32_t *r) {

    BI32_t w1, w2, w3;

    if (!ISNEGATIVE(m) && !ISNEGATIVE(n))
        return MOD0(m, n, r);

    if (!ISNEGATIVE(m) && ISNEGATIVE(n))
        return ADD(n, MOD0(m, NEGATE(n, &w2), &w1), r);

    if (ISNEGATIVE(m) && !ISNEGATIVE(n))
        return SUB(n,MOD0(NEGATE(m, &w2), n, &w1), r);

    // both negative
    return NEGATE( MOD0( NEGATE(m, &w1), NEGATE(n, &w2), &w3), r);
}

BI32_t* DIV(BI32_t *m0, BI32_t*n0, BI32_t* q) {

    BI32_t m, n, w1, w2, r;
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

void BI32_gcd(BI32_t *a0, BI32_t *b0, BI32_t *gcd) {
    BI32_t a, b, r;
    ASSIGN(&a, a0);
    ASSIGN(&b, b0);
    while (!ISZERO(&b)) {
        MOD( &a, &b, &r );
        ASSIGN(&a, &b);
        ASSIGN(&b, &r);
    }
    ASSIGN(gcd, &a);
}



void BI32_toString(BI32_t *n0, char *s) {
    BI32_t n, ten;
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
        BI32_t q, r;
        DIVMOD0(&n, &ten, &q, &r);
        *--ptr = '0'+r._[0];
        ASSIGN(&n, &q);
    }
    if (ISNEGATIVE(n0))
        *--ptr = '-';

    strcpy(s, ptr);
}

BI32_t* FROMSTRING(BI32_t *n, char *s) {
    FROMINT(n, 0);
    BI32_t digit, ten, result;
    FROMINT(&digit, 0);
    FROMINT(&ten, 10);
    FROMINT(&result, 0);
    int negative = 0;

    if (*s=='-') {
        s++;
        negative = 1;
    }

    while (*s!='\0') {
        BI32_t temp;
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

void PRINTHEX(BI32_t* a) {
    printf("0x");
    for (int i=BI32_SIZE-1; i>=0; i--) {
        printf("%08X", a->_[i]);
    }
    printf("\n");
}

void PRINTD(BI32_t *n0) {
    char buf[256];
    BI32_toString(n0, buf);
    printf("%s\n", buf);
}

void PRINTD_(BI32_t *n0) {
    char buf[256];
    BI32_toString(n0, buf);
    printf("%s", buf);
}