/* 
	Finite field of order 2^80 implemented as a degree-5 field extension of F_2^16 with minimal polynomial X^5 + X^2 + 1 
*/

#ifndef F80FIELD_H
#define F80FIELD_H

#include <stdint.h>
#include "F16Field.h"
#include "parameters.h"

typedef struct {
	f16FELT coef[5];
} f80FELT;

static const f80FELT f80ONE = { { 1,0,0,0,0 } };
static const f80FELT f80ZERO = { { 0,0,0,0,0 } };

f80FELT f80IntToFELT(uint64_t a);
f80FELT f80RandomFELT(csprng* rng);
f80FELT f80RandomMTFELT(twister* MT);

void f80PrintFELT(f80FELT a);

f80FELT f80Add(f80FELT a, f80FELT b);
f80FELT f80Multiply(f80FELT a, f80FELT b);
void f80ScalarMultiply(f80FELT* a, f16FELT b);
f80FELT f80Inverse(f80FELT a);
int f80IsEqual(f80FELT a, f80FELT b);

void serialize_f80FELT(writer *W, f80FELT a);
f80FELT deserialize_f80FELT(reader *R);
void f80AddBtoA(f80FELT *A, const f80FELT *B);

#ifdef F80
#include "F80Field.h"
#define FELT f80FELT
#define intToFELT f80IntToFELT
#define serialize_FELT serialize_f80FELT
#define deserialize_FELT deserialize_f80FELT
#define randomFELT f80RandomFELT
#define randomMTFELT f80RandomMTFELT
#define printFELT f80PrintFELT
#define isEqual f80IsEqual
#define multiply f80Multiply
#define scalarMultiply f80ScalarMultiply
#define minus(x) (x)
#define add f80Add
#define inverse f80Inverse
#define ZERO f80ZERO
#define ONE f80ONE
#define addBtoA f80AddBtoA
#endif

#endif
