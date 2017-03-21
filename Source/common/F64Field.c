#include <stdio.h>
#include <stdlib.h>

#include "F64Field.h"

/*
	Makes a new field element, given its coeficients
*/
f64FELT newF64FELT(f16FELT c0, f16FELT c1, f16FELT c2, f16FELT c3) {
	f64FELT new;
	new.coef[0] = c0;
	new.coef[1] = c1;
	new.coef[2] = c2;
	new.coef[3] = c3;
	return new;
}

/*
	Irreducible polynomial over F_16
*/
static f16FELT f64IrredPoly[6] = { 1,2,1,0,1,0 };

/*
	Converts an integer to a field element
*/
f64FELT f64IntToFELT(uint64_t a) {
	return newF64FELT(a, a >> 16, a >> 32 , a >> 48);
}

/*
	Generates a field element from a Keccak csprng

	rng : pseudorandom number generator object

	returns : a pseudorandomly generated field element
*/
f64FELT f64RandomFELT(csprng* rng) {
	return newF64FELT(f16RandomFELT(rng), f16RandomFELT(rng), f16RandomFELT(rng), f16RandomFELT(rng));
}

/*
	Generates a pseudorandom field element from a mersenne twister

	MT : mersenne twister object

	returns: a pseudorandomly generated field element
*/
f64FELT f64RandomMTFELT(twister* MT) {
	uint32_t a = randomMT(MT);
	uint32_t b = randomMT(MT);
	return newF64FELT(a, a >> 16, b , b>> 16);
}

/*
	prints a field element, mainly used for debugging
*/
void f64PrintFELT(f64FELT a) {
	/* printf("%.5uX^3 + %.5uX^2 + %.5uX + %.5u ", a.coef[3], a.coef[2], a.coef[1], a.coef[0]); */
	if (f64IsEqual(f64ZERO, a)) {
		printf(".");
	}
	else{
		printf("x");
	}
}

/*
	Adds two field elements

	a,b : field element to add

	return : the sum of a and b
*/
f64FELT f64Add(f64FELT a, f64FELT b) {
	return newF64FELT(a.coef[0] ^ b.coef[0], a.coef[1] ^ b.coef[1], a.coef[2] ^ b.coef[2], a.coef[3] ^ b.coef[3]);
}

/*
	Add one field element to another

	A : Field element that is set to A+B
	B : Field element that is added
*/
void f64AddBtoA(f64FELT *A, const f64FELT *B) {
	A->coef[0] ^= B->coef[0];
	A->coef[1] ^= B->coef[1];
	A->coef[2] ^= B->coef[2];
	A->coef[3] ^= B->coef[3];
}

/*
	Multiplies two field elements

	a,b : field element to multiply

	return : the product of a and b
*/
f64FELT f64Multiply(f64FELT a, f64FELT b) {
	f16FELT c0 = f16Multiply(a.coef[0], b.coef[0]);
	f16FELT c1 = f16Multiply(a.coef[1], b.coef[0]) ^ f16Multiply(a.coef[0], b.coef[1]);
	f16FELT c2 = f16Multiply(a.coef[2], b.coef[0]) ^ f16Multiply(a.coef[1], b.coef[1]) ^ f16Multiply(a.coef[0], b.coef[2]);
	f16FELT c3 = f16Multiply(a.coef[3], b.coef[0]) ^ f16Multiply(a.coef[2], b.coef[1]) ^ f16Multiply(a.coef[1], b.coef[2]) ^ f16Multiply(a.coef[0], b.coef[3]);
	f16FELT c4 = f16Multiply(a.coef[3], b.coef[1]) ^ f16Multiply(a.coef[2], b.coef[2]) ^ f16Multiply(a.coef[1], b.coef[3]);
	f16FELT c5 = f16Multiply(a.coef[3], b.coef[2]) ^ f16Multiply(a.coef[2], b.coef[3]);
	f16FELT c6 = f16Multiply(a.coef[3], b.coef[3]);

	return newF64FELT(c0 ^ c4 ^ c6, c1 ^ f16Multiply(2, c4 ^ c6) ^ c5, c2 ^ c4 ^ f16Multiply(2, c5), c3 ^ c5 ^ f16Multiply(2, c6));
}

/*
	Scalar multiplication

	a : an element in the field extension, which is muliplied by 2^b
	b : an integer
*/
void f64ScalarMultiply(f64FELT* a, f16FELT b) {
	if (a->coef[0] != 0) {
		a->coef[0] = f16Antilog((f16Log(a->coef[0]) + b) % f16units);
	}
	if (a->coef[1] != 0) {
		a->coef[1] = f16Antilog((f16Log(a->coef[1]) + b) % f16units);
	}
	if (a->coef[2] != 0) {
		a->coef[2] = f16Antilog((f16Log(a->coef[2]) + b) % f16units);
	}
	if (a->coef[3] != 0) {
		a->coef[3] = f16Antilog((f16Log(a->coef[3]) + b) % f16units);
	}
}

/*
	Inverts a field element

	a : field element to invert

	return : the inverse of a, if a is nonzero
*/
f64FELT f64Inverse(f64FELT a) {
	f16FELT aPoly[6];
	f16FELT gcd[6], x[6], y[6];
	f16FELT gcdinv;

	aPoly[0] = a.coef[0];
	aPoly[1] = a.coef[1];
	aPoly[2] = a.coef[2];
	aPoly[3] = a.coef[3];
	aPoly[4] = 0;
	aPoly[5] = 0;
	
	f16ExtendedEuclideanAlgorithm(aPoly, f64IrredPoly, x, y, gcd);
	gcdinv = f16Inverse(gcd[0]);

	return  newF64FELT(f16Multiply(gcdinv, x[0]), f16Multiply(gcdinv, x[1]), f16Multiply(gcdinv, x[2]), f16Multiply(gcdinv, x[3]));
}

/*
	Checks if two field elements are equal
*/
int f64IsEqual(f64FELT a, f64FELT b) {
	return (a.coef[0] == b.coef[0]) && (a.coef[1] == b.coef[1]) && (a.coef[2] == b.coef[2]) && (a.coef[3] == b.coef[3]);
}

/*
	Write a field element to a char array

	W : writer object
	a : field element to write
*/
void serialize_f64FELT(writer *W, f64FELT a) {
	serialize_f16FELT(W, a.coef[0]);
	serialize_f16FELT(W, a.coef[1]);
	serialize_f16FELT(W, a.coef[2]);
	serialize_f16FELT(W, a.coef[3]);
}

/*
	Read a field element from a char array

	R : reader object

	returns : a field element
*/
f64FELT deserialize_f64FELT(reader *R) {
	f64FELT new;
	new.coef[0] = deserialize_f16FELT(R);
	new.coef[1] = deserialize_f16FELT(R);
	new.coef[2] = deserialize_f16FELT(R);
	new.coef[3] = deserialize_f16FELT(R);
	return new;
}
