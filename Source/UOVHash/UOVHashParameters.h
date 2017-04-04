#define DIVIDE_AND_ROUND_UP(N, S) (((N) + (S) - 1) / (S))

#ifdef UOVHASH

#ifndef UOVHASHPARAMETERS_H
#define UOVHASHPARAMETERS_H

/*
	We define which field to use (options are F16, F32, F48, F64 and F80 ),
	M, the number of equations in the UOV system,
	ALPHA , the number of equations included in the signature,
	TAU , the depth of the merkle tree,
	KAPPA , the length (in bytes) of the hash values used in the merkle tree,
	and THETA , the number of MAC tests included in the signature
*/

/* test 
#define PRIME_FIELD
#define FIELDPRIME 31
#define K 4
#define SIGMA 3
enum { M = 2, ALPHA = 2, TAU = 16, KAPPA = 2, THETA = 1 }; */

/* 100-bit 
#define F32
enum { M = 29, ALPHA = 3, TAU = 16, KAPPA = 13, THETA = 14 }; */

/* 128-bit 
#define F48
enum { M = 36, ALPHA = 3 , TAU = 16 , KAPPA = 16 , THETA = 20}; */

/* 128-bit quantum */
#define F64
#define SIGMA 4
enum { M = 40, ALPHA = 1, TAU = 16, KAPPA = 32, THETA = 42 }; 

/* 128-bit quantum 
#define PRIME_FIELD
#define FIELDPRIME 31
#define K 4
#define SIGMA 18
enum { M = 71, ALPHA = 3, TAU = 16, KAPPA = 32, THETA = 55 }; */

/* 192-bit 
#define F64
enum { M = 52, ALPHA = 3 , TAU = 16 , KAPPA = 24 , THETA = 35}; */

/* 256-bit 
#define F80
enum { M = 68, ALPHA = 3 , TAU = 16 , KAPPA = 32 , THETA = 53 }; */

/*derived parameters */

enum { O = M };
enum { V = 2 * O + 1 };
enum { N = O + V };
enum { D = V*(V + 1) / 2 + O*V };
enum { D2 = O*(O + 1) / 2 };

#ifndef K
	#define K 1
#endif

#ifndef SIGMA
	#define SIGMA 1
#endif

#define MERKLE_PATHS_SIZE (THETA*(KAPPA*TAU + DIVIDE_AND_ROUND_UP(M*K*BITS_PER_FELT ,8)))   /* Number of bytes it takes to encode merkle paths */

#endif

#else
#define ALPHA 1
#define TAU 1
#define KAPPA 1
#define THETA 1
#define MERKLE_PATHS_SIZE 1 
#endif
