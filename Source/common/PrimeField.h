#include "parameters.h"

#ifndef PRIMEFIELD_H
#define PRIMEFIELD_H

/* Prime number */
#ifndef FIELDPRIME
#define FIELDPRIME 31
#endif

#if FIELDPRIME == 31
#define BITS 5
#endif
#if FIELDPRIME == 127
#define BITS 7
#endif


/* Field element of F_Q */
typedef unsigned char pfFELT;

void pfPrintFELT(pfFELT a);
void pfAddBtoA(pfFELT *a, const pfFELT *b);
pfFELT pfInverse(pfFELT a);

#ifdef PRIME_FIELD

#define BITS_PER_FELT BITS
#define FELT pfFELT
#define intToFELT(x) (x%FIELDPRIME)
#define serialize_FELT(W,x) serialize_uint64_t(W,x,BITS)
#define deserialize_FELT(R) ((pfFELT)deserialize_uint64_t(R,BITS))
#define randomFELT(rng) (f16RandomFELT(rng)%FIELDPRIME)
#define randomMTFELT(MT) (f16RandomMTFELT(MT)%FIELDPRIME)
#define printFELT pfPrintFELT
#define isEqual(a,b) (a == b)
#define multiply(a,b) ((a*b)%FIELDPRIME)
#define minus(x) ((FIELDPRIME-x)%FIELDPRIME)
#define add(a,b) ((a+b)%FIELDPRIME)
#define inverse pfInverse
#define ZERO (0)
#define ONE (1)
#define addBtoA pfAddBtoA

#endif

#endif
