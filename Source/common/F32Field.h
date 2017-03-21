/*
	Finite field of order 2^32 implemented as quadratic field extension of F_2^16 with minimal polynomial X^2 + X + 8192
*/

#ifndef F32FIELD_H
#define F32FIELD_H

#include <stdint.h>
#include "F16Field.h"
#include "parameters.h"

typedef struct {
	f16FELT c0, c1;
} f32FELT;

static const f32FELT f32ONE = { 1 , 0 };
static const f32FELT f32ZERO = { 0 , 0 };

f32FELT f32IntToFELT(uint64_t a);
f32FELT f32RandomFELT(csprng* rng);
f32FELT f32RandomMTFELT(twister* MT);
void f32PrintFELT(f32FELT a);
f32FELT f32Add(f32FELT a, f32FELT b);
f32FELT f32Multiply(f32FELT a, f32FELT b);
void f32ScalarMultiply(f32FELT* a, f16FELT b);
f32FELT f32Inverse(f32FELT a);
int f32IsEqual(f32FELT a, f32FELT b);
void f32AddBtoA(f32FELT *A, const f32FELT *B);

void serialize_f32FELT(writer *W, f32FELT a);
f32FELT deserialize_f32FELT(reader *R);

#ifdef F32
#include "F32Field.h"
#define FELT f32FELT
#define intToFELT f32IntToFELT
#define serialize_FELT serialize_f32FELT
#define deserialize_FELT deserialize_f32FELT
#define randomFELT f32RandomFELT
#define randomMTFELT f32RandomMTFELT
#define printFELT f32PrintFELT
#define isEqual f32IsEqual
#define multiply f32Multiply
#define scalarMultiply f32ScalarMultiply
#define minus(x) (x)
#define add f32Add
#define inverse f32Inverse
#define ZERO f32ZERO
#define ONE f32ONE
#define addBtoA f32AddBtoA
#endif

#endif
