#include <stdio.h>
#include <stdlib.h>

#include "F80Field.h"

/*
	Makes a new field element, given its coeficients
*/
f80FELT newF80FELT(f16FELT c0, f16FELT c1, f16FELT c2, f16FELT c3 , f16FELT c4) {
	f80FELT new;
	new.coef[0] = c0;
	new.coef[1] = c1;
	new.coef[2] = c2;
	new.coef[3] = c3;
	new.coef[4] = c4;
	return new;
}

/*
	Irreducible polynomial over F_16
*/
static f16FELT f80IrredPoly[6] = { 1,0,1,0,0,1 };

/*
	Converts an integer to a field element
*/
f80FELT f80IntToFELT(uint64_t a) {
	return newF80FELT(a, a >> 16, a >> 32, a >> 48 , 0);
}

/*
	Generates a field element from a Keccak csprng

	rng : pseudorandom number generator object

	returns : a pseudorandomly generated field element
*/
f80FELT f80RandomFELT(csprng* rng) {
	return newF80FELT(f16RandomFELT(rng), f16RandomFELT(rng), f16RandomFELT(rng), f16RandomFELT(rng) , f16RandomFELT(rng));
}

/*
	Generates a pseudorandom field element from a mersenne twister

	MT : mersenne twister object

	returns: a pseudorandomly generated field element
*/
f80FELT f80RandomMTFELT(twister* MT) {
	uint32_t a = randomMT(MT);
	uint32_t b = randomMT(MT);
	uint32_t c = randomMT(MT);
	return newF80FELT(a, a >> 16, b , b>>16 , c);
}

/*
	prints a field element, mainly used for debugging
*/
void f80PrintFELT(f80FELT a) {
	/*printf("%.5uX^4 + %.5uX^3 + %.5uX^2 + %.5uX + %.5u ", a.coef[4], a.coef[3], a.coef[2], a.coef[1], a.coef[0]); */
	if (f80IsEqual(f80ZERO, a)) {
		printf(".");
	}
	else {
		printf("x");
	}
}

/*
	Adds two field elements

	a,b : field element to add

	return : the sum of a and b
*/
f80FELT f80Add(f80FELT a, f80FELT b) {
	return newF80FELT(a.coef[0] ^ b.coef[0], a.coef[1] ^ b.coef[1], a.coef[2] ^ b.coef[2], a.coef[3] ^ b.coef[3], a.coef[4] ^ b.coef[4]);
}

/*
	Add one field element to another

	A : Field element that is set to A+B
	B : Field element that is added
*/
void f80AddBtoA(f80FELT *A, const f80FELT *B) {
	A->coef[0] ^= B->coef[0];
	A->coef[1] ^= B->coef[1];
	A->coef[2] ^= B->coef[2];
	A->coef[3] ^= B->coef[3];
	A->coef[4] ^= B->coef[4];
}


/*
	Multiplies two field elements

	a,b : field element to multiply

	return : the product of a and b
*/
f80FELT f80Multiply(f80FELT a, f80FELT b) {
	int i, j;
	/* should be possible with much less table lookups */

	f16FELT temp[9];
	for (i = 0; i < 9; i++) {
		temp[i] = 0;
		for (j = ((0 > i-4)? 0: i - 4) ; j <= ((4<i)?4:i) ; j++) {
			temp[i] ^= f16Multiply(a.coef[j], b.coef[i - j]);
		}
	}

	return newF80FELT(temp[0] ^ temp[5] ^ temp[8], temp[1] ^ temp[6], temp[2] ^ temp[5] ^ temp[7] ^ temp[8], temp[3] ^ temp[6] ^ temp[8], temp[4] ^ temp[7]);
}

/*
	Scalar multiplication

	a : an element in the field extension, which is muliplied by 2^b
	b : an integer
*/
void f80ScalarMultiply(f80FELT* a, f16FELT b) {
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
	if (a->coef[4] != 0) {
		a->coef[4] = f16Antilog((f16Log(a->coef[4]) + b) % f16units);
	}
}

/*
	Inverts a field element

	a : field element to invert

	return : the inverse of a, if a is nonzero
*/
f80FELT f80Inverse(f80FELT a) {
	f16FELT aPoly[6];
	f16FELT gcd[6], x[6], y[6];
	f16FELT gcdinv;

	aPoly[0] = a.coef[0];
	aPoly[1] = a.coef[1];
	aPoly[2] = a.coef[2];
	aPoly[3] = a.coef[3];
	aPoly[4] = a.coef[4];
	aPoly[5] = 0;

	f16ExtendedEuclideanAlgorithm(aPoly, f80IrredPoly, x, y, gcd);
	gcdinv = f16Inverse(gcd[0]);

	return  newF80FELT(f16Multiply(gcdinv, x[0]), f16Multiply(gcdinv, x[1]), f16Multiply(gcdinv, x[2]), f16Multiply(gcdinv, x[3]), f16Multiply(gcdinv, x[4]));
}

/*
	Checks if two field elements are equal
*/
int f80IsEqual(f80FELT a, f80FELT b) {
	return (a.coef[0] == b.coef[0]) && (a.coef[1] == b.coef[1]) && (a.coef[2] == b.coef[2]) && (a.coef[3] == b.coef[3]) && (a.coef[4] == b.coef[4]);
}

/*
	Write a field element to a char array

	W : writer object
	a : field element to write
*/
void serialize_f80FELT(writer *W, f80FELT a) {
	serialize_f16FELT(W, a.coef[0]);
	serialize_f16FELT(W, a.coef[1]);
	serialize_f16FELT(W, a.coef[2]);
	serialize_f16FELT(W, a.coef[3]);
	serialize_f16FELT(W, a.coef[4]);
}

/*
	Read a field element from a char array

	R : reader object

	returns : a field element
*/
f80FELT deserialize_f80FELT(reader *R) {
	f80FELT new;
	new.coef[0] = deserialize_f16FELT(R);
	new.coef[1] = deserialize_f16FELT(R);
	new.coef[2] = deserialize_f16FELT(R);
	new.coef[3] = deserialize_f16FELT(R);
	new.coef[4] = deserialize_f16FELT(R);
	return new;
}