/* Implementation of the field used for the MAC-polynomials */
#include "MACField.h"

MAC_FELT MAC_add(MAC_FELT a, MAC_FELT b) {
	int i;
	MAC_FELT sum;
	for (i = 0; i < K; i++)
	{
		sum.coef[i] = (a.coef[i] + b.coef[i])%FIELDPRIME;
	}
	return sum;
}

void serialize_MAC_FELT(writer* W, MAC_FELT a) {
	int i;
	for ( i = 0; i < K; i++)
	{
		serialize_uint64_t(W, a.coef[i], BITS_PER_FELT);
	}
}

MAC_FELT deserialize_MAC_FELT(reader* R) {
	int i;
	MAC_FELT ans;
	for (i = 0; i < K; i++) {
		ans.coef[i] = (unsigned char) deserialize_uint64_t(R,BITS_PER_FELT);
	}
	return ans;
}

MAC_FELT MAC_scalarMultiply(unsigned char a, MAC_FELT b) {
	int i;
	MAC_FELT ans;
	for (i = 0; i < K; i++) {
		ans.coef[i] = (b.coef[i]* a)%FIELDPRIME;
	}
	return ans;
}

int MAC_isEqual(MAC_FELT a, MAC_FELT b) {
	int i;
	for (i = 0; i < K; i++) {
		if (a.coef[i] != b.coef[i]) {
			return 0;
		}
	}
	return 1;
}

MAC_FELT intToMAC_FELT(uint64_t a) {
	int i;
	MAC_FELT ans;
	for (i = 0; i < K; i++) {
		ans.coef[i] = a%FIELDPRIME;
		a /= FIELDPRIME;
	}
	return ans;
}

#if K==4 && FIELDPRIME ==31 /* F31[x]/(x^4-x-1) */
MAC_FELT MAC_multiply(MAC_FELT a, MAC_FELT b) {
	FELT c0 = (a.coef[0] * b.coef[0]) % FIELDPRIME;
	FELT c1 = (a.coef[1] * b.coef[0] + a.coef[0] * b.coef[1]) % FIELDPRIME;
	FELT c2 = (a.coef[2] * b.coef[0] + a.coef[1] * b.coef[1] + a.coef[0] * b.coef[2]) % FIELDPRIME;
	FELT c3 = (a.coef[3] * b.coef[0] + a.coef[2] * b.coef[1] + a.coef[1] * b.coef[2] + a.coef[0] * b.coef[3]) % FIELDPRIME;
	FELT c4 = (a.coef[3] * b.coef[1] + a.coef[2] * b.coef[2] + a.coef[1] * b.coef[3]) % FIELDPRIME;
	FELT c5 = (a.coef[3] * b.coef[2] + a.coef[2] * b.coef[3]) % FIELDPRIME;
	FELT c6 = (a.coef[3] * b.coef[3]) % FIELDPRIME;

	MACFELT product;
	product.coef[0] = add(c0, c4);
	product.coef[1] = (c1 + c4 + c5) % FIELDPRIME;
	product.coef[2] = (c2 + c5 + c6) % FIELDPRIME;
	product.coef[3] = add(c3, c6);
	return product;
}
#endif

#if K==3 && FIELDPRIME ==127 /* F127[x]/(x^3-3) */
MAC_FELT MAC_multiply(MAC_FELT a, MAC_FELT b) {
	MACFELT product;
	product.coef[0] = ((a.coef[0] * b.coef[0]) + 3* (a.coef[2] * b.coef[1] + a.coef[1] * b.coef[2])) % FIELDPRIME;
	product.coef[1] = ((a.coef[1] * b.coef[0] + a.coef[0] * b.coef[1])+ 3*(a.coef[2] * b.coef[2])) % FIELDPRIME;
	product.coef[2] = (a.coef[2] * b.coef[0] + a.coef[1] * b.coef[1] + a.coef[0] * b.coef[2]) % FIELDPRIME;
	return product;
}
#endif
