//
// Created by Alexis Farmer on 2019-04-11.
//

#include "varint.h"

#include <stdio.h>
#include <string.h>
#include <assert.h>

V* V_fromInt(V* a, uint64_t n) {
    a->__[0] = n & 0x7ffffffffffffffful;
    if ( n & 0x8000000000000000ul ) {
        for (int i = 1; i<VARINT_DWORDS; i++)
            a->__[i] = 0x7fffffffffffffff;
    }
    else {
        for (int i = 1; i<VARINT_DWORDS; i++)
            a->__[i] = 0;
    }
    return a;
}

V* V_add(V* a, V* b, V*c) {
    uint64_t carry = 0;
    int i;
    for (i=0; i<VARINT_DWORDS; i++) {
        carry += a->__[i] + b->__[i];
        c->__[i] = carry & 0x7fffffffffffffffu;
        carry >>= 63u;
    }
    return c;
}

V* V_sub(V* a, V* b, V*c) {
    uint64_t carry = 1;
    for (int i=0; i<VARINT_DWORDS; i++) {
        carry += a->__[i] + (~b->__[i] & 0x7fffffffffffffffu);
        c->__[i] = carry & 0x7fffffffffffffffu;
        carry >>= 63u;
    }
    return c;
}

int V_isZero(V* a) {
    for (int i=0; i<VARINT_DWORDS; i++)
        if (a->__[i]!=0)
            return 0;
    return 1;
}

int V_uCompare(V* a, V* b) {
    for (int i = VARINT_DWORDS-1; i>=0; i--) {
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

void V_lshift_bit(V *a) {
    uint64_t carry=0;
    for (int i=0; i<VARINT_DWORDS; i++) {
        uint64_t result = carry | (a->__[i]<<1u);
        carry = (a->__[i]>>62u)&1u;
        a->__[i] = result&0x7fffffffffffffffu;
    }
}

void V_rshift_bit(V *a) {
    uint64_t carry=0;
    for (int i=VARINT_DWORDS-1; i>=0; i--) {
        uint64_t result = carry | (a->__[i]>>1u);
        carry = (a->__[i]<<62u)&0x4000000000000000u;
        a->__[i] = result&0x7fffffffffffffffu;
    }
}

void V_add_int(V *a, uint64_t carry, int shift_left) {
    for (int i=shift_left; i<VARINT_DWORDS; i++) {
        carry += a->__[i];
        a->__[i] = carry & 0x7fffffffffffffffu;
        carry >>= 63u;
        if (carry==0)
            break;
    }
}

void V_divmod(V* m0, V* n0, V* q, V* r) {
    V n;

    uint64_t s0 = 0;

    memset(q, 0, sizeof(V));
    ASSIGN(r, m0);
    ASSIGN(&n, n0);

    while (V_uCompare( r, &n ) > 0) {
        s0++;
        V_lshift_bit(&n);
    }

    for (;;) {
        if (V_uCompare( &n, r ) <= 0) {
            V_sub(r, &n, r);
            //V_add(q, &shift, q);
            int j = s0/63;
            int k = s0-j*63;
            uint64_t kk = 1ul<<(uint64_t )(k);
            V_add_int(q, kk, j);
        }
        if (s0==0)
            break;
        s0--;
        V_rshift_bit(&n);
    }
}

V* V_MOD0(V* m, V *n, V *r) {
    V q;
    V_divmod(m, n, &q, r);
    return r;
}

V* V_MOD(V* m, V *n, V *r) {

    V w1, w2, w3;

    if (!V_isNegative(m) && !V_isNegative(n))
        return V_MOD0(m, n, r);

    if (!V_isNegative(m) && V_isNegative(n))
        return V_add(n, V_MOD0(m, V_negate(n, &w2), &w1), r);

    if (V_isNegative(m) && !V_isNegative(n))
        return V_sub(n,V_MOD0(V_negate(m, &w2), n, &w1), r);

    // both negative
    return V_negate( V_MOD0( V_negate(m, &w1), V_negate(n, &w2), &w3), r);
}

V* V_DIV(V *m0, V*n0, V* q) {

    V m, n, w1, w2, r;
    ASSIGN(&m, m0);
    ASSIGN(&n, n0);

    if (!V_isNegative(&m) && !V_isNegative(&n)) {
        V_divmod(&m, &n, q, &r);
    }
    else if (!V_isNegative(&m) && V_isNegative(&n)) {
        V_divmod(&m, V_negate(&n, &w1), q, &r);
        V_NEGATE_INPLACE(q);
    }
    else if (V_isNegative(&m) && !V_isNegative(&n)) {
        V_divmod(V_negate(&m, &w1), &n, q, &r);
        V_NEGATE_INPLACE(q);
    }
    else
        V_divmod(V_negate(&m, &w1), V_negate(&n, &w2), q, &r);

    return q;
}


V* V_MUL(V *a, V *b, V *c) {
    V r;
    V_fromInt(&r, 0);
    for (int i = 0; i < VARINT_DWORDS; ++i) {
        uint64_t bb = b->__[i];
        if (bb==0)
            continue;
        uint64_t b_lo = (uint64_t)(uint32_t)bb;
        uint64_t b_hi = bb >> 32u;
        for (int j = 0; j < VARINT_DWORDS-i; ++j) {
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

            V_add_int(&r, lo&0x7fffffffffffffffu, i+j);
            if (i+j+1<VARINT_DWORDS)
                V_add_int(&r, (hi<<1u)+(lo>>63u), i+j+1);
        }
    }

    ASSIGN(c, &r);
    return c;
}

void V_NEGATE_INPLACE(V *n) {
    for (int i=0; i<VARINT_DWORDS; i++)
        n->__[i] = (~n->__[i])&0x7fffffffffffffffu;
    V_add_int(n, 1, 0);
}

V* V_negate(V *n, V* out) {
    for (int i=0; i<VARINT_DWORDS; i++)
        out->__[i] = (~n->__[i])&0x7fffffffffffffffu;
    V_add_int(out, 1, 0);
    return out;
}

int V_isNegative(V *n) {
    return (n->__[VARINT_DWORDS-1]>>62u)&1u;
}

void V_printRawHex(V* a) {
    printf("0x");
    for (int i = VARINT_DWORDS-1; i>=0; i--) {
        assert(0==(a->__[i]&0x8000000000000000));
        printf("%016llX", a->__[i]);
    }
    printf("\n");
}

void V_toString(V *n0, char *s) {
    V n, ten;
    ASSIGN(&n, n0);
    V_fromInt(&ten, 10);
    char buf[256], *ptr = &buf[255];

    *ptr = '\0';

    if (V_isZero(&n)) {
        strcpy(s, "0");
        return;
    }

    if (V_isNegative(n0))
        V_NEGATE_INPLACE(&n);

    while (!V_isZero(&n)) {
        V q, r;
        V_divmod(&n, &ten, &q, &r);
        *--ptr = '0'+r.__[0];
        ASSIGN(&n, &q);
    }
    if (V_isNegative(n0))
        *--ptr = '-';

    strcpy(s, ptr);
}

void V_PRINTD(V *n0) {
    char buf[256];
    V_toString(n0, buf);
    printf("%s\n", buf);
}

void V_printHex(V* a) {
    printf("0x");
    uint64_t carry = 0;
    int carryBits = 0;
    for (int i = VARINT_DWORDS-1; i>=0; i--) {
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

V* V_FROMSTRING(V *n, char *s) {
    V_fromInt(n, 0);
    V digit, ten, result;
    V_fromInt(&digit, 0);
    V_fromInt(&ten, 10);
    V_fromInt(&result, 0);
    int negative = 0;

    if (*s=='-') {
        s++;
        negative = 1;
    }

    while (*s!='\0') {
        V temp;
        digit.__[0] = *s++ -'0';
        //PRINT(&digit);
        V_MUL(&result, &ten, &temp);
        V_add(&temp, &digit, &result);
    }
    ASSIGN(n, &result);
    if (negative)
        V_NEGATE_INPLACE(n);
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

void v_random_digits(int digits, V* out) {
    char n[256];
    for (int i=0; i<digits; i++)
        n[i] = '0'+(int)(v_rand()%10);
    n[digits] = '\0';
    V_FROMSTRING(out, n);
    //printf("rnd:"); PRINTD(out);
}



void v_random_range(V* n, V* out) {
    char buf[256];
    V m;
    //printf("in range:");
    //PRINTD(n);
    V_toString(n, buf);
    int digits = strlen(buf);
    v_random_digits(digits, &m);
    V_MOD0(&m, n, out);
}

#define PRIMEMAX (1000)

int isComposite[PRIMEMAX+1];
int primes[PRIMEMAX];
int primeCount;

// Sieve of Eratosthenes
void V_PRIMES() {
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
void V_MODULAR_INV(V* a, V* b, V out[3]) {

    if (V_isZero(b)) {
        V_fromInt(&out[0], 1);
        V_fromInt(&out[1], 0);
        ASSIGN(&out[2], a);
        return;
    }

    V q,r;

    V_divmod( a, b, &q, &r);
    // String q=t[0], r=t[1];
    const int X=0, Y=1, G=2;
    // String[] xyg = modular_inv(b, r, t);
    V xyg[3];
    V_MODULAR_INV(b, &r, xyg);
    ASSIGN(&out[0], &xyg[Y]);
    V_sub(&xyg[X], V_MUL(&q, &xyg[Y], &out[1]), &out[1]);
    ASSIGN(&out[2], &xyg[G]);
}

static V* V_MULx2(V* a, V* out) {
    V_MUL(a, V_fromInt(out, 2), out);
    return out;
}

static V* V_MULx3(V* a, V* out) {
    V_MUL(a, V_fromInt(out, 3), out);
    return out;
}

static V* V_MULx4(V* a, V* out) {
    V_MUL(a, V_fromInt(out, 4), out);
    return out;
}

static V* V_MULx27(V* a, V* out) {
    V_MUL(a, V_fromInt(out, 27), out);
    return out;
}

int V_COMPARE(V* a, V* b) {
    V c;
    V_sub(a, b, &c);
    return V_isNegative(&c) ? -1 : (V_isZero(&c)?0:1);
}

// Addition in Elliptic curve modulo m space
void V_ELLIPTIC_ADD(V p[3], V q[3], V* a, V* b, V* m, V out[3]) {

    // If one point is infinity, return other one
    if (V_isZero(&p[2])) {
        memcpy(out, q, 3*sizeof(V));
        return;
    }

    if (V_isZero(&q[2])) {
        memcpy(out, p, 3*sizeof(V));
        return;
    }

    V num, denom, w1, w2, w3;

    if ( V_uCompare( &p[0], &q[0] ) == 0 ) {
        if ( V_isZero( V_MOD( V_add(&p[1], &q[1], &w1 ), m, &w2 ) ) ) {
            V_fromInt( &out[0], 0 );
            V_fromInt( &out[1], 1 );
            V_fromInt( &out[2], 0 );
            return;
        }
        V_MOD( V_add( V_MULx3(V_MUL(&p[0], &p[0], &w1), &w2 ), a, &w3 ), m, &num );
        V_MOD( V_MULx2( &p[1], &w1 ), m, &denom );
    }
    else {
        V_MOD(V_sub(&q[1], &p[1], &w1), m, &num );
        V_MOD(V_sub(&q[0], &p[0], &w1), m, &denom );
    }

    V inv_g[3];

    V_MODULAR_INV(&denom, m, inv_g);
    const int INV = 0;
    const int G = 2;
    //inv, _, g = modular_inv(denom, m)
    // Unable to find inverse, arithmetic breaks
    if ( V_COMPARE( &inv_g[G], V_fromInt( &w1, 1 ) ) > 0 ) {
        V_fromInt(&out[0], 0);
        V_fromInt(&out[1], 0);
        ASSIGN(&out[2], &denom); // FAILURE
        return;
    }

    V num_times_inv;
    V_MUL(&num, &inv_g[INV], &num_times_inv);
    V z;

    V_MOD(V_sub(V_MUL(&num_times_inv, &num_times_inv, &w1), V_add(&p[0], &q[0], &w2), &w3), m, &z );

    ASSIGN( &out[0], &z );
    V_MOD(V_sub(V_MUL(&num_times_inv, V_sub(&p[0], &z, &w1), &w2), &p[1], &w3), m, &out[1]);
    V_fromInt(&out[2], 1);
}


// Multiplication (repeated addition and doubling)
void V_ELLIPTIC_MUL(V* k0, V p0[3], V* a, V* b, V* m, V out[3])
{
    V r[3], p[3], w1, w2, w3, k, w4[3];

    ASSIGN(&k, k0);
    memcpy(p, p0, 3*sizeof(V));
    V_fromInt(&r[0], 0);
    V_fromInt(&r[1], 1);
    V_fromInt(&r[2], 0);

    while ( !V_isZero(&k ) ){
        // p is failure, return it
        if ( V_COMPARE(&p[2],V_fromInt(&w1, 1)) > 0 ) {
            memcpy(out, p, 3*sizeof(V));
            return;
        }

        if ( V_uCompare( V_MOD( &k, V_fromInt( &w1, 2 ), &w3), V_fromInt(&w2, 1))==0) {
            V_ELLIPTIC_ADD(p, r, a, b, m, w4);
            memcpy(r, w4, 3*sizeof(V));
        }
        V_rshift_bit(&k);
        V_ELLIPTIC_ADD(p, p, a, b, m, w4);
        memcpy(p, w4, 3*sizeof(V));
    }

    memcpy(out, r, 3*sizeof(V));
}

void V_GCD(V* a0, V* b0, V* gcd) {
    V a, b, r;
    ASSIGN(&a, a0);
    ASSIGN(&b, b0);
    while (!V_isZero(&b)) {
        V_MOD( &a, &b, &r );
        ASSIGN(&a, &b);
        ASSIGN(&b, &r);
    }
    ASSIGN(gcd, &a);
}

// Lenstra's algorithm for factoring
// Limit specifies the amount of work permitted
void V_LENSTRA(V* n0, int limit, V* out) {

    V n, g, a, b, w1, w2, w3, w4, w5, w6;
    ASSIGN(&n, n0);
    ASSIGN(&g, n0);
    V_fromInt( &a, 0 );
    V_fromInt( &b, 0 );
    V q[3];
    V_fromInt( &q[0], 0 );
    V_fromInt( &q[1], 1 );
    V_fromInt( &q[2], 0 );

    //String[] q = new String[]{"0", "1", "0"}; // infinity? doesnt matter gets overwritten

    while ( V_uCompare( &g, &n ) == 0 ) {

        // Randomized x and y
        v_random_range(&n, &q[0]);
        v_random_range(&n, &q[1]);
        V_fromInt(&q[2], 1);
        // Randomized curve coefficient a, computed b
        v_random_range(&n, &a);
        V_MOD( V_sub( V_MUL( &q[1], &q[1], &w1), V_add( V_MUL( &q[0], V_MUL( &q[0], &q[0], &w2 ), &w4 ), V_MUL( &a, &q[0], &w3 ), &w5), &w6), &n, &b);
        V_GCD( V_add( V_MUL( V_MULx4( &a, &w1 ), V_MUL( &a, &a, &w2 ), &w3 ), V_MULx27( V_MUL( &b, &b, &w4 ), &w5 ), &w6 ), &n, &g);  // singularity check
    }

    // If we got lucky, return lucky factor
    if ( V_COMPARE( &g, V_fromInt( &w1, 1 ) ) > 0 ) {
        ASSIGN(out, &g);
        return;
    }

    // increase k step by step until lcm(1, ..., limit)
    for (int i = 0; i < primeCount; i++) {
        V p, pp;
        V_fromInt( &p, primes[i] );
        ASSIGN( &pp, &p );

        while (V_COMPARE( &pp, V_fromInt( &w1, limit ) ) < 0 ) {
            V_ELLIPTIC_MUL(&p, q, &a, &b, &n, q);
            if ( V_COMPARE( &q[2], V_fromInt( &w1, 1 ) ) > 0 ) {
                V_GCD( &q[2], &n, out );
                return;
            }
            V_MUL( &p, &pp, &pp);
        }
    }
    V_fromInt(out, 0);
}

void V_LENSTRA_TEST(V *n0, int limit) {

    V_PRIMES();

    V p, n;
    ASSIGN(&n, n0);
    V_fromInt(&p, 1);
    //printf("p:");PRINTD(&p);
    for (int i=0;i < 5; i++) {
        V m;
        printf("n:"); V_PRINTD(&n);
        char buf[256];
        V_toString(&n, buf);
        //if (strcmp(buf, "1009012267184813")==0)
        //    printf("%ld\n", _seed);
        V_LENSTRA(&n, limit, &m);
        //break;
        if (V_isZero(&m))
            continue;
        //printf("1n:");PRINTD(n);
        //printf("1m:");PRINTD(&m);
        //printf("1p:");PRINTD(&p);
        V tmp;
        V_DIV(&n, &m, &tmp);
        ASSIGN(&n, &tmp);
        V_MUL(&p, &m, &p);
        //printf("2n:");PRINTD(n);
        V_PRINTD(&m);
        //break;
        //printf("2p:");PRINTD(&p);
    }

    //printf("%d\n", ran_count);
}
