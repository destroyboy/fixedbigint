//
// Created by Alexis Farmer on 2019-04-08.
//
#include <string.h>

#include "lenstra.h"
#include "bi32.h"

#include <stdio.h>
#include <assert.h>


static long _seed = 123456789;
static long _m = 0x100000000L;
static long _a = 1103515245;
static long _c = 12345;

long _rand()
{
    _seed = (_a * _seed + _c) % _m;
    //print(_seed); print ("\n");
    return _seed;
}

void k_random_digits(int digits, BI32_t* out) {
    char n[256];
    for (int i=0; i<digits; i++)
        n[i] = '0'+(int)(_rand()%10);
    n[digits] = '\0';
    FROMSTRING(out, n);
    //printf("rnd:"); PRINTD(out);
}



void k_random_range(BI32_t* n, BI32_t* out) {
    char buf[256];
    BI32_t m;
    //printf("in range:");
    //PRINTD(n);
    BI32_toString(n, buf);
    int digits = strlen(buf);
    k_random_digits(digits, &m);
    MOD0(&m, n, out);
}

int isComposite[PRIMEMAX+1];
int primes[PRIMEMAX];
int primeCount;

// Sieve of Eratosthenes
void PRIMES() {
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
void MODULAR_INV(BI32_t* a, BI32_t* b, BI32_t out[3]) {

    if (ISZERO(b)) {
        FROMINT(&out[0], 1);
        FROMINT(&out[1], 0);
        ASSIGN(&out[2], a);
        return;
    }

    BI32_t q,r;

    DIVMOD0( a, b, &q, &r);
    // String q=t[0], r=t[1];
    const int X=0, Y=1, G=2;
    // String[] xyg = modular_inv(b, r, t);
    BI32_t xyg[3];
    MODULAR_INV(b, &r, xyg);
    ASSIGN(&out[0], &xyg[Y]);
    SUB(&xyg[X], MUL(&q, &xyg[Y], &out[1]), &out[1]);
    ASSIGN(&out[2], &xyg[G]);
}

// Addition in Elliptic curve modulo m space
void ELLIPTIC_ADD(BI32_t p[3], BI32_t q[3], BI32_t* a, BI32_t* b, BI32_t* m, BI32_t out[3]) {

    // If one point is infinity, return other one
    if (ISZERO(&p[2])) {
        memcpy(out, q, 3*sizeof(BI32_t));
        return;
    }

    if (ISZERO(&q[2])) {
        memcpy(out, p, 3*sizeof(BI32_t));
        return;
    }

    BI32_t num, denom, w1, w2, w3, w4;

    if ( COMPARE_UNSIGNED( &p[0], &q[0] ) == 0 ) {
        if ( ISZERO( MOD( ADD(&p[1], &q[1], &w1 ), m, &w2 ) ) ) {
            FROMINT( &out[0], 0 );
            FROMINT( &out[1], 1 );
            FROMINT( &out[2], 0 );
            return;
        }
        MOD( ADD( MUL(MUL(&p[0], &p[0], &w1), FROMINT(&w4, 3),&w2 ), a, &w3 ), m, &num );
        MOD( MUL( &p[1], FROMINT(&w2,2), &w1 ), m, &denom );
    }
    else {
        MOD(SUB(&q[1], &p[1], &w1), m, &num );
        MOD(SUB(&q[0], &p[0], &w1), m, &denom );
    }

    BI32_t inv_g[3];

    MODULAR_INV(&denom, m, inv_g);
    const int INV = 0;
    const int G = 2;
    //inv, _, g = modular_inv(denom, m)
    // Unable to find inverse, arithmetic breaks
    if ( COMPARE( &inv_g[G], FROMINT( &w1, 1 ) ) > 0 ) {
        FROMINT(&out[0], 0);
        FROMINT(&out[1], 0);
        ASSIGN(&out[2], &denom); // FAILURE
        return;
    }

    BI32_t num_times_inv;
    MUL(&num, &inv_g[INV], &num_times_inv);
    BI32_t z;

    MOD(SUB(MUL(&num_times_inv, &num_times_inv, &w1), ADD(&p[0], &q[0], &w2), &w3), m, &z );

    ASSIGN( &out[0], &z );
    MOD(SUB(MUL(&num_times_inv, SUB(&p[0], &z, &w1), &w2), &p[1], &w3), m, &out[1]);
    FROMINT(&out[2], 1);
}

// Multiplication (repeated addition and doubling)
void ELLIPTIC_MUL(BI32_t* k0, BI32_t p0[3], BI32_t* a, BI32_t* b, BI32_t* m, BI32_t out[3])
{
    BI32_t r[3], p[3], w1, w2, w3, k, w4[3];

    ASSIGN(&k, k0);
    memcpy(p, p0, 3*sizeof(BI32_t));
    FROMINT(&r[0], 0);
    FROMINT(&r[1], 1);
    FROMINT(&r[2], 0);

    while ( !ISZERO( &k ) ){
        // p is failure, return it
        if ( COMPARE(&p[2],FROMINT(&w1, 1)) > 0 ) {
            memcpy(out, p, 3*sizeof(BI32_t));
            return;
        }

        if ( COMPARE_UNSIGNED( MOD( &k, FROMINT( &w1, 2 ), &w3), FROMINT(&w2, 1))==0) {
            ELLIPTIC_ADD(p, r, a, b, m, w4);
            memcpy(r, w4, 3*sizeof(BI32_t));
        }
        BI32_rshift_bit(&k);
        ELLIPTIC_ADD(p, p, a, b, m, w4);
        memcpy(p, w4, 3*sizeof(BI32_t));
    }

    memcpy(out, r, 3*sizeof(BI32_t));
}

// Lenstra's algorithm for factoring
// Limit specifies the amount of work permitted
void LENSTRA(BI32_t* n0, int limit, BI32_t* out) {

    BI32_t n, g, a, b, w1, w2, w3, w4, w5, w6, w7,w8;
    ASSIGN(&n, n0);
    ASSIGN(&g, n0);
    FROMINT( &a, 0 );
    FROMINT( &b, 0 );
    BI32_t q[3];
    FROMINT( &q[0], 0 );
    FROMINT( &q[1], 1 );
    FROMINT( &q[2], 0 );

    //String[] q = new String[]{"0", "1", "0"}; // infinity? doesnt matter gets overwritten

    while ( COMPARE_UNSIGNED( &g, &n ) == 0 ) {

        // Randomized x and y
        k_random_range(&n, &q[0]);
        k_random_range(&n, &q[1]);
        FROMINT(&q[2], 1);
        // Randomized curve coefficient a, computed b
        k_random_range(&n, &a);
        MOD( SUB( MUL( &q[1], &q[1], &w1), ADD( MUL( &q[0], MUL( &q[0], &q[0], &w2 ), &w4 ), MUL( &a, &q[0], &w3 ), &w5), &w6), &n, &b);
        BI32_gcd(ADD(MUL(MUL(&a, FROMINT(&w7, 4),&w1), MUL(&a, &a, &w2), &w3), MUL(MUL(&b, &b, &w4), FROMINT(&w8, 27), &w5), &w6), &n, &g);  // singularity check
    }

    // If we got lucky, return lucky factor
    if ( COMPARE( &g, FROMINT( &w1, 1 ) ) > 0 ) {
        ASSIGN(out, &g);
        return;
    }

    // increase k step by step until lcm(1, ..., limit)
    for (int i = 0; i < primeCount; i++) {
        BI32_t p, pp;
        FROMINT( &p, primes[i] );
        ASSIGN( &pp, &p );

        while (COMPARE( &pp, FROMINT( &w1, limit ) ) < 0 ) {
            ELLIPTIC_MUL(&p, q, &a, &b, &n, q);
            if ( COMPARE( &q[2], FROMINT( &w1, 1 ) ) > 0 ) {
                BI32_gcd(&q[2], &n, out);
                return;
            }
            MUL( &p, &pp, &pp);
        }
    }
    FROMINT(out, 0);
}


void LENSTRA_TEST(BI32_t *n0, int limit, int print) {

    PRIMES();
    BI32_t p, n;
    ASSIGN(&n, n0);
    FROMINT(&p, 1);
    for (int i=0;i < 5; i++) {
        BI32_t m;
        if (print) {
            printf("n:"); PRINTD(&n);
        }
        LENSTRA(&n, limit, &m);
        if (ISZERO(&m))
            continue;
        BI32_t tmp;
        DIV(&n, &m, &tmp);
        ASSIGN(&n, &tmp);
        MUL(&p, &m, &p);
        if (print)
            PRINTD(&m);
    }
}