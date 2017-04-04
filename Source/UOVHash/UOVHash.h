#ifndef UOVHASH_H
#define UOVHASH_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "parameters.h"
#include "LinearAlgebra.h"
#include "buffer.h"
#include "MerkleTree.h"
#include "UOVClassic.h"
#include "MACField.h"

typedef struct UOVHash_SecretKey {
	unsigned char seed[KAPPA];
	uint32_t publicseed;
	Matrix T;
	Matrix Q;
	Matrix B2;
	MerkleTree Tree;
} UOVHash_SecretKey;

typedef struct UOVHash_PublicKey {
	uint32_t seed;
	unsigned char merkleRoot[KAPPA];
} UOVHash_PublicKey;

typedef struct UOVHash_Signature {
	Matrix signatures[SIGMA];
	Matrix TP;
	unsigned char* merklePaths;
} UOVHash_Signature;

void UOVHash_generateKeyPair(UOVHash_PublicKey *pk , UOVHash_SecretKey *sk);
UOVHash_Signature UOVHash_signDocument(UOVHash_SecretKey sk, const unsigned char* document, uint64_t len);
int UOVHash_verify(UOVHash_PublicKey* pk, UOVHash_Signature* signature, unsigned char* document, uint64_t len);

void UOVHash_serialize_SecretKey(writer *Buff, UOVHash_SecretKey *sk);
void UOVHash_deserialize_SecretKey(reader *Buff, UOVHash_SecretKey *sk);
void UOVHash_destroy_SecretKey(UOVHash_SecretKey *sk);

void UOVHash_serialize_PublicKey(writer* Buff, UOVHash_PublicKey* pk);
void UOVHash_deserialize_PublicKey(reader* Buff, UOVHash_PublicKey* pk);

void UOVHash_serialize_signature(writer *W, UOVHash_Signature *S);
void UOVHash_deserialize_signature(reader *R, UOVHash_Signature *S);
void UOVHash_destroy_signature(UOVHash_Signature *S);

#ifdef UOVHASH

#define SecretKey UOVHash_SecretKey
#define PublicKey UOVHash_PublicKey
#define Signature UOVHash_Signature

#define signDocument UOVHash_signDocument
#define generateKeyPair UOVHash_generateKeyPair
#define verify UOVHash_verify

#define serialize_SecretKey UOVHash_serialize_SecretKey
#define deserialize_SecretKey UOVHash_deserialize_SecretKey
#define destroy_SecretKey UOVHash_destroy_SecretKey

#define serialize_PublicKey UOVHash_serialize_PublicKey
#define deserialize_PublicKey UOVHash_deserialize_PublicKey
#define destroy_PublicKey(x)

#define serialize_signature UOVHash_serialize_signature
#define deserialize_signature UOVHash_deserialize_signature
#define destroy_signature UOVHash_destroy_signature

#endif

#endif 
