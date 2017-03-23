#include "MerkleTree.h"
#include "UOVHash.h"

/*
	Calculates the value of a leaf of the merkle tree i.e. an evaluation of the hat-polynomial at an element

	sk : the secret key
	leaf : the index of the leaf
	W : writer object to which the leaf value is written
*/
void calculateLeaf(UOVHash_SecretKey *sk, uint16_t leaf, writer* W) {
	uint64_t k;
	int j;

	FELT evaluation[M];

	for (j = 0; j < M; j++) {
		evaluation[j] = ZERO;
	}
	if (leaf == 0) {
		for (k = 0; k < M; k++) {
			evaluation[k] = sk->B2.array[0][k];
		}
	}
	else {
		uint16_t x = f16Log(leaf);
		for (j = D2 - 1; j >= 0; j--) {
			for (k = 0; k < M; k++) {
				scalarMultiply(&evaluation[k], x);
				addBtoA(&evaluation[k], &sk->B2.array[j][k]);
			}
		}
	}
	for (j = 0; j < M; j++) {
		serialize_FELT(W, evaluation[j]);
	}
}

/*
	Calculates the hash-value corresponding to an internal node of the merkle tree

	Tree : the merkle tree
	sk : the secret key
	depth : depth of the node
	node : index of the node, lies between 0 and 2^depth
	W : the writer object the hash value is written to
	useStored : whether to use the stored values to speed up the process or not. (Always use this except when computing the values that have to be stored themselves)
*/
void CalculateInternalNode(MerkleTree* Tree, UOVHash_SecretKey *sk, int depth, uint16_t node, writer* W, int useStored) {
	int i;
	if (depth == Tree->depth) {
		unsigned char serEval[M * sizeof(FELT)];
		writer evalW = newWriter(serEval);
		calculateLeaf(sk, node, &evalW);
		csprng rng;
		csprng_init(&rng);
		csprng_seed(&rng, M * sizeof(FELT), serEval);
		csprng_generate(&rng, KAPPA, W->data + W->next);
		W->next += KAPPA;
		return;
	}
	if (useStored == 1 && depth == Tree->storedLayerDepth) {
		for (i = 0; i < KAPPA; i++) {
			W->data[W->next + i] = Tree->storedLayer[node*KAPPA + i];
		}
		W->next += KAPPA;
		return;
	}
	unsigned char * children = malloc(2 * KAPPA);
	writer C = newWriter(children);
	CalculateInternalNode(Tree, sk, depth + 1, node * 2, &C, useStored);
	CalculateInternalNode(Tree, sk, depth + 1, (node * 2) + 1, &C, useStored);
	csprng rng;
	csprng_init(&rng);
	csprng_seed(&rng, 2 * KAPPA, children);
	free(children);
	csprng_generate(&rng, KAPPA, W->data + W->next);
	W->next += KAPPA;
}

/*
	Creates a new merkle tree. One layer of the nodes is precomputed and stored, this speeds up the process of generating a merkle path.

	depth : depth of the new tree
	storedLayerDepth: depth of the layer of nodes that is stored
	sk : the secret key

	returns : the new merkle tree
*/
MerkleTree newMerkleTree(int depth, int storedLayerDepth, UOVHash_SecretKey *sk) {
	uint16_t i;
	MerkleTree MT;
	MT.depth = depth;
	MT.storedLayerDepth = storedLayerDepth;
	MT.storedLayer = malloc((1 << storedLayerDepth) * KAPPA);
	writer W = newWriter(MT.storedLayer);
	for (i = 0; i < (1 << storedLayerDepth); i++) {
		CalculateInternalNode(&MT, sk, storedLayerDepth, i, &W, 0);
	}
	return MT;
}

/*
	Frees the memory allocated by a merkle tree
*/
void destroy_MerkleTree(MerkleTree* Tree) {
	free(Tree->storedLayer);
}

/*
	Write a merkle tree to a char array

	Tree : the tree to write
	W : the writer object
*/
void serialize_merkleTree(MerkleTree* Tree, writer* W) {
	int i;
	for (i = 0; i < (1 << Tree->storedLayerDepth)*KAPPA; i++) {
		W->data[i + W->next] = Tree->storedLayer[i];
	}
	W->next += (1 << Tree->storedLayerDepth)*KAPPA;
}

/*
	Reads a merkle tree from a char array

	depth : the depth of the tree to read
	storedlayerDepth : the depth of the stored layer of the tree to read
	R : reader object
*/
MerkleTree deserialize_merkleTree(int depth, int storedLayerDepth, reader* R) {
	MerkleTree Tree;
	int i;
	Tree.depth = depth;
	Tree.storedLayerDepth = storedLayerDepth;
	Tree.storedLayer = malloc((1 << storedLayerDepth)*KAPPA);
	for (i = 0; i < (1 << storedLayerDepth)*KAPPA; i++) {
		Tree.storedLayer[i] = R->data[i + R->next];
	}
	R->next += (1 << storedLayerDepth)*KAPPA;
	return Tree;
}

/*
	Calculate the root of a merkle tree and write it to a char array

	Tree : the merkle tree
	sk : the secret key
	W : a writer object that the root is written to
*/
void getRoot(MerkleTree* Tree, UOVHash_SecretKey *sk, writer* W) {
	CalculateInternalNode(Tree, sk , 0, 0, W, 1);
}

/*
	Opens a merkle path and writes it to a char array

	Tree : a merkle tree
	sk : the secret key
	a : index of the leaf to open
	W : a writer object to write the path to
*/
void OpenMerkleTreePath(MerkleTree* Tree, UOVHash_SecretKey* sk, uint16_t a, writer* W) {
	int i;
	calculateLeaf(sk, a, W);
	for (i = Tree->depth; i > 0; i--) {
		a ^= 1;
		CalculateInternalNode(Tree, sk, i, a, W, 1);
		a >>= 1;
	}
}

/*
	Verify a merkle path read from a char array

	c : index of the leaf that is verified
	root : the root of the merkle tree
	R : a reader object
*/
int VerifyMerkleTreePath(uint64_t c, unsigned char* root, reader* R) {
	int depth, i;
	unsigned char leaf[M * sizeof(FELT)];
	unsigned char buf[2 * KAPPA];
	memcpy(leaf, R->data + R->next, M * sizeof(FELT));
	R->next += M * sizeof(FELT);
	csprng rng;
	csprng_init(&rng);
	csprng_seed(&rng, M * sizeof(FELT), leaf);
	for (depth = TAU; depth > 0; depth--) {
		if ((c & 1) == 0) {
			csprng_generate(&rng, KAPPA, buf);
			memcpy(buf + KAPPA, R->data + R->next, KAPPA);
			R->next += KAPPA;
		}
		else
		{
			memcpy(buf, R->data + R->next, KAPPA);
			R->next += KAPPA;
			csprng_generate(&rng, KAPPA, buf + KAPPA);
		}
		csprng_init(&rng);
		csprng_seed(&rng, 2 * KAPPA, buf);
		c /= 2;
	}
	csprng_generate(&rng, KAPPA, buf);
	for (i = 0; i < KAPPA; i++) {
		if (buf[i] != root[i]) {
			return 0;
		}
	}
	return 1;
}