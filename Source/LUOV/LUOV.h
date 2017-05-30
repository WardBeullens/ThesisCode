#ifndef LUOV_H
#define LUOV_H

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
} LUOV_SecretKey;

typedef struct {
	uint32_t seed;
	bitcontainer *B2;
} LUOV_PublicKey;

typedef struct {
	Matrix s;
} LUOV_Signature;

void LUOV_generateKeyPair(LUOV_PublicKey *pk, LUOV_SecretKey *sk);
LUOV_Signature LUOV_signDocument(LUOV_SecretKey sk, const unsigned char* document, uint64_t len);
int LUOV_verify(LUOV_PublicKey* pk, LUOV_Signature* signature, unsigned char* document, uint64_t len);

void LUOV_serialize_SecretKey(writer *Buff, LUOV_SecretKey *sk);
void LUOV_deserialize_SecretKey(reader *Buff, LUOV_SecretKey *sk);
void LUOV_destroy_SecretKey(LUOV_SecretKey *sk);

void LUOV_serialize_PublicKey(writer* Buff, LUOV_PublicKey* pk);
void LUOV_deserialize_PublicKey(reader* Buff, LUOV_PublicKey* pk);
void LUOV_destroy_PublicKey(LUOV_PublicKey *pk);

void LUOV_serialize_signature(writer *W, LUOV_Signature *S);
void LUOV_deserialize_signature(reader *R, LUOV_Signature *S);
void LUOV_destroy_signature(LUOV_Signature *S);

#ifdef LUOV

#define SecretKey LUOV_SecretKey
#define PublicKey LUOV_PublicKey
#define Signature LUOV_Signature

#define signDocument LUOV_signDocument
#define generateKeyPair LUOV_generateKeyPair
#define verify LUOV_verify

#define serialize_SecretKey LUOV_serialize_SecretKey
#define deserialize_SecretKey LUOV_deserialize_SecretKey
#define destroy_SecretKey LUOV_destroy_SecretKey

#define serialize_PublicKey LUOV_serialize_PublicKey
#define deserialize_PublicKey LUOV_deserialize_PublicKey
#define destroy_PublicKey LUOV_destroy_PublicKey

#define serialize_signature LUOV_serialize_signature
#define deserialize_signature LUOV_deserialize_signature
#define destroy_signature LUOV_destroy_signature

#endif 

#endif 
