/*
	Implementation of UOV, with Petzold's method to reduce public key size
*/

#ifndef UOVCLASSIC_H
#define UOVCLASSIC_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>
#include <string.h>
#include "parameters.h"
#include "LinearAlgebra.h"
#include "buffer.h"

/* Secret key */
typedef struct {
	Matrix T;
	Matrix Q;
} UOVClassic_SecretKey;

/* Public key */
typedef struct {
	uint32_t seed;
	Matrix B2;
} UOVClassic_PublicKey;

/* Signature */
typedef struct {
	Matrix s;
} UOVClassic_Signature;

Matrix calculateB2(Matrix Q, Matrix T);
Matrix calculatePrivateQ(Matrix T, uint32_t seed);

void UOVClassic_generateKeyPair(UOVClassic_PublicKey *pk , UOVClassic_SecretKey *sk);
UOVClassic_Signature UOVClassic_signDocument(UOVClassic_SecretKey sk, const unsigned char* document, uint64_t len);
int UOVClassic_verify(UOVClassic_PublicKey* pk, UOVClassic_Signature *signature, unsigned char* document, uint64_t len);

void UOVClassic_serialize_SecretKey(writer *Buff, UOVClassic_SecretKey *sk);
void UOVClassic_deserialize_SecretKey(reader *Buff, UOVClassic_SecretKey *sk);
void UOVClassic_destroy_SecretKey(UOVClassic_SecretKey*sk);

void UOVClassic_serialize_PublicKey(writer* Buff, UOVClassic_PublicKey* pk);
void UOVClassic_deserialize_PublicKey(reader* Buff, UOVClassic_PublicKey* pk);
void UOVClassic_destroy_PublicKey(UOVClassic_PublicKey *kp);

void UOVClassic_serialize_signature(writer* W, UOVClassic_Signature* S);
void UOVClassic_deserialize_signature(reader* R, UOVClassic_Signature* S);
void UOVClassic_destroy_signature(UOVClassic_Signature *S);

#ifdef UOVCLASSIC

#define SecretKey UOVClassic_SecretKey
#define PublicKey UOVClassic_PublicKey
#define Signature UOVClassic_Signature
#define KeyPair   UOVClassic_KeyPair

#define signDocument UOVClassic_signDocument
#define generateKeyPair UOVClassic_generateKeyPair
#define verify UOVClassic_verify

#define serialize_SecretKey UOVClassic_serialize_SecretKey
#define deserialize_SecretKey UOVClassic_deserialize_SecretKey
#define destroy_SecretKey UOVClassic_destroy_SecretKey

#define serialize_PublicKey UOVClassic_serialize_PublicKey
#define deserialize_PublicKey UOVClassic_deserialize_PublicKey
#define destroy_PublicKey UOVClassic_destroy_PublicKey

#define serialize_signature UOVClassic_serialize_signature
#define deserialize_signature UOVClassic_deserialize_signature
#define destroy_signature UOVClassic_destroy_signature

#endif

#endif 