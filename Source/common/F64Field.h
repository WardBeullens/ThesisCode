/* 
	Finite field of order 2^64 implemented as a degree-4 field extension of F_2^16 with minimal polynomial X^4 + X^2 + 2X + 1 
*/

#ifndef F64FIELD_H
#define F64FIELD_H

#include <stdint.h>
#include "F16Field.h"
#include "parameters.h"

typedef struct {
	f16FELT coef[4];
} f64FELT;

static const f64FELT f64ONE = { { 1,0,0,0 } };
static const f64FELT f64ZERO = { { 0,0,0,0 } };

f64FELT f64IntToFELT(uint64_t a);
f64FELT f64RandomFELT(csprng* rng);
f64FELT f64RandomMTFELT(twister* MT);

void f64PrintFELT(f64FELT a);

f64FELT f64Add(f64FELT a, f64FELT b);
f64FELT f64Multiply(f64FELT a, f64FELT b);
void f64ScalarMultiply(f64FELT* a, f16FELT b);
f64FELT f64Inverse(f64FELT a);
int f64IsEqual(f64FELT a, f64FELT b);

void serialize_f64FELT(writer *W, f64FELT a);
f64FELT deserialize_f64FELT(reader *R);
void f64AddBtoA(f64FELT *A, const f64FELT *B);

#ifdef F64
#define BITS_PER_FELT 64
#define FELT f64FELT
#define intToFELT f64IntToFELT
#define serialize_FELT serialize_f64FELT
#define deserialize_FELT deserialize_f64FELT
#define randomFELT f64RandomFELT
#define randomMTFELT f64RandomMTFELT
#define printFELT f64PrintFELT
#define isEqual f64IsEqual
#define multiply f64Multiply
#define scalarMultiply f64ScalarMultiply
#define minus(x) (x)
#define add f64Add
#define inverse f64Inverse
#define ZERO f64ZERO
#define ONE f64ONE
#define addBtoA f64AddBtoA
#endif

#endif

