#include <stdio.h>
#include <stdlib.h>

#include "PrimeField.h"
#include "F16Field.h"

#if FIELDPRIME == 31
	const pfFELT inverseLookup[] = { 0,1,16,21,8,25,26,9,4,7,28,17,13,12,20,29,2,11,19,18,14,3,24,27,22,5,6,23,10,15,30 };
#endif

#if FIELDPRIME == 127
	const pfFELT inverseLookup[] = { 0,1,64,85,32,51,106,109,16,113,89,104,53,88,118,17,8,15,120,107,108,121,52,116,90,61,44,80,59,92,72,41,4,77,71,98,60,103,117,114,54,31,124,65,26,48,58,100,45,70,94,5,22,12,40,97,93,78,46,28,36,25,84,125,2,43,102,91,99,81,49,34,30,87,115,105,122,33,57,82,27,69,79,101,62,3,96,73,13,10,24,67,29,56,50,123,86,55,35,68,47,83,66,37,11,75,6,19,20,7,112,119,110,9,39,74,23,38,14,111,18,21,76,95,42,63,126};
#endif

void pfPrintFELT(pfFELT a) {
	printf("%d ", (int)a);
	if (a < 10) {
		printf(" ");
	}
}

void pfAddBtoA(pfFELT *a, const pfFELT *b) {
	*a = (*a + *b) % FIELDPRIME;
}

pfFELT pfInverse(pfFELT a) {
	return inverseLookup[a];
}