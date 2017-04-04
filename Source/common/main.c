#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "parameters.h"

#include "F16Field.h"
#include "F32Field.h"
#include "F48Field.h"
#include "F64Field.h"
#include "F80Field.h"

#include "UOVHash.h"
#include "UOVClassic.h"
#include "UOVTinF2.h"

#include "api.h"

/*
	Generates a new keypair

	pk : char array that receives the new public key
	sk : char array that receives the new secret key
*/
int crypto_sign_wbtest_ref_keypair(unsigned char *pk, unsigned char *sk) {
	writer W = newWriter(sk);
	PublicKey PK;
	SecretKey SK;

	generateKeyPair(&PK , &SK);

	serialize_SecretKey(&W, &SK);

	W = newWriter(pk);
	serialize_PublicKey(&W, &PK);

	destroy_SecretKey(&SK);
	destroy_PublicKey(&PK);

	return 0;
}

/*
	Signs a document

	sm : char array that receives the signed message
	smlen : receives the length of the signed message
	m  : char array that contains the original message
	mlen : length of original message
	sk : char array containing the secret key
*/
int crypto_sign_wbtest_ref(unsigned char *sm, unsigned long long *smlen, const unsigned char *m, unsigned long long mlen, const unsigned char *sk)
{
	int i;
	SecretKey skey;
	reader R = newReader(sk);
	writer W = newWriter(sm);
	W.next = mlen;
	Signature signature;


	deserialize_SecretKey(&R, &skey);

	for (i = 0; i<mlen; i++) {
		sm[i] = m[i];
	}

	signature = signDocument(skey, m, mlen);

	serialize_signature(&W, &signature);
	serialize_uint64_t(&W, 0, (8 - W.bitsUsed) % 8);
	*smlen = W.next;

	destroy_signature(&signature);
	destroy_SecretKey(&skey);

	return 0;
}

/*
	Verify a signature

	m :  char array that receives the original message
	mlen : receives the length of the original message
	sm : char array that contains the signed message
	smlen : the length of the signed message
	pk : char array containing the public key

	returns : 0 if the signature is accepted, -1 otherwise
*/
int crypto_sign_wbtest_ref_open(unsigned char *m, unsigned long long *mlen, const unsigned char *sm, unsigned long long smlen, const unsigned char *pk)
{
	int i, valid;
	reader R = newReader(pk);
	PublicKey pkey;
	Signature signature;

	deserialize_PublicKey(&R, &pkey);

	for (i = 0; i<smlen - CRYPTO_BYTES; i++) {
		m[i] = sm[i];
	}
	*mlen = smlen - CRYPTO_BYTES;

	R = newReader(sm);
	R.next = *mlen;

	deserialize_signature(&R, &signature);

	valid = verify(&pkey, &signature, m, *mlen);

	destroy_signature(&signature);
	destroy_PublicKey(&pkey);

	return valid;
}

/*
	Times the execution of the keypair generation, signature generation and signature verification algorithms
*/
int main(void)
{
	int i, j, k;
	unsigned long long smlen;
	unsigned char m[59];
	unsigned char *pk = malloc(sizeof(unsigned char[CRYPTO_PUBLICKEYBYTES]));
	unsigned char *sk = malloc(sizeof(unsigned char[CRYPTO_SECRETKEYBYTES]));
	unsigned char *sm = malloc(sizeof(unsigned char[59 + CRYPTO_BYTES]));
	clock_t cl;

	printf("Public Key takes %.2f kB\n", CRYPTO_PUBLICKEYBYTES / 1024.0);
	printf("Secret Key takes %.2f kB\n", CRYPTO_SECRETKEYBYTES / 1024.0);
	printf("Signature takes %.2f kB\n\n", CRYPTO_BYTES / 1024.0);

	srand((unsigned int) time(NULL));

	float genTime = 0.0;
	float signTime = 0.0;
	float verifyTime = 0.0;

	for (i = 0; i < NUMBER_OF_KEYPAIRS ; i++) {

		cl = clock();
		crypto_sign_wbtest_ref_keypair(pk, sk);
		cl = clock() - cl;

		genTime += ((float) cl)/CLOCKS_PER_SEC;

		for (j = 0; j < SIGNATURES_PER_KEYPAIR ; j++) {
			for (k = 0; k < 59; k++) {
				m[k] = ((unsigned char) rand());
			}

			cl = clock();
			crypto_sign_wbtest_ref(sm, &smlen, m, (unsigned long long) 59, sk);
			cl = clock() - cl;
			signTime += ((float)cl) / CLOCKS_PER_SEC;

			cl = clock();
			if (crypto_sign_wbtest_ref_open(m, &smlen, sm, smlen, pk) != 0) {
				printf("Verification of signature Failed!\n");
			}
			cl = clock() - cl;
			verifyTime += ((float)cl) / CLOCKS_PER_SEC;
		}

	}

	printf("Key pair generation took %.3f seconds.\n", genTime / NUMBER_OF_KEYPAIRS);
	printf("Signing took %.3f seconds.\n", (signTime/NUMBER_OF_KEYPAIRS)/SIGNATURES_PER_KEYPAIR );
	printf("Verifying took %.3f seconds.\n\n", (verifyTime / NUMBER_OF_KEYPAIRS) / SIGNATURES_PER_KEYPAIR );

	free(pk);
	free(sk);
	free(sm);

	system("pause");

	return 0;
}