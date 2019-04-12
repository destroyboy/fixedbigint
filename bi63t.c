//
// Created by Alexis Farmer on 2019-04-11.
//

#include "bi63t.h"

#include <stdio.h>
#include <string.h>
#include <assert.h>

BI63_t* BI63_fromInt(BI63_t *a, uint64_t n) {
    a->__[0] = n & 0x7ffffffffffffffful;
    if ( n & 0x8000000000000000ul ) {
        for (int i = 1; i<BI63_SIZE; i++)
            a->__[i] = 0x7fffffffffffffff;
    }
    else {
        for (int i = 1; i<BI63_SIZE; i++)
            a->__[i] = 0;
    }
    return a;
}

BI63_t* BI63_add(BI63_t *a, BI63_t *b, BI63_t *c) {
    uint64_t carry = 0;
    int i;
    for (i=0; i<BI63_SIZE; i++) {
        carry += a->__[i] + b->__[i];
        c->__[i] = carry & 0x7fffffffffffffffu;
        carry >>= 63u;
    }
    return c;
}

BI63_t* BI63_sub(BI63_t *a, BI63_t *b, BI63_t *c) {
    uint64_t carry = 1;
    for (int i=0; i<BI63_SIZE; i++) {
        carry += a->__[i] + (~b->__[i] & 0x7fffffffffffffffu);
        c->__[i] = carry & 0x7fffffffffffffffu;
        carry >>= 63u;
    }
    return c;
}

int BI63_isZero(BI63_t *a) {
    for (int i=0; i<BI63_SIZE; i++)
        if (a->__[i]!=0)
            return 0;
    return 1;
}

int BI63_compare(BI63_t *a, BI63_t *b) {
    for (int i = BI63_SIZE-1; i>=0; i--) {
        int64_t c = a->__[i] - b->__[i];
        if (c==0)
            continue;
        if (c<0)
            return -1;
        else
            return 1;
    }
    return 0;
}

void BI63_lshift_bit(BI63_t *a) {
    uint64_t carry=0;
    for (int i=0; i<BI63_SIZE; i++) {
        uint64_t result = carry | (a->__[i]<<1u);
        carry = (a->__[i]>>62u)&1u;
        a->__[i] = result&0x7fffffffffffffffu;
    }
}

void BI63_rshift_bit(BI63_t *a) {
    uint64_t carry=0;
    for (int i=BI63_SIZE-1; i>=0; i--) {
        uint64_t result = carry | (a->__[i]>>1u);
        carry = (a->__[i]<<62u)&0x4000000000000000u;
        a->__[i] = result&0x7fffffffffffffffu;
    }
}

void BI63_add_int(BI63_t *a, uint64_t carry, int shift_left) {
    for (int i=shift_left; i<BI63_SIZE; i++) {
        carry += a->__[i];
        a->__[i] = carry & 0x7fffffffffffffffu;
        carry >>= 63u;
        if (carry==0)
            break;
    }
}

void BI63_divmod(BI63_t *m0, BI63_t *n0, BI63_t *q, BI63_t *r) {
    BI63_t n;

    unsigned int ss0 = 0;

    memset(q, 0, sizeof(BI63_t));
    ASSIGN(r, m0);
    ASSIGN(&n, n0);

    while (BI63_compare(r, &n) > 0) {
        ss0++;
        if ((ss0&63u)==63u)
            ss0++;
        BI63_lshift_bit(&n);
    }

    for (;;) {
        BI63_t t;
        BI63_sub(r, &n, &t);

        if (BI63_isPositive(&t)) {
            ASSIGN(r, &t);
            BI63_add_int(q, 1ull << (ss0 & 63u), ss0 >> 6u);
        }
        if (ss0==0)
            break;
        ss0--;
        if ((ss0&63u)==63u)
            ss0--;
        BI63_rshift_bit(&n);
    }
}

BI63_t* BI63_mod0(BI63_t *m, BI63_t *n, BI63_t *r) {
    BI63_t q;
    BI63_divmod(m, n, &q, r);
    return r;
}

BI63_t* BI63_mod(BI63_t *m, BI63_t *n, BI63_t *r) {

    BI63_t w1, w2, w3;

    if (!BI63_isNegative(m) && !BI63_isNegative(n))
        return BI63_mod0(m, n, r);

    if (!BI63_isNegative(m) && BI63_isNegative(n))
        return BI63_add(n, BI63_mod0(m, BI63_negate(n, &w2), &w1), r);

    if (BI63_isNegative(m) && !BI63_isNegative(n))
        return BI63_sub(n, BI63_mod0(BI63_negate(m, &w2), n, &w1), r);

    // both negative
    return BI63_negate(BI63_mod0(BI63_negate(m, &w1), BI63_negate(n, &w2), &w3), r);
}

BI63_t* BI63_div(BI63_t *m0, BI63_t *n0, BI63_t *q) {

    BI63_t m, n, w1, w2, r;
    ASSIGN(&m, m0);
    ASSIGN(&n, n0);

    if (!BI63_isNegative(&m) && !BI63_isNegative(&n)) {
        BI63_divmod(&m, &n, q, &r);
    }
    else if (!BI63_isNegative(&m) && BI63_isNegative(&n)) {
        BI63_divmod(&m, BI63_negate(&n, &w1), q, &r);
        BI63_negate(q, q);
    }
    else if (BI63_isNegative(&m) && !BI63_isNegative(&n)) {
        BI63_divmod(BI63_negate(&m, &w1), &n, q, &r);
        BI63_negate(q, q);
    }
    else
        BI63_divmod(BI63_negate(&m, &w1), BI63_negate(&n, &w2), q, &r);

    return q;
}


BI63_t* BI63_mul(BI63_t *a, BI63_t *b, BI63_t *c) {
    BI63_t r;
    BI63_fromInt(&r, 0);
    for (int i = 0; i < BI63_SIZE; ++i) {
        uint64_t bb = b->__[i];
        if (bb==0)
            continue;
        uint64_t b_lo = (uint64_t)(uint32_t)bb;
        uint64_t b_hi = bb >> 32u;
        for (int j = 0; j < BI63_SIZE-i; ++j) {
            uint64_t aa = (uint64_t )a->__[j];
            if (aa==0)
                continue;

            uint64_t a_lo = (uint64_t)(uint32_t)aa;
            uint64_t a_hi = aa >> 32u;

            uint64_t p0 = a_lo * b_lo;
            uint64_t p1 = a_lo * b_hi;
            uint64_t p2 = a_hi * b_lo;
            uint64_t p3 = a_hi * b_hi;

            uint32_t cy = (uint32_t)(((p0 >> 32u) + (uint32_t)p1 + (uint32_t)p2) >> 32u);

            uint64_t lo = p0 + (p1 << 32u) + (p2 << 32u);
            uint64_t hi = p3 + (p1 >> 32u) + (p2 >> 32u) + cy;

            BI63_add_int(&r, lo & 0x7fffffffffffffffu, i + j);
            if (i+j+1<BI63_SIZE)
                BI63_add_int(&r, (hi << 1u) + (lo >> 63u), i + j + 1);
        }
    }

    ASSIGN(c, &r);
    return c;
}

BI63_t* BI63_negate(BI63_t *in, BI63_t *out) {
    for (int i=0; i<BI63_SIZE; i++)
        out->__[i] = (~in->__[i])&0x7fffffffffffffffu;
    BI63_add_int(out, 1, 0);
    return out;
}

uint64_t BI63_isNegative(BI63_t *a) {
    return a->__[BI63_SIZE-1]&0x4000000000000000ul;
}

uint64_t BI63_isPositive(BI63_t *a) {
    return (~a->__[BI63_SIZE-1])&0x4000000000000000ul;
}

void BI63_printRawHex(BI63_t* a) {
    printf("0x");
    for (int i = BI63_SIZE-1; i>=0; i--) {
        assert(0==(a->__[i]&0x8000000000000000));
        printf("%016llX", a->__[i]);
    }
    printf("\n");
}

void BI63_toString(BI63_t *n0, char *s) {
    BI63_t n, ten;
    ASSIGN(&n, n0);
    BI63_fromInt(&ten, 10);
    char buf[256], *ptr = &buf[255];

    *ptr = '\0';

    if (BI63_isZero(&n)) {
        strcpy(s, "0");
        return;
    }

    if (BI63_isNegative(n0))
        BI63_negate(&n, &n);

    while (!BI63_isZero(&n)) {
        BI63_t q, r;
        BI63_divmod(&n, &ten, &q, &r);
        *--ptr = '0'+r.__[0];
        ASSIGN(&n, &q);
    }
    if (BI63_isNegative(n0))
        *--ptr = '-';

    strcpy(s, ptr);
}

void BI63_printDecimal(BI63_t *n0) {
    char buf[256];
    BI63_toString(n0, buf);
    printf("%s\n", buf);
}

void BI63_printHex(BI63_t *a) {
    printf("0x");
    uint64_t carry = 0;
    int carryBits = 0;
    for (int i = BI63_SIZE-1; i>=0; i--) {
        uint64_t a0 = a->__[i];

        switch (carryBits) {
            case 0:
                //63 + 0 = 63 bits
                printf("%015llX", (a0&0x7fffffffffffffffu)>>3u);
                carry = a0<<61u;
                carryBits = 3;
                break;
            case 3:
                //63 + 3 = 66 bits
                printf("%016llX", carry|((a0&0x7fffffffffffffffu)>>2u));
                carry = a0<<62u;
                carryBits = 2;
                break;
            case 2:
                //63 + 2 = 65 bits
                printf("%016llX", carry|((a0&0x7fffffffffffffffu)>>1u));
                carry = a0<<63u;
                carryBits = 1;
                break;
            case 1:
                //63 + 1 = 64 bits
                printf("%016llX", carry|((a0&0x7fffffffffffffffu)>>0u));
                carry = a0<<0u;
                carryBits = 0;
                break;

        }
        //printf("[%d]", carryBits);
    }
    printf("\n");
}

BI63_t* BI63_fromString(BI63_t *n, char *s) {
    BI63_fromInt(n, 0);
    BI63_t digit, ten, result;
    BI63_fromInt(&digit, 0);
    BI63_fromInt(&ten, 10);
    BI63_fromInt(&result, 0);
    int negative = 0;

    if (*s=='-') {
        s++;
        negative = 1;
    }

    while (*s!='\0') {
        BI63_t temp;
        digit.__[0] = *s++ -'0';
        //PRINT(&digit);
        BI63_mul(&result, &ten, &temp);
        BI63_add(&temp, &digit, &result);
    }
    ASSIGN(n, &result);
    if (negative)
        BI63_negate(n, n);
    return n;
}

static long _seed = 123456789;
static long _m = 0x100000000L;
static long _a = 1103515245;
static long _c = 12345;

long v_rand()
{
    _seed = (_a * _seed + _c) % _m;
    //print(_seed); print ("\n");
    return _seed;
}

void BI63_random_digits(int digits, BI63_t *out) {
    char n[256];
    for (int i=0; i<digits; i++)
        n[i] = '0'+(int)(v_rand()%10);
    n[digits] = '\0';
    BI63_fromString(out, n);
    //printf("rnd:"); PRINTD(out);
}



static void BI63_random_range(BI63_t *n, BI63_t *out) {
    char buf[256];
    BI63_t m;
    //printf("in range:");
    //PRINTD(n);
    BI63_toString(n, buf);
    int digits = strlen(buf);
    BI63_random_digits(digits, &m);
    BI63_mod0(&m, n, out);
}

#define PRIMEMAX (1000)

static int isComposite[PRIMEMAX+1];
static int primes[PRIMEMAX];
static int primeCount;

// Sieve of Eratosthenes
static void BI63_PRIMES() {
    primeCount = 0;
    memset(isComposite, 0, sizeof(*isComposite));
    for (int primeCandidate = 2; primeCandidate <= PRIMEMAX; primeCandidate++) {
        if (isComposite[primeCandidate])
            continue;

        primes[primeCount++] = primeCandidate;
        //print(primeCandidate); print("\n");
        for (int composite = 2*primeCandidate; composite <= PRIMEMAX; composite += primeCandidate)
            isComposite[composite] = 1;
    }
}

// Finds modular inverse
// Returns inverse, unused helper and gcd
void BI63_modular_inv(BI63_t *a, BI63_t *b, BI63_t *out) {

    if (BI63_isZero(b)) {
        BI63_fromInt(&out[0], 1);
        BI63_fromInt(&out[1], 0);
        ASSIGN(&out[2], a);
        return;
    }

    BI63_t q,r;

    BI63_divmod(a, b, &q, &r);
    // String q=t[0], r=t[1];
    const int X=0, Y=1, G=2;
    // String[] xyg = modular_inv(b, r, t);
    BI63_t xyg[3];
    BI63_modular_inv(b, &r, xyg);
    ASSIGN(&out[0], &xyg[Y]);
    BI63_sub(&xyg[X], BI63_mul(&q, &xyg[Y], &out[1]), &out[1]);
    ASSIGN(&out[2], &xyg[G]);
}

// Addition in Elliptic curve modulo m space
void BI63_elliptic_add(BI63_t *p, BI63_t *q, BI63_t *a, BI63_t *b, BI63_t *m, BI63_t *out) {

    // If one point is infinity, return other one
    if (BI63_isZero(&p[2])) {
        memcpy(out, q, 3*sizeof(BI63_t));
        return;
    }

    if (BI63_isZero(&q[2])) {
        memcpy(out, p, 3*sizeof(BI63_t));
        return;
    }

    BI63_t num, denom, w1, w2, w3, w4;

    if (BI63_compare(&p[0], &q[0]) == 0 ) {
        if (BI63_isZero(BI63_mod(BI63_add(&p[1], &q[1], &w1), m, &w2)) ) {
            BI63_fromInt(&out[0], 0);
            BI63_fromInt(&out[1], 1);
            BI63_fromInt(&out[2], 0);
            return;
        }
        BI63_mod(BI63_add(BI63_mul(BI63_mul(&p[0], &p[0], &w1), BI63_fromInt(&w4, 3), &w2), a, &w3), m, &num);
        BI63_mod(BI63_mul(&p[1], BI63_fromInt(&w2, 2), &w1), m, &denom);
    }
    else {
        BI63_mod(BI63_sub(&q[1], &p[1], &w1), m, &num);
        BI63_mod(BI63_sub(&q[0], &p[0], &w1), m, &denom);
    }

    BI63_t inv_g[3];

    BI63_modular_inv(&denom, m, inv_g);
    const int INV = 0;
    const int G = 2;
    //inv, _, g = modular_inv(denom, m)
    // Unable to find inverse, arithmetic breaks
    if (BI63_compare(&inv_g[G], BI63_fromInt(&w1, 1)) > 0 ) {
        BI63_fromInt(&out[0], 0);
        BI63_fromInt(&out[1], 0);
        ASSIGN(&out[2], &denom); // FAILURE
        return;
    }

    BI63_t num_times_inv;
    BI63_mul(&num, &inv_g[INV], &num_times_inv);
    BI63_t z;

    BI63_mod(BI63_sub(BI63_mul(&num_times_inv, &num_times_inv, &w1), BI63_add(&p[0], &q[0], &w2), &w3), m, &z);

    ASSIGN( &out[0], &z );
    BI63_mod(BI63_sub(BI63_mul(&num_times_inv, BI63_sub(&p[0], &z, &w1), &w2), &p[1], &w3), m, &out[1]);
    BI63_fromInt(&out[2], 1);
}


// Multiplication (repeated addition and doubling)
void BI63_elliptic_mul(BI63_t *k0, BI63_t *p0, BI63_t *a, BI63_t *b, BI63_t *m, BI63_t *out)
{
    BI63_t r[3], p[3], w1, w2, w3, k, w4[3];

    ASSIGN(&k, k0);
    memcpy(p, p0, 3*sizeof(BI63_t));
    BI63_fromInt(&r[0], 0);
    BI63_fromInt(&r[1], 1);
    BI63_fromInt(&r[2], 0);

    while ( !BI63_isZero(&k) ){
        // p is failure, return it
        if (BI63_compare(&p[2], BI63_fromInt(&w1, 1)) > 0 ) {
            memcpy(out, p, 3*sizeof(BI63_t));
            return;
        }

        if (BI63_compare(BI63_mod(&k, BI63_fromInt(&w1, 2), &w3), BI63_fromInt(&w2, 1))==0) {
            BI63_elliptic_add(p, r, a, b, m, w4);
            memcpy(r, w4, 3*sizeof(BI63_t));
        }
        BI63_rshift_bit(&k);
        BI63_elliptic_add(p, p, a, b, m, w4);
        memcpy(p, w4, 3*sizeof(BI63_t));
    }

    memcpy(out, r, 3*sizeof(BI63_t));
}

void BI63_gcd(BI63_t *a0, BI63_t *b0, BI63_t *gcd) {
    BI63_t a, b, r;
    ASSIGN(&a, a0);
    ASSIGN(&b, b0);
    while (!BI63_isZero(&b)) {
        BI63_mod(&a, &b, &r);
        ASSIGN(&a, &b);
        ASSIGN(&b, &r);
    }
    ASSIGN(gcd, &a);
}

// Lenstra's algorithm for factoring
// Limit specifies the amount of work permitted
void BI63_lenstra(BI63_t *n0, int limit, BI63_t *out) {

    BI63_t n, g, a, b, w1, w2, w3, w4, w5, w6, w7;
    ASSIGN(&n, n0);
    ASSIGN(&g, n0);
    BI63_fromInt(&a, 0);
    BI63_fromInt(&b, 0);
    BI63_t q[3];
    BI63_fromInt(&q[0], 0);
    BI63_fromInt(&q[1], 1);
    BI63_fromInt(&q[2], 0);

    //String[] q = new String[]{"0", "1", "0"}; // infinity? doesnt matter gets overwritten

    while (BI63_compare(&g, &n) == 0 ) {

        // Randomized x and y
        BI63_random_range(&n, &q[0]);
        BI63_random_range(&n, &q[1]);
        BI63_fromInt(&q[2], 1);
        // Randomized curve coefficient a, computed b
        BI63_random_range(&n, &a);
        BI63_mod(BI63_sub(BI63_mul(&q[1], &q[1], &w1),
                          BI63_add(BI63_mul(&q[0], BI63_mul(&q[0], &q[0], &w2), &w4), BI63_mul(&a, &q[0], &w3), &w5), &w6),
                 &n, &b);
        BI63_gcd(BI63_add(BI63_mul(BI63_mul(&a, BI63_fromInt(&w4, 4),&w1), BI63_mul(&a, &a, &w2), &w3),
                          BI63_mul(BI63_mul(&b, &b, &w4), BI63_fromInt(&w7, 27), &w5), &w6), &n, &g);  // singularity check
    }

    // If we got lucky, return lucky factor
    if (BI63_compare(&g, BI63_fromInt(&w1, 1)) > 0 ) {
        ASSIGN(out, &g);
        return;
    }

    // increase k step by step until lcm(1, ..., limit)
    for (int i = 0; i < primeCount; i++) {
        BI63_t p, pp;
        BI63_fromInt(&p, primes[i]);
        ASSIGN( &pp, &p );

        while (BI63_compare(&pp, BI63_fromInt(&w1, limit)) < 0 ) {
            BI63_elliptic_mul(&p, q, &a, &b, &n, q);
            if (BI63_compare(&q[2], BI63_fromInt(&w1, 1)) > 0 ) {
                BI63_gcd(&q[2], &n, out);
                return;
            }
            BI63_mul(&p, &pp, &pp);
        }
    }
    BI63_fromInt(out, 0);
}

void BI63_lenstra_test(BI63_t *n0, int limit) {

    BI63_PRIMES();

    BI63_t p, n;
    ASSIGN(&n, n0);
    BI63_fromInt(&p, 1);
    //printf("p:");PRINTD(&p);
    for (int i=0;i < 5; i++) {
        BI63_t m;
        printf("n:");
        BI63_printDecimal(&n);
        char buf[256];
        BI63_toString(&n, buf);
        //if (strcmp(buf, "1009012267184813")==0)
        //    printf("%ld\n", _seed);
        BI63_lenstra(&n, limit, &m);
        //break;
        if (BI63_isZero(&m))
            continue;
        //printf("1n:");PRINTD(n);
        //printf("1m:");PRINTD(&m);
        //printf("1p:");PRINTD(&p);
        BI63_t tmp;
        BI63_div(&n, &m, &tmp);
        ASSIGN(&n, &tmp);
        BI63_mul(&p, &m, &p);
        //printf("2n:");PRINTD(n);
        BI63_printDecimal(&m);
        //break;
        //printf("2p:");PRINTD(&p);
    }

    //printf("%d\n", ran_count);
}
