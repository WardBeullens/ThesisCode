#include <stdio.h>
#include <stdlib.h>

#include "F32Field.h"

/*
	Makes a new field element, given its coeficients
*/
f32FELT newF32FELT(f16FELT c0, f16FELT c1) {
	f32FELT new;
	new.c0 = c0;
	new.c1 = c1;
	return new;
}

/*
	Write a field element to a char array

	W : writer object
	a : field element to write
*/
void serialize_f32FELT(writer *W, f32FELT a) {
	serialize_f16FELT(W, a.c0);
	serialize_f16FELT(W, a.c1);
}

/*
	Read a field element from a char array

	R : reader object

	returns : a field element
*/
f32FELT deserialize_f32FELT(reader *R) {
	f32FELT new;
	new.c0 = deserialize_f16FELT(R);
	new.c1 = deserialize_f16FELT(R);
	return new;
}

/*
	Converts an integer to a field element
*/
f32FELT f32IntToFELT(uint64_t a) {
	return newF32FELT((uint16_t) a, (uint16_t) (a>>16));
}

/*
	Generates a field element from a Keccak csprng

	rng : pseudorandom number generator object

	returns : a pseudorandomly generated field element
*/
f32FELT f32RandomFELT(csprng* rng) {
	return newF32FELT(f16RandomFELT(rng), f16RandomFELT(rng));
}

/*
	Generates a pseudorandom field element from a mersenne twister

	MT : mersenne twister object

	returns: a pseudorandomly generated field element
*/
f32FELT f32RandomMTFELT(twister* MT) {
	uint32_t a = randomMT(MT);
	return newF32FELT((uint16_t) a, (uint16_t) (a >> 16));
}

/*
	prints a field element, mainly used for debugging
*/
void f32PrintFELT(f32FELT a) {
	printf("%.5uX + %.5u ", a.c1, a.c0);
}

/*
	Adds two field elements

	a,b : field element to add

	return : the sum of a and b
*/
f32FELT f32Add(f32FELT a, f32FELT b) {
	return newF32FELT(f16Add(a.c0, b.c0), f16Add(a.c1, b.c1));
}

/*
	Add one field element to another

	A : Field element that is set to A+B
	B : Field element that is added
*/
void f32AddBtoA(f32FELT *A, const f32FELT *B) {
	A->c0 ^= B->c0;
	A->c1 ^= B->c1;
}

/*
	Multiplies two field elements

	a,b : field element to multiply

	return : the product of a and b
*/
f32FELT f32MultiplyOld(f32FELT a, f32FELT b) {
	f16FELT c0 = f16Multiply(a.c0, b.c0);
	f16FELT c1 = f16Multiply(a.c1, b.c0) ^ f16Multiply(a.c0, b.c1);
	f16FELT c2 = f16Multiply(a.c1, b.c1);

	return newF32FELT(c0 ^ f16Multiply(c2, 8192), c1 ^ c2);
}

/*
	Scalar multiplication

	a : an element in the field extension, which is muliplied by 2^b
	b : an integer
*/
void f32ScalarMultiply(f32FELT* a, f16FELT b) {
	if (a->c0 != 0) {
		a->c0 = f16Antilog((f16Log(a->c0) + b) % f16units);
	}
	if (a->c1 != 0) {
		a->c1 = f16Antilog((f16Log(a->c1) + b) % f16units);
	}
}

/*
	Multiplies two field elements, optimized in 
	the case the coefficients of a and b are nonzero

	a,b : field element to multiply

	return : the product of a and b
*/
f32FELT f32Multiply(f32FELT a, f32FELT b) {
	if (a.c0 == 0 || a.c1 == 0 || b.c0 == 0 || b.c1 == 0) {
		return f32MultiplyOld(a, b);
	}
	uint16_t A, B, C, E;
	A = f16Log(a.c0);
	B = f16Log(a.c1);
	C = f16Log(b.c0);
	E = f16Log(b.c1);

	return newF32FELT(f16Antilog((A+C)%f16units) ^ f16Antilog((B+E+13)%f16units) , f16Antilog((B+C)%f16units) ^ f16Antilog((A+E)%f16units) ^ f16Antilog((B + E) % f16units) );
}

/*
	Inverts a field element

	a : field element to invert

	return : the inverse of a, if a is nonzero
*/
f32FELT f32Inverse(f32FELT a) {
	f16FELT a1inv,temp2;
	f32FELT temp;

	if (a.c1 == 0)
		return newF32FELT(f16Inverse(a.c0), 0);

	a1inv = f16Inverse(a.c1);
	temp = newF32FELT(f16Multiply(a1inv, f16Add(1, f16Multiply(a1inv, a.c0))), a1inv);
	temp2 = f16Inverse(f32Multiply(a, temp).c0);

	return newF32FELT(f16Multiply(temp.c0, temp2), f16Multiply(temp.c1, temp2));
}

/*
	Checks if two field elements are equal
*/
int f32IsEqual(f32FELT a, f32FELT b) {
	return (a.c0 == b.c0) && (a.c1 == b.c1);
}