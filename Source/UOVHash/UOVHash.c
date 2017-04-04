#include "UOVHash.h"

/*
	Writes the seeds for the secret key and the merkle tree to a char array

	W  : A writer object
	sk : a secret key
*/
void UOVHash_serialize_SecretKey(writer *W, UOVHash_SecretKey *sk) {
	reader R = newReader(sk->seed);
	transcribe(W, &R, KAPPA);
	serialize_uint64_t(W, sk->publicseed, 32);
	serialize_merkleTree(&sk->Tree , W);
}

/*
	Reads the seeds for the secret key and the merkle tree from a char array, and recomputes the rest of the secret key

	R : A reader object
	sk : receives the secret key
*/
void UOVHash_deserialize_SecretKey(reader *R, UOVHash_SecretKey *sk) {
	int i;
	writer W = newWriter(sk->seed);
	transcribe(&W, R, KAPPA);
	sk->publicseed = ((uint32_t) deserialize_uint64_t(R, 32));
	csprng rng;
	csprng_init(&rng);
	csprng_seed(&rng, KAPPA, sk->seed);

	sk->T = randomMatrixrng(V, O, &rng);
	for (i = 0; i < O; i++) {
		sk->T.array[0][i] = ZERO;
	}
	sk->Q = calculatePrivateQ(sk->T, sk->publicseed);
	sk->B2 = calculateB2(sk->Q, sk->T);
	sk->Tree = deserialize_merkleTree(TAU, TAU / 2, R);
}

/*
	Frees the memory allocated by a secret key
*/
void UOVHash_destroy_SecretKey(UOVHash_SecretKey *sk) {
	destroy(sk->B2);
	destroy(sk->Q);
	destroy(sk->T);
	destroy_MerkleTree(&sk->Tree);
}

/*
	Writes a public key to a char array

	W : A writer object
	pk : the public key
*/
void UOVHash_serialize_PublicKey(writer* W, UOVHash_PublicKey* pk) {
	serialize_uint64_t(W, pk->seed, 32);
	reader R = newReader(pk->merkleRoot);
	transcribe(W, &R, KAPPA);
}

/*
	Reads a public key from a char array

	R : A reader object
	pk : receives the public key
*/
void UOVHash_deserialize_PublicKey(reader* R, UOVHash_PublicKey* pk) {
	pk->seed = ((uint32_t) deserialize_uint64_t(R, 32));
	writer W = newWriter(pk->merkleRoot);
	transcribe(&W, R, KAPPA);
}

/*
	Writes a signature to a char array

	W : A writer object
	S : the signature
*/
void UOVHash_serialize_signature(writer* W, UOVHash_Signature* S) {
	int i;
	reader R = newReader(S->merklePaths);
	for (i = 0; i < SIGMA; i++) {
		serialize_matrix(W, S->signatures[i]);
	}
	serialize_matrix(W, S->TP);
	transcribe(W, &R , MERKLE_PATHS_SIZE);
}

/*
	Reads a signature from a char array

	R : A reader object
	S : receives the signature
*/
void UOVHash_deserialize_signature(reader* R, UOVHash_Signature* S) {
	int i;
	writer W;
	for (i = 0; i < SIGMA; i++) {
		S->signatures[i] = newMatrix(N, 1);
		deserialize_matrix(R, S->signatures[i]);
	}
	S->TP = newMatrix(D2, ALPHA);
	deserialize_matrix(R, S->TP);

	S->merklePaths = malloc(MERKLE_PATHS_SIZE);
	W = newWriter(S->merklePaths);
	transcribe(&W, R, MERKLE_PATHS_SIZE);
}

/*
	Frees the memory allocated by a signature
*/
void UOVHash_destroy_signature(UOVHash_Signature *S) {
	int i;
	for (i = 0; i < SIGMA; i++) {
		destroy(S->signatures[i]);
	}
	destroy(S->TP);
	free(S->merklePaths);
}

/*
	Generates a key pair

	pk : receives the public key
	sk : receives the secret key
*/
void UOVHash_generateKeyPair(UOVHash_PublicKey *pk, UOVHash_SecretKey *sk) {
	int i;
	pk->seed = rand();
	sk->publicseed = pk->seed;

	for (i = 0; i < KAPPA; i++) {
		sk->seed[i] = ((unsigned char) rand());
	}

	csprng rng;
	csprng_init(&rng);
	csprng_seed(&rng, KAPPA, sk->seed);

	sk->T = randomMatrixrng(V, O, &rng);
	for (i = 0; i < O; i++) {
		sk->T.array[0][i] = ZERO;
	}

	sk->Q = calculatePrivateQ(sk->T, pk->seed);
	sk->B2 = calculateB2(sk->Q , sk->T);
	sk->Tree = newMerkleTree(TAU, TAU / 2, sk);
	writer W = newWriter(pk->merkleRoot);
	getRoot(&sk->Tree, sk, &W);
}

/*
	Generates a signature for a document

	sk : the secret key
	document : a char array containing the document to be signed
	len : the length of the document

	returns : A signature for the document
*/
UOVHash_Signature UOVHash_signDocument(UOVHash_SecretKey sk, const unsigned char* document, uint64_t len) {
	int i, j;
	int s;
	FELT temp;
	Matrix hash,T;
	UOVHash_Signature sig;
	csprng rng;
	writer W;

	for (s = 0; s < SIGMA; s++) {
		/* calculate hash of document */
		csprng_seed_uint64_t(&rng, s);
		csprng_seed(&rng, len, document);
		hash = randomMatrixrng(M, 1, &rng);

		/* find solution of private system */
		csprng_init(&rng);
		csprng_seed(&rng, len, document);
		csprng_seed(&rng, KAPPA, sk.seed);
		sig.signatures[s] = findSolutionOfUOVSystem(sk.Q, hash, &rng);

		/* convert to solution of public system */
		for (i = 0; i < V; i++) {
			for (j = 0; j < O; j++) {
				temp = minus(multiply(sig.signatures[s].array[V + j][0], sk.T.array[i][j]));
				addBtoA(&sig.signatures[s].array[i][0], &temp);
			}
		}
	}

	/* calculate TP */
	csprng_init(&rng);
	csprng_seed(&rng, len, document);
	for (s = 0; s < SIGMA; s++) {
		csprng_seed_matrix(&rng, sig.signatures[s]);
	}
	T = randomMatrixrng(M, ALPHA, &rng);
	sig.TP = multiplyAB(sk.B2,T);

	/* Include merkle paths in signature */
	csprng_init(&rng);
	csprng_seed(&rng, len, document);
	for (s = 0; s < SIGMA; s++) {
		csprng_seed_matrix(&rng, sig.signatures[s]);
	}
	csprng_seed_matrix(&rng, sig.TP);

	unsigned char challenges[THETA * 4];
	W = newWriter(challenges);
	csprng_generate(&rng, THETA * 4, &W);

	sig.merklePaths = malloc(MERKLE_PATHS_SIZE);
	reader R = newReader(challenges);
	W = newWriter(sig.merklePaths);
	for (i = 0; i < THETA; i++) {
		uint16_t c = ((uint16_t) deserialize_uint64_t(&R, TAU));
		OpenMerkleTreePath(&sk.Tree, &sk, c, &W);
	}

	return sig;
}

/*
	Verifies a signature for a document

	pk : the public key
	signature : a signature
	document : a char array containing a document
	len : the length of the document

	returns : 0 if the signature is valid, -1 otherwise
*/
int UOVHash_verify(UOVHash_PublicKey* pk, UOVHash_Signature* signature, unsigned char* document, uint64_t len) {
	int i, j, k, l , s;
	int valid = 0;
	FELT temp;
	writer W;

	Matrix hash , T , Thash;
	csprng rng;
	twister MT;

	/* calculate T */
	csprng_init(&rng);
	csprng_seed(&rng, len, document);
	for (s = 0; s < SIGMA; s++) {
		csprng_seed_matrix(&rng, signature->signatures[s]);
	}
	T = randomMatrixrng(M, ALPHA, &rng);

	for (s = 0; s < SIGMA; s++) {
		seedMT(&MT, pk->seed);

		/* calculate hash of document */
		csprng_seed_uint64_t(&rng, s);
		csprng_seed(&rng, len, document);
		hash = randomMatrixrng(M, 1, &rng);

		/* subtract the evaluation of the first part of the public system at the signature from the hash */
		for (i = 0; i < V; i++) {
			for (j = i; j < N; j++) {
				FELT prod = multiply(signature->signatures[s].array[i][0], signature->signatures[s].array[j][0]);
				for (k = 0; k < M; k++) {
					temp = minus(multiply(prod, randomMTFELT(&MT)));
					addBtoA(&hash.array[k][0], &temp);
				}
			}
		}

		/* calculate the evaluation of TP at the signature */
		Matrix evaluation = zeroMatrix(ALPHA, 1);
		int col = 0;
		for (i = V; i < N; i++) {
			for (j = i; j < N; j++) {
				FELT prod = multiply(signature->signatures[s].array[i][0], signature->signatures[s].array[j][0]);
				for (k = 0; k < ALPHA; k++) {
					temp = multiply(prod, signature->TP.array[col][k]);
					addBtoA(&evaluation.array[k][0], &temp);
				}
				col++;
			}
		}
	
		/* verify if TP(sign) = T*hash */
		Thash = multiplyAtB(T, hash);
		int eq = equals(evaluation, Thash);

		if (eq != 1) {
			printf("signature was invalid : signature #%d is an invalid UOV signature \n",s);
			valid = -1;
		}
		destroy(evaluation);
		destroy(hash);
		destroy(Thash);
	}

	if (valid == 0) {

		/* calculate indices of leaves to check */
		csprng_init(&rng);
		csprng_seed(&rng, len, document);
		for (s = 0; s < SIGMA; s++) {
			csprng_seed_matrix(&rng, signature->signatures[s]);
		}
		csprng_seed_matrix(&rng, signature->TP);
		unsigned char challenges[THETA * 4];
		W = newWriter(challenges);
		csprng_generate(&rng, THETA * 4, &W);
		reader ChallengeReader = newReader(challenges);
		reader R = newReader(signature->merklePaths);
		reader R2;
		MACFELT check[M];
		MACFELT Tcheck[ALPHA];
		MACFELT MACevaluation[ALPHA];

		/* checking if leaves are valid */
		for (i = 0; i < THETA; i++) {
			if (valid != 0) {
				break;
			}

			uint64_t c = deserialize_uint64_t(&ChallengeReader, TAU);

			/* check whether the mac value is valid */
			for (j = 0; j < ALPHA; j++) {
				Tcheck[j] = MACZERO; /* Tcheck = 0 */
				MACevaluation[j] = MACZERO;
			}

			R2.data = R.data;
			R2.next = R.next;
			R2.bitsUsed = R.bitsUsed;
			for (j = 0; j < M; j++) {
				check[j] = deserialize_MACFELT(&R2); /* read mac value */
				for (k = 0; k < ALPHA; k++) {
					Tcheck[k] = MACadd(Tcheck[k], MACscalarMultiply(T.array[j][k], check[j])); /* calculate T * check */
				}
			}

			MACFELT x = intToMACFELT(c);
			MACFELT MACtemp;

			for (j = 0 ; j < D2; j+= K) {
				for (k = 0; k < ALPHA; k++) {
					for (l = 0; l < K; l++) {
						if (j + l < D2) {
							setCoef(MACtemp, l, signature->TP.array[j+l][k]);
						}
						else
						{
							setCoef(MACtemp, l, ZERO);
						}
					}
					MACevaluation[k] = MACadd(MACmultiply(MACevaluation[k], x), MACtemp);
				}
			}

			for (j = 0; j < ALPHA; j++){
				if (MACisEqual(MACevaluation[j], Tcheck[j]) == 0) {
					printf("signature was invalid : authenticity check of TP failed\n");
					valid = -1;
				}
			}

			/* check whether the merkle path is valid */
			if (VerifyMerkleTreePath(c, pk->merkleRoot , &R) == 0) {
				printf("signature was invalid : verification of merkle path failed\n");
				valid = -1;
			}
		}
	}

	destroy(T);

	return valid;
}