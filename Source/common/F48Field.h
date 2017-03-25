/* 
	Finite field of order 2^48 implemented as a cubic field extension of F_2^16 with minimal polynomial X^3 + X + 1 
*/

#ifndef F48FIELD_H
#define F48FIELD_H

#include <stdint.h>
#include "F16Field.h"
#include "parameters.h"

typedef struct {
	f16FELT coef[3];
} f48FELT;


static const f48FELT f48ONE = { { 1,0,0 } };
static const f48FELT f48ZERO = { { 0,0,0 } };

f48FELT f48IntToFELT(uint64_t a);
f48FELT f48RandomFELT(csprng* rng);
f48FELT f48RandomMTFELT(twister* MT);
void f48PrintFELT(f48FELT a);
f48FELT f48Add(f48FELT a, f48FELT b);
f48FELT f48Multiply(f48FELT a, f48FELT b);
void f48ScalarMultiply(f48FELT* a, f16FELT b);
f48FELT f48Inverse(f48FELT a);
int f48IsEqual(f48FELT a, f48FELT b);
void f48AddBtoA(f48FELT *A, const f48FELT *B);

void serialize_f48FELT(writer *W, f48FELT a);
f48FELT deserialize_f48FELT(reader *R);


#ifdef F48
#define BITS_PER_FELT 48
#define FELT f48FELT
#define intToFELT f48IntToFELT
#define serialize_FELT serialize_f48FELT
#define deserialize_FELT deserialize_f48FELT
#define randomFELT f48RandomFELT
#define randomMTFELT f48RandomMTFELT
#define printFELT f48PrintFELT
#define isEqual f48IsEqual
#define multiply f48Multiply
#define scalarMultiply f48ScalarMultiply
#define minus(x) (x)
#define add f48Add
#define inverse f48Inverse
#define ZERO f48ZERO
#define ONE f48ONE
#define addBtoA f48AddBtoA
#endif

#endif
