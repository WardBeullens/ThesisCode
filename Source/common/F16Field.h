/*
	Finite field of order 2^16 implemented as polynomial ring F_2[x] mod x^16+x^12 + x^3 + 1
*/

#ifndef F16FIELD_H
#define F16FIELD_H

#include <stdint.h>
#include "csprng.h"
#include "buffer.h"
#include "twister.h"
#include "parameters.h"

enum { twoPow16 = 65536, f16units = twoPow16 - 1 };
typedef uint16_t f16FELT;

/* Field operations */

f16FELT f16RandomFELT(csprng* rng);
f16FELT f16RandomMTFELT(twister* rng);
void f16PrintFELT(f16FELT a);
f16FELT f16Add(f16FELT a, f16FELT b);
f16FELT f16Multiply(f16FELT a, f16FELT b);
f16FELT f16Inverse(f16FELT a); 
void f16AddBtoA(f16FELT *A, const f16FELT *B);
uint16_t f16Log(f16FELT);
f16FELT f16Antilog(uint16_t);

/* serialization/deserialization */

void serialize_f16FELT(writer *W, f16FELT a);
f16FELT deserialize_f16FELT(reader *R);

/*
Extended Euclidean Algorithm , used for calculating inverses in an extension field of F_2^16.
Given polynomials a and b this calculates the gcd of a and b and polynomials x and y such that a*x + b*y = gcd.
*/
void f16ExtendedEuclideanAlgorithm(f16FELT *a, f16FELT *b, f16FELT *x, f16FELT *y, f16FELT *gcd);

#ifdef F16
#define BITS_PER_FELT 16
#define FELT f16FELT
#define intToFELT(x) (x)
#define serialize_FELT serialize_f16FELT
#define deserialize_FELT deserialize_f16FELT
#define randomFELT f16RandomFELT
#define randomMTFELT f16RandomMTFELT
#define printFELT f16PrintFELT
#define isEqual(X,Y) (X==Y)
#define multiply f16Multiply
#define minus(x) (x)
#define add f16Add
#define inverse f16Inverse
#define ZERO 0
#define ONE 1
#define addBtoA f16AddBtoA
#define scalarMultiply f16scalarMultiply
#endif

#endif
