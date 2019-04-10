//
// Created by Alexis Farmer on 2019-04-08.
//
#include <string.h>

#include "lenstra.h"
#include "bigint.h"

#include <stdio.h>
#include <assert.h>


long _seed = 123456789;
long _m = 0x100000000L;
long _a = 1103515245;
long _c = 12345;

long _rand()
{
    _seed = (_a * _seed + _c) % _m;
    //print(_seed); print ("\n");
    return _seed;
}

uint64_t ran_count=0;

void k_random_digits(int digits, I* out) {
    char n[256];
    for (int i=0; i<digits; i++)
        n[i] = '0'+(int)(_rand()%10);
    n[digits] = '\0';
    FROMSTRING(out, n);
    //printf("rnd:"); PRINTD(out);
}



void k_random_range(I* n, I* out) {
    char buf[256];
    I m;
    //printf("in range:");
    //PRINTD(n);
    TOSTRING(n, buf);
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
void MODULAR_INV(I* a, I* b, I out[3]) {

    if (ISZERO(b)) {
        FROMINT(&out[0], 1);
        FROMINT(&out[1], 0);
        ASSIGN(&out[2], a);
        return;
    }

    I q,r;

    DIVMOD0( a, b, &q, &r);
    // String q=t[0], r=t[1];
    const int X=0, Y=1, G=2;
    // String[] xyg = modular_inv(b, r, t);
    I xyg[3];
    MODULAR_INV(b, &r, xyg);
    ASSIGN(&out[0], &xyg[Y]);
    SUB(&xyg[X], MUL(&q, &xyg[Y], &out[1]), &out[1]);
    ASSIGN(&out[2], &xyg[G]);
}

// Addition in Elliptic curve modulo m space
void ELLIPTIC_ADD(I p[3], I q[3], I* a, I* b, I* m, I out[3]) {

    // If one point is infinity, return other one
    if (ISZERO(&p[2])) {
        memcpy(out, q, 3*sizeof(I));
        return;
    }

    if (ISZERO(&q[2])) {
        memcpy(out, p, 3*sizeof(I));
        return;
    }

    I num, denom, w1, w2, w3;

    if ( COMPARE( &p[0], &q[0] ) == 0 ) {
        if ( ISZERO( MOD( ADD(&p[1], &q[1], &w1 ), m, &w2 ) ) ) {
            FROMINT( &out[0], 0 );
            FROMINT( &out[1], 1 );
            FROMINT( &out[2], 0 );
            return;
        }
        MOD( ADD( MULx3(MUL(&p[0], &p[0], &w1), &w2 ), a, &w3 ), m, &num );
        MOD( MULx2( &p[1], &w1 ), m, &denom );
    }
    else {
        MOD(SUB(&q[1], &p[1], &w1), m, &num );
        MOD(SUB(&q[0], &p[0], &w1), m, &denom );
    }

    I inv_g[3];

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

    I num_times_inv;
    MUL(&num, &inv_g[INV], &num_times_inv);
    I z;

    MOD(SUB(MUL(&num_times_inv, &num_times_inv, &w1), ADD(&p[0], &q[0], &w2), &w3), m, &z );

    ASSIGN( &out[0], &z );
    MOD(SUB(MUL(&num_times_inv, SUB(&p[0], &z, &w1), &w2), &p[1], &w3), m, &out[1]);
    FROMINT(&out[2], 1);
}

// Multiplication (repeated addition and doubling)
void ELLIPTIC_MUL(I* k0, I p0[3], I* a, I* b, I* m, I out[3])
{
    I r[3], p[3], w1, w2, w3, k, w4[3];

    ASSIGN(&k, k0);
    memcpy(p, p0, 3*sizeof(I));
    FROMINT(&r[0], 0);
    FROMINT(&r[1], 1);
    FROMINT(&r[2], 0);

    while ( !ISZERO( &k ) ){
        // p is failure, return it
        if ( COMPARE(&p[2],FROMINT(&w1, 1)) > 0 ) {
            memcpy(out, p, 3*sizeof(I));
            return;
        }

        if ( COMPARE( MOD( &k, FROMINT( &w1, 2 ), &w3), FROMINT(&w2, 1))==0) {
            ELLIPTIC_ADD(p, r, a, b, m, w4);
            memcpy(r, w4, 3*sizeof(I));
        }
        DIVx2(&k);
        ELLIPTIC_ADD(p, p, a, b, m, w4);
        memcpy(p, w4, 3*sizeof(I));
    }

    memcpy(out, r, 3*sizeof(I));
}

// Lenstra's algorithm for factoring
// Limit specifies the amount of work permitted
void LENSTRA(I* n0, int limit, I* out) {

    I n, g, a, b, w1, w2, w3, w4, w5, w6;
    ASSIGN(&n, n0);
    ASSIGN(&g, n0);
    FROMINT( &a, 0 );
    FROMINT( &b, 0 );
    I q[3];
    FROMINT( &q[0], 0 );
    FROMINT( &q[1], 1 );
    FROMINT( &q[2], 0 );

    //String[] q = new String[]{"0", "1", "0"}; // infinity? doesnt matter gets overwritten

    while ( COMPARE( &g, &n ) == 0 ) {

        // Randomized x and y
        k_random_range(&n, &q[0]);
        k_random_range(&n, &q[1]);
        FROMINT(&q[2], 1);
        // Randomized curve coefficient a, computed b
        k_random_range(&n, &a);
        MOD( SUB( MUL( &q[1], &q[1], &w1), ADD( MUL( &q[0], MUL( &q[0], &q[0], &w2 ), &w4 ), MUL( &a, &q[0], &w3 ), &w5), &w6), &n, &b);
        GCD( ADD( MUL( MULx4( &a, &w1 ), MUL( &a, &a, &w2 ), &w3 ), MULx27( MUL( &b, &b, &w4 ), &w5 ), &w6 ), &n, &g);  // singularity check
    }

    // If we got lucky, return lucky factor
    if ( COMPARE( &g, FROMINT( &w1, 1 ) ) >0 ) {
        ASSIGN(out, &g);
        return;
    }

    // increase k step by step until lcm(1, ..., limit)
    for (int i = 0; i < primeCount; i++) {
        I p, pp;
        FROMINT( &p, primes[i] );
        ASSIGN( &pp, &p );

        while (COMPARE( &pp, FROMINT( &w1, limit ) ) < 0 ) {
            ELLIPTIC_MUL(&p, q, &a, &b, &n, q);
            if ( COMPARE( &q[2], FROMINT( &w1, 1 ) ) > 0 ) {
                GCD( &q[2], &n, out );
                return;
            }
            MUL( &p, &pp, &pp);
        }
    }
    FROMINT(out, 0);
}


void LENSTRA_TEST(I *n0, int limit) {

    PRIMES();

    I p, n;
    ASSIGN(&n, n0);
    FROMINT(&p, 1);
    //printf("p:");PRINTD(&p);
    for (int i=0;i < 5; i++) {
        I m;
        printf("n:"); PRINTD(&n);
        char buf[256];
        TOSTRING(&n, buf);
        //if (strcmp(buf, "1009012267184813")==0)
        //    printf("%ld\n", _seed);
        LENSTRA(&n, limit, &m);
        //break;
        if (ISZERO(&m))
            continue;
        //printf("1n:");PRINTD(n);
        //printf("1m:");PRINTD(&m);
        //printf("1p:");PRINTD(&p);
        I tmp;
        DIV(&n, &m, &tmp);
        ASSIGN(&n, &tmp);
        MUL(&p, &m, &p);
        //printf("2n:");PRINTD(n);
        PRINTD(&m);
        //break;
        //printf("2p:");PRINTD(&p);
    }

    //printf("%d\n", ran_count);
}