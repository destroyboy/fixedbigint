//
// Created by Alexis Farmer on 2019-04-11.
//

#ifndef FIXEDBIGINT_VARINT_H
#define FIXEDBIGINT_VARINT_H

#include <stdint.h>

# define VARINT_DWORDS (8)

typedef struct {
    uint64_t __[VARINT_DWORDS];
} V;

#define ASSIGN(dst,src) *(dst) = *(src)

int V_isNegative(V* a);

V* V_fromInt(V* a, uint64_t n);
V* V_add(V* a, V* b, V*c);
V* V_sub(V* a, V* b, V*c);
V* V_negate(V *in, V*out);
void V_NEGATE_INPLACE(V* q);
void V_divmod(V* m0, V* n0, V* q, V* r);
V* V_MUL(V *a, V *b, V *c);

void V_lshift_bit(V *a);
void V_rshift_bit(V *a);
void V_add_int(V *a, uint64_t carry, int shift_left);

void V_printRawHex(V* a);
void V_printHex(V* a);
void V_PRINTD(V *n0);


void v_random_digits(int digits, V* out);
void V_LENSTRA_TEST(V *n0, int limit);

#endif //FIXEDBIGINT_VARINT_H
