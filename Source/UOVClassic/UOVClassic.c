#include "UOVClassic.h"

/*
	Write a secret key to a char array

	W  : A writer object
	sk : a secret key
*/
void UOVClassic_serialize_SecretKey(writer *W, UOVClassic_SecretKey *sk) {
	serialize_matrix(W, sk->T);
	serialize_matrix(W, sk->Q);
}

/*
	Read a secret key from a char array

	R : A reader object
	sk : receives the secret key
*/
void UOVClassic_deserialize_SecretKey(reader *R, UOVClassic_SecretKey *sk) {
	sk->T = newMatrix(V, O);
	deserialize_matrix(R, sk->T);
	sk->Q = newMatrix(D, M);
	deserialize_matrix(R,sk->Q);
}

/*
	Frees the memory allocated by a secret key
*/
void UOVClassic_destroy_SecretKey(UOVClassic_SecretKey * sk) {
	destroy(sk->Q);
	destroy(sk->T);
}

/*
	Writes a public key to a char array

	W : A writer object
	pk : the public key
*/
void UOVClassic_serialize_PublicKey(writer* W, UOVClassic_PublicKey* pk) {
	serialize_uint64_t(W, pk->seed, 32);
	serialize_matrix(W, pk->B2);
}

/*
	Reads a public key from a char array

	R : A reader object
	pk : receives the public key
*/
void UOVClassic_deserialize_PublicKey(reader* R, UOVClassic_PublicKey* pk) {
	pk->seed = ((uint32_t) deserialize_uint64_t(R, 32));
	pk->B2 = newMatrix(O*(O + 1) / 2, M);
	deserialize_matrix(R, pk->B2);
}

/*
	Frees the memory allocated by a public key
*/
void UOVClassic_destroy_PublicKey(UOVClassic_PublicKey * pk) {
	destroy(pk->B2);
}

/*
	Writes a signature to a char array

	W : A writer object
	S : the signature
*/
void UOVClassic_serialize_signature(writer* W, UOVClassic_Signature* S) {
	serialize_matrix(W,S->s);
}

/*
	Reads a signature from a char array

	R : A reader object
	S : receives the signature
*/
void UOVClassic_deserialize_signature(reader* R, UOVClassic_Signature* S) {
	S->s = newMatrix(N, 1);
	deserialize_matrix(R, S->s);
}

/*
	Frees the memory allocated by a signature
*/
void UOVClassic_destroy_signature(UOVClassic_Signature *S) {
	destroy(S->s);
}

/*
	Calculates the transpose of the Macaulaymatrix of the private UOV system 
	(only the first (V*V+1)/2 + O*V rows, the rest is zero)

	T : the upper right V-by-O submatrix of T
	seed : a seed used to generate the first part of the public system

	returns : the transpose of the macaulaymatrix of the private UOV system
*/
Matrix calculatePrivateQ(Matrix T, uint32_t seed) {
	int i, j, k, r, s;
	twister MT;
	FELT temp;
	seedMT(&MT, seed);

	Matrix PrivateKey = randomMatrixMT(D, M, &MT);
	int a = -1;
	for (i = 0; i < V; i++) {
		a += V - i;
		for (j = 0; j < O; j++) {
			a++;

			for (r = 0; r <= i; r++) {
				int col = lexindex(r, i);
				if (!isEqual(T.array[r][j], ZERO)) {
					for (k = 0; k < M; k++) {
						temp = minus(multiply(T.array[r][j], PrivateKey.array[col][k]));
						addBtoA(&PrivateKey.array[a][k], &temp);
					}
				}
			}

			for (s = i; s < V; s++) {
				int col = lexindex(i, s);
				if (!isEqual(T.array[s][j], ZERO)) {
					for (k = 0; k < M; k++) {
						temp = minus(multiply(T.array[s][j], PrivateKey.array[col][k]));
						addBtoA(&PrivateKey.array[a][k], &temp);
					}
				}
			}
		}
	}
	return PrivateKey;
}

/*
	Calculates the second part of the public system

	Q : The transpose of the macaulay matrix of the private system
	T : the upper right V-by-O submatrix of T

	returns : the transpose of the last O*(O+1)/2 columns of the macaulay matrix of the public system
*/
Matrix calculateB2(Matrix Q, Matrix T) {
	int k, i, j;
	Matrix B2 = newMatrix(O*(O + 1) / 2, M);

	Matrix Q1 = zeroMatrix(V, V);
	Matrix Q2 = newMatrix(V, O);
	for (k = 0; k < M; k++) {
		int col = -1;
		for (i = 0; i < V; i++) {
			for (j = i; j < V; j++) {
				Q1.array[i][j] = Q.array[++col][k];
			}
			for (j = 0; j < O; j++) {
				Q2.array[i][j] = Q.array[++col][k];
			}
		}


		Matrix QT = multiplyAB(Q1, T);
		addInPlace(QT, Q2);
		Matrix TtQT = multiplyAtB(T, QT);

		col = -1;
		for (i = 0; i < O; i++) {
			for (j = i; j < O; j++) {
				col++;
				if (i == j) {
					B2.array[col][k] = TtQT.array[i][j];
				}
				else {
					B2.array[col][k] = add(TtQT.array[i][j], TtQT.array[j][i]);
				}
			}
		}

		destroy(QT);
		destroy(TtQT);

	}
	destroy(Q1);
	destroy(Q2);
	return B2;
}

/*
	Generates a key pair

	pk : receives the public key
	sk : receives the secret key
*/
void UOVClassic_generateKeyPair(UOVClassic_PublicKey *pk , UOVClassic_SecretKey *sk) {
	int i;
	pk->seed = rand();

	csprng rng;
	csprng_seed_uint64_t(&rng, rand());

	sk->T = randomMatrixrng(V, O, &rng);
	for (i = 0; i < O; i++) {
		sk->T.array[0][i] = ZERO;
	}

	sk->Q = calculatePrivateQ(sk->T, pk->seed);

	pk->B2 = calculateB2(sk->Q , sk->T);
}

/*
	Generates a signature for a document

	sk : the secret key
	document : a char array containing the document to be signed
	len : the length of the document

	returns : A signature for the document
*/
UOVClassic_Signature UOVClassic_signDocument(UOVClassic_SecretKey sk, const unsigned char* document , uint64_t len) {
	int i, j;

	Matrix hash ;
	UOVClassic_Signature signature;
	csprng rng, rng2;
	csprng_init(&rng);
	csprng_init(&rng2);
	csprng_seed(&rng, len, document);

	unsigned char randchar = ((unsigned char) rand());
	csprng_seed(&rng2, len, document);
	csprng_seed(&rng2, 1, &randchar);

	hash = randomMatrixrng(M, 1, &rng);

	signature.s = findSolutionOfUOVSystem(sk.Q, hash, &rng2);

	for (i = 0; i < V; i++) {
		for (j = 0; j < O; j++) {
			signature.s.array[i][0] = add(signature.s.array[i][0], minus(multiply(signature.s.array[V + j][0], sk.T.array[i][j])));
		}
	}
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
int UOVClassic_verify(UOVClassic_PublicKey *pk, UOVClassic_Signature *signature, unsigned char* document , uint64_t len) {
	int i, j,k;

	Matrix hash;
	csprng rng;

	twister MT;
	seedMT(&MT, pk->seed);

	csprng_init(&rng);
	csprng_seed(&rng, len, document);

	hash = randomMatrixrng(M, 1, &rng);
	
	Matrix evaluation = zeroMatrix(M, 1);
	for (i = 0; i < V; i++) {
		for (j = i; j < N; j++) {
			FELT prod = multiply(signature->s.array[i][0], signature->s.array[j][0]);
			for (k = 0; k < M; k++) {
				evaluation.array[k][0] = add(evaluation.array[k][0], multiply(prod, randomMTFELT(&MT)));
			}
		}
	}

	int col = 0;
	for (i = V; i < N; i++) {
		for (j = i; j < N; j++) {
			FELT prod = multiply(signature->s.array[i][0], signature->s.array[j][0]);
			for (k = 0; k < M; k++) {
				evaluation.array[k][0] = add(evaluation.array[k][0], multiply(prod, pk->B2.array[col][k]));
			}
			col++;
		}
	}
	int eq = equals(evaluation, hash);
	destroy(evaluation);
	destroy(hash);
	return eq - 1;
}
