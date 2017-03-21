#ifndef UOVTINF2_H
#define UOVTINF2_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "parameters.h"
#include "twister.h"
#include "csprng.h"
#include "LinearAlgebra.h"
#include "buffer.h"

typedef struct {
	unsigned char privateseed[32];
	uint32_t publicseed;
	bitcontainer T[V];
	bitcontainer *Q;
} UOVTINF2_SecretKey;

typedef struct {
	uint32_t seed;
	bitcontainer *B2;
} UOVTINF2_PublicKey;

typedef struct {
	Matrix s;
} UOVTINF2_Signature;

void UOVTINF2_generateKeyPair(UOVTINF2_PublicKey *pk, UOVTINF2_SecretKey *sk);
UOVTINF2_Signature UOVTINF2_signDocument(UOVTINF2_SecretKey sk, const unsigned char* document, uint64_t len);
int UOVTINF2_verify(UOVTINF2_PublicKey* pk, UOVTINF2_Signature* signature, unsigned char* document, uint64_t len);

void UOVTINF2_serialize_SecretKey(writer *Buff, UOVTINF2_SecretKey *sk);
void UOVTINF2_deserialize_SecretKey(reader *Buff, UOVTINF2_SecretKey *sk);
void UOVTINF2_destroy_SecretKey(UOVTINF2_SecretKey *sk);

void UOVTINF2_serialize_PublicKey(writer* Buff, UOVTINF2_PublicKey* pk);
void UOVTINF2_deserialize_PublicKey(reader* Buff, UOVTINF2_PublicKey* pk);
void UOVTINF2_destroy_PublicKey(UOVTINF2_PublicKey *pk);

void UOVTINF2_serialize_signature(writer *W, UOVTINF2_Signature *S);
void UOVTINF2_deserialize_signature(reader *R, UOVTINF2_Signature *S);
void UOVTINF2_destroy_signature(UOVTINF2_Signature *S);

#ifdef UOVTINF2

#define SecretKey UOVTINF2_SecretKey
#define PublicKey UOVTINF2_PublicKey
#define Signature UOVTINF2_Signature

#define signDocument UOVTINF2_signDocument
#define generateKeyPair UOVTINF2_generateKeyPair
#define verify UOVTINF2_verify

#define serialize_SecretKey UOVTINF2_serialize_SecretKey
#define deserialize_SecretKey UOVTINF2_deserialize_SecretKey
#define destroy_SecretKey UOVTINF2_destroy_SecretKey

#define serialize_PublicKey UOVTINF2_serialize_PublicKey
#define deserialize_PublicKey UOVTINF2_deserialize_PublicKey
#define destroy_PublicKey UOVTINF2_destroy_PublicKey

#define serialize_signature UOVTINF2_serialize_signature
#define deserialize_signature UOVTINF2_deserialize_signature
#define destroy_signature UOVTINF2_destroy_signature

#endif 

#endif 
