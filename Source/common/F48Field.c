#include <stdio.h>
#include <stdlib.h>

#include "F48Field.h"

/*
	Makes a new field element, given its coeficients
*/
f48FELT newF48FELT(f16FELT c0, f16FELT c1, f16FELT c2) {
	f48FELT new;
	new.coef[0] = c0;
	new.coef[1] = c1;
	new.coef[2] = c2;
	return new;
}

/*
	Write a field element to a char array

	W : writer object
	a : field element to write
*/
void serialize_f48FELT(writer *W, f48FELT a) {
	serialize_f16FELT(W, a.coef[0]);
	serialize_f16FELT(W, a.coef[1]);
	serialize_f16FELT(W, a.coef[2]);
}

/*
	Read a field element from a char array

	R : reader object

	returns : a field element
*/
f48FELT deserialize_f48FELT(reader *R) {
	f48FELT new;
	new.coef[0] = deserialize_f16FELT(R);
	new.coef[1] = deserialize_f16FELT(R);
	new.coef[2] = deserialize_f16FELT(R);
	return new;
}

/*
	Irreducible polynomial over F_16
*/
static f16FELT f48IrredPoly[6] = { 1,1,0,1,0,0 };

/*
	Converts an integer to a field element
*/
f48FELT f48IntToFELT(uint64_t a) {
	return newF48FELT(a, a >> 16 , a>>32);
}

/*
	Generates a field element from a Keccak csprng

	rng : pseudorandom number generator object

	returns : a pseudorandomly generated field element
*/
f48FELT f48RandomFELT(csprng* rng) {
	return newF48FELT(f16RandomFELT(rng), f16RandomFELT(rng), f16RandomFELT(rng));
}

/*
	Generates a pseudorandom field element from a mersenne twister

	MT : mersenne twister object

	returns: a pseudorandomly generated field element
*/
f48FELT f48RandomMTFELT(twister* MT) {
	uint32_t a = randomMT(MT);
	uint32_t b = randomMT(MT);
	return newF48FELT(a, a >> 16,b);
}

/*
	prints a field element, mainly used for debugging
*/
void f48PrintFELT(f48FELT a) {
	printf("%.5uX^2 + %.5uX + %.5u ", a.coef[2], a.coef[1], a.coef[0]);
}

/*
	Adds two field elements

	a,b : field element to add

	return : the sum of a and b
*/
f48FELT f48Add(f48FELT a, f48FELT b) {
	return newF48FELT(a.coef[0] ^ b.coef[0], a.coef[1] ^ b.coef[1], a.coef[2] ^ b.coef[2]);
}

/*
	Add one field element to another

	A : Field element that is set to A+B
	B : Field element that is added
*/
void f48AddBtoA(f48FELT *A, const f48FELT *B) {
	A->coef[0] ^= B->coef[0];
	A->coef[1] ^= B->coef[1];
	A->coef[2] ^= B->coef[2];
}

/*
	Multiplies two field elements

	a,b : field element to multiply

	return : the product of a and b
*/
f48FELT f48Multiply(f48FELT a, f48FELT b) {
	f16FELT c0 = f16Multiply(a.coef[0], b.coef[0]);
	f16FELT c1 = f16Multiply(a.coef[1], b.coef[0]) ^ f16Multiply(a.coef[0], b.coef[1]);
	f16FELT c2 = f16Multiply(a.coef[2], b.coef[0]) ^ f16Multiply(a.coef[1], b.coef[1]) ^ f16Multiply(a.coef[0], b.coef[2]);
	f16FELT c3 = f16Multiply(a.coef[2], b.coef[1]) ^ f16Multiply(a.coef[1], b.coef[2]);
	f16FELT c4 = f16Multiply(a.coef[2], b.coef[2]);

	return newF48FELT(c0 ^ c3, c1 ^ c3 ^ c4, c2 ^ c4);
}

/*
	Scalar multiplication

	a : an element in the field extension, which is muliplied by 2^b
	b : an integer
*/
void f48ScalarMultiply(f48FELT* a, f16FELT b) {
	if (a->coef[0] != 0) {
		a->coef[0] = f16Antilog((f16Log(a->coef[0]) + b) % f16units);
	}
	if (a->coef[1] != 0) {
		a->coef[1] = f16Antilog((f16Log(a->coef[1]) + b) % f16units);
	}
	if (a->coef[2] != 0) {
		a->coef[2] = f16Antilog((f16Log(a->coef[2]) + b) % f16units);
	}
}

/*
	Inverts a field element

	a : field element to invert

	return : the inverse of a, if a is nonzero
*/
f48FELT f48Inverse(f48FELT a) {
	f16FELT aPoly[6];
	f16FELT gcd[6], x[6], y[6];
	f16FELT gcdinv;
	aPoly[0] = a.coef[0];
	aPoly[1] = a.coef[1];
	aPoly[2] = a.coef[2];
	aPoly[3] = 0;
	aPoly[4] = 0;
	aPoly[5] = 0;

	f16ExtendedEuclideanAlgorithm(aPoly, f48IrredPoly, x, y, gcd);
	gcdinv = f16Inverse(gcd[0]);
	return newF48FELT(f16Multiply(gcdinv, x[0]), f16Multiply(gcdinv, x[1]), f16Multiply(gcdinv, x[2]));
}

/*
	Checks if two field elements are equal
*/
int f48IsEqual(f48FELT a, f48FELT b) {
	return (a.coef[0] == b.coef[0]) && (a.coef[1] == b.coef[1]) && (a.coef[2] == b.coef[2]);
}