#include "LUOV.h"

void LUOV_calculatePrivateQ(LUOV_SecretKey *sk, uint32_t seed);
void randomizeArrayKeccak(bitcontainer *arr, int size, unsigned char *seed);
void randomizeArrayMT(bitcontainer *arr, int size, uint32_t seed);

/*
	Writes the seeds for the secret key to a char array

	W  : A writer object
	sk : a secret key
*/
void LUOV_serialize_SecretKey(writer *W, LUOV_SecretKey *sk) {
	reader R = newReader(sk->privateseed);
	transcribe(W, &R, 32);
	serialize_uint64_t(W, sk->publicseed, 32);
}

/*
	Reads the seeds for a secret key from a char array and recomputest the secret key

	R : A reader object
	sk : receives the secret key
*/
void LUOV_deserialize_SecretKey(reader *R, LUOV_SecretKey *sk) {
	writer W = newWriter(sk->privateseed);
	transcribe(&W, R, 32);
	sk->publicseed = ((uint32_t) deserialize_uint64_t(R, 32));

	sk->Q = malloc(D * sizeof(bitcontainer));

	randomizeArrayKeccak(sk->T, V, sk->privateseed);
	sk->T[0] = empty;

	LUOV_calculatePrivateQ(sk, sk->publicseed);
}

/*
	Frees the memory allocated by a secret key
*/
void LUOV_destroy_SecretKey(LUOV_SecretKey *sk) {
	free(sk->Q);
}

/*
	Writes a public key to a char array

	W : A writer object
	pk : the public key
*/
void LUOV_serialize_PublicKey(writer* W, LUOV_PublicKey* pk) {
	int i;
	serialize_uint64_t(W, pk->seed, 32);
	for (i = 0; i < D2; i++) {
		serialize_bitcontainer(W, pk->B2[i]);
	}
	return;
}

/*
	Reads a public key from a char array

	R : A reader object
	pk : receives the public key
*/
void LUOV_deserialize_PublicKey(reader* R, LUOV_PublicKey* pk) {
	int i;
	pk->seed = ((uint32_t) deserialize_uint64_t(R, 32));
	pk->B2 = malloc(sizeof(bitcontainer)*D2);
	for (i = 0; i < D2; i++) {
		pk->B2[i] = deserialize_bitcontainer(R);
	}
	return;
}

/*
	Frees the memory allocated by a public key
*/
void LUOV_destroy_PublicKey(LUOV_PublicKey *pk) {
	free(pk->B2);
}

/*
	Writes a signature to a char array

	W : A writer object
	S : the signature
*/
void LUOV_serialize_signature(writer* W, LUOV_Signature* S) {
	serialize_matrix(W, S->s);
}

/*
	Reads a signature from a char array

	R : A reader object
	S : receives the signature
*/
void LUOV_deserialize_signature(reader* R, LUOV_Signature* S) {
	S->s = newMatrix(N, 1);
	deserialize_matrix(R, S->s);
}

/*
	Frees the memory allocated by a signature
*/
void LUOV_destroy_signature(LUOV_Signature *S) {
	destroy(S->s);
}

/*
	Calculates the transpose of the Macaulaymatrix of the private UOV system
	(only the first (V*V+1)/2 + O*V rows, the rest is zero)

	sk : the secret key
	seed : a seed used to generate the first part of the public system
*/
void LUOV_calculatePrivateQ(LUOV_SecretKey *sk, uint32_t seed) {
	int i, j, s, r,col;
	int a = -1;
	randomizeArrayMT(sk->Q, D, seed);
	for (i = 0; i < V; i++) {
		a += V - i;
		for (j = 0; j < O; j++) {
			a++;

			for (r = 0; r <= i; r++) {
				col = lexindex(r, i);
				if (getBit(sk->T[r], j)) {
					sk->Q[a] = xor(sk->Q[a], sk->Q[col]);
				}
			}

			for (s = i; s < V; s++) {
				col = lexindex(i, s);
				if (getBit(sk->T[s], j)) {
					sk->Q[a] = xor(sk->Q[a], sk->Q[col]);
				}
			}
		}
	}
}

/*
	Randomizes an array of bitcontainers with keccak

	arr : the array of bitcontainers to randomize
	size : the size of the array
	seed : a seed consisting of 32 chars for the keccak pseudorandom number generator
*/
void randomizeArrayKeccak(bitcontainer *arr, int size, unsigned char *seed) {
	int i;
	csprng rng;
	csprng_init(&rng);
	csprng_seed(&rng, 32, seed);
	for (i = 0; i < size; i++) {
		arr[i] = randomBitcontainerKeccak(&rng);
	}
}

/*
	Randomizes an array of bitcontainers with a mersenne twister

	arr : the array of bitcontainers to randomize
	size : the size of the array
	seed : a uint32_t for seeding the mersenne twister
*/
void randomizeArrayMT(bitcontainer *arr, int size , uint32_t seed) {
	int i;
	twister MT;
	seedMT(&MT, seed);
	for (i = 0; i < size; i++) {
		arr[i] = randomBitcontainerMT(&MT);
	}
}

/*
	Calculates the transpose of the last O*(O+1)/2 columns of the macaulay matrix of the public system

	sk : the secret key
	pk : the public key
*/
void LUOV_calculateB2(LUOV_SecretKey *sk , LUOV_PublicKey *pk) {
	int i, j,k;
	int col = 0;
	bitcontainer **Q2 = malloc(sizeof(bitcontainer*) * V);
	bitcontainer **B2 = malloc(sizeof(bitcontainer*) * V);

	for (i = 0; i < V; i++) {
		Q2[i] = malloc(sizeof(bitcontainer)*O);
		B2[i] = malloc(sizeof(bitcontainer)*O);
		for (j = 0; j < O; j++) {
			Q2[i][j] = empty;
			B2[i][j] = empty;
		}
	}

	/*Calculate Q1*T + Q2 with Q1 the V*V part of Q and Q2 the V*O part*/
	col = 0;
	for (i = 0; i < V; i++) {
		for (j = i; j < V; j++) {
			for (k = 0; k < M; k++) {
				if (getBit(sk->T[j], k)) {
					Q2[i][k] = xor(Q2[i][k], sk->Q[col]);
				}
			}
			col++;
		}
		for (j = 0; j < O; j++) {
			Q2[i][j] = xor(Q2[i][j], sk->Q[col++]);
		}
	}

	/*Calculate Transpose(T)*(Q1*T + Q_2)*/
	for (i = 0; i < O; i++) {
		for (j = 0; j < O; j++) {
			for (k = 0; k < V; k++) {
				if (getBit(sk->T[k] , i ) ) {
					B2[i][j] = xor(B2[i][j], Q2[k][j]);
				}
			}
		}
	}

	/*Put result into public key*/
	col = 0;
	for (i = 0; i < O; i++) {
		for (j = i; j < O; j++) {
			if (i == j) {
				pk->B2[col] = B2[i][j];
			}
			else {
				pk->B2[col] = xor(B2[i][j] , B2[j][i]);
			}
			col++;
		}
	}

	for (i = 0; i < V; i++) {
		free(Q2[i]);
		free(B2[i]);
	}
	free(Q2);
	free(B2);
}

/*
	Generates a key pair

	pk : receives the public key
	sk : receives the secret key
*/
void LUOV_generateKeyPair(LUOV_PublicKey *pk, LUOV_SecretKey *sk) {
	int i;
	for (i = 0; i < 32; i++) {
		sk->privateseed[i] = ((unsigned char) rand());
	}
	
	sk->Q = malloc(D * sizeof(bitcontainer));
	pk->B2 = malloc(D2 * sizeof(bitcontainer));

	pk->seed = rand();
	sk->publicseed = pk->seed;
	randomizeArrayKeccak(sk->T, V, sk->privateseed);
	sk->T[0]=empty; /* makes T linear in stead of affine*/

	LUOV_calculatePrivateQ(sk, pk->seed);
	LUOV_calculateB2( sk , pk);
}

/*
	Builds the augmented matrix for the linear system resulting from fixing the vinegar variables in a UOV system

	A : receives the augmented matrix
	signature : A N-by-1 matrix whose first V entries are the vinegar variables
	target : The target for the UOV system
	system : The transpose of the first V*(V+1)/2 + V*O columns of the macaulay matrix for the UOV system (the last O*(O+1)/2 columns are zero anyway )
*/
void BuildAugmentedMatrixLUOV(Matrix A, Matrix signature, Matrix target, bitcontainer *system) {
	int i, j, k , col;
	for (k = 0; k < M; k++) {
		A.array[k][O] = target.array[k][0];
	}

	col = 0;
	for (i = 0; i < V; i++) {
		for (j = i; j < V; j++) {
			FELT prod = multiply(signature.array[i][0], signature.array[j][0]);
			for (k = 0; k < M; k++) {
				if (getBit(system[col] , k) ) {
					addBtoA(&A.array[k][O], &prod);
				}
			}
			col++;
		}

		for (j = 0; j < O; j++) {
			for (k = 0; k < M; k++) {
				if (getBit(system[col], k)) {
					addBtoA(&A.array[k][j], &signature.array[i][0]);
				}
			}
			col++;
		}
	}
}

/*
	Finds a solution to a UOVSystem

	sk : a secret key
	target : the target for the UOV system

	returns : A N-by-1 matrix containing a solution 
*/
Matrix solveUOVSystem(LUOV_SecretKey sk, Matrix hash , csprng *rng) {
	int i;
	Matrix signature;
	Matrix A;
	Matrix x;
	x.cols = 0;

	while (x.cols == 0) {
		signature = randomMatrixrng(N, 1, rng);
		signature.array[0][0] = ONE;

		A = zeroMatrix(M, O + 1);

		BuildAugmentedMatrixLUOV(A, signature, hash, sk.Q);
		x = getSolution(A);

		destroy(A);
		if (x.cols == 0) {
			destroy(signature);
		}
	}
	for (i = 0; i < O; i++) {
		signature.array[i + signature.rows - O][0] = x.array[i][0];
	}
	destroy(x);

	return signature;
}

/*
	Generates a signature for a document

	sk : the secret key
	document : a char array containing the document to be signed
	len : the length of the document

	returns : A signature for the document
*/
LUOV_Signature LUOV_signDocument(LUOV_SecretKey sk,const unsigned char *document , uint64_t len) {
	int i, j;
	FELT temp;
	Matrix hash;
	LUOV_Signature signature;
	csprng rng , rng2;
	csprng_init(&rng);
	csprng_init(&rng2);
	csprng_seed(&rng, len, document);

	csprng_seed(&rng2, len, document);
	csprng_seed(&rng2, 32 , sk.privateseed );
	
	hash = randomMatrixrng(M, 1, &rng);
	signature.s = solveUOVSystem(sk, hash, &rng2);

	for (i = 0; i < V; i++) {
		for (j = 0; j < O; j++) {
			if ( getBit(sk.T[i] , j )) {
				temp = minus(signature.s.array[V + j][0]);
				addBtoA(&signature.s.array[i][0], &temp);
			}
		}
	}
	destroy(hash);

	return signature;
}

/*
	Verifies a signature for a document

	pk : the public key
	signature : a signature
	document : a char array containing a document
	len : the length of the document

	returns : 0 if the signature is valid, -1 otherwise
*/
int LUOV_verify(LUOV_PublicKey *pk, LUOV_Signature *signature, unsigned char *document , uint64_t len) {
	int i, k, j , eq , col;
	Matrix hash , evaluation;
	twister MT;
	FELT prod;
	bitcontainer r;
	csprng rng;
	csprng_init(&rng);
	csprng_seed(&rng, len, document);

	hash = randomMatrixrng(M, 1, &rng);
	
	seedMT(&MT, pk->seed);
	evaluation = zeroMatrix(M, 1);

	for (i = 0; i < V; i++) {
		for (j = i; j < N; j++) {
			r = randomBitcontainerMT(&MT);

			prod = multiply(signature->s.array[i][0], signature->s.array[j][0]);
			for (k = 0; k < M; k++) {
				if (getBit(r,k)) {
					addBtoA(&evaluation.array[k][0], &prod);
				}
			}
		}
	}
	col = 0;
	for (i = V; i < N; i++) {
		for (j = i; j < N; j++) {
			prod = multiply(signature->s.array[i][0], signature->s.array[j][0]);
			for (k = 0; k < M; k++) {
				if ( getBit(pk->B2[col] , k) ) {
					addBtoA(&evaluation.array[k][0], &prod);
				}
			}
			col++;
		}
	}
	eq = equals(evaluation, hash);
	destroy(evaluation);
	destroy(hash);
	return eq-1;
}
