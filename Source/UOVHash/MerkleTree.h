#ifndef MERKLETREE_H
#define MERKLETREE_H

#include <stdint.h>
#include <string.h>
#include "parameters.h"
#include "buffer.h"

typedef struct {
	int depth;
	int storedLayerDepth;
	unsigned char* storedLayer;
} MerkleTree ;

MerkleTree newMerkleTree(int depth, int storedLayerDepth,struct UOVHash_SecretKey * sk);
void destroy_MerkleTree(MerkleTree* Tree);

void getRoot(MerkleTree* Tree ,struct UOVHash_SecretKey * sk, writer* W);
void OpenMerkleTreePath(MerkleTree* Tree, struct UOVHash_SecretKey * sk, uint16_t a , writer* W);

void serialize_merkleTree(MerkleTree* Tree, writer* W);
MerkleTree deserialize_merkleTree(int depth, int storedLayerDepth, reader* R);
int VerifyMerkleTreePath(uint64_t c, unsigned char* root, reader* R);

#endif
