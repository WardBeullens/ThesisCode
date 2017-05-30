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

/* 100 BITS OF SECURITY */

	/* CLASSICAL SECURITY */

		/* PROVABLE REDUCTION TO UOV
			#define F32
			enum { M = 29, ALPHA = 3, TAU = 16, KAPPA = 13, THETA = 14 }; */

		/* NO PROVABLE REDUCTION TO UOV 
			#define PRIME_FIELD
			#define FIELDPRIME 127
			#define K 3
			#define SIGMA 8
			enum { M = 39, ALPHA = 2, TAU = 16, KAPPA = 13, THETA = 13 }; */

	/* QUANTUM SECURITY */

		/* PROVABLE REDUCTION TO UOV 
			#define F32
			enum { M = 35, ALPHA = 7, TAU = 16, KAPPA = 26, THETA = 28 }; */

		/* NO PROVABLE REDUCTION TO UOV
			#define PRIME_FIELD
			#define FIELDPRIME 127
			#define K 3
			#define SIGMA 15
			enum { M = 47, ALPHA = 2, TAU = 16, KAPPA = 26, THETA = 27 }; */

/* 128 BITS OF SECURITY */

	/* CLASSICAL SECURITY */

		/* PROVABLE REDUCTION TO UOV */
			#define F48
			enum {M = 36, ALPHA = 3 , TAU = 16 , KAPPA = 16 , THETA = 20}; 

		/* NO PROVABLE REDUCTION TO UOV 
			#define PRIME_FIELD
			#define FIELDPRIME 127
			#define K 3
			#define SIGMA 10
			enum { M = 50, ALPHA = 2, TAU = 16, KAPPA = 16, THETA = 18 }; */

	/* QUANTUM SECURITY */

		/* PROVABLE REDUCTION TO UOV 
			#define F48
			enum { M = 42, ALPHA = 6, TAU = 16, KAPPA = 32, THETA = 39 }; */

		/* NO PROVABLE REDUCTION TO UOV 
			#define PRIME_FIELD
			#define FIELDPRIME 127
			#define K 3
			#define SIGMA 19
			enum { M = 60, ALPHA = 2, TAU = 16, KAPPA = 32, THETA = 38 }; */

/* 192 BITS OF SECURITY */

	/* CLASSICAL SECURITY */

		/* PROVABLE REDUCTION TO UOV  
			#define F64
			enum { M = 52, ALPHA = 3 , TAU = 16 , KAPPA = 24 , THETA = 35}; */

		/* NO PROVABLE REDUCTION TO UOV 
			#define PRIME_FIELD
			#define FIELDPRIME 127
			#define K 3
			#define SIGMA 14
			enum { M = 76, ALPHA = 2, TAU = 16, KAPPA = 24, THETA = 32 }; */

	/* QUANTUM SECURITY */
		/* PROVABLE REDUCTION TO UOV 
			#define F64
			enum { M = 62, ALPHA = 6 , TAU = 16 , KAPPA = 48 , THETA = 69}; */

		/* NO PROVABLE REDUCTION TO UOV 
			#define PRIME_FIELD
			#define FIELDPRIME 127
			#define K 3
			#define SIGMA 28
			enum { M = 92, ALPHA = 2, TAU = 16, KAPPA = 48, THETA = 70 }; */


/* 256 BITS OF SECURITY */
	
	/* CLASSICAL SECURITY*/

		/* PROVABLE REDUCTION TO UOV 
			#define F80
			enum { M = 68, ALPHA = 4 , TAU = 16 , KAPPA = 32 , THETA = 54 }; */

		/* NO PROVABLE REDUCTION TO UOV 
			#define PRIME_FIELD
			#define FIELDPRIME 127
			#define K 3
			#define SIGMA 19
			enum { M = 102, ALPHA = 2, TAU = 16, KAPPA = 32, THETA = 49 }; */

	/* QUANTUM SECURITY */
		/* PROVABLE REDUCTION TO UOV 
			#define F80
			enum { M = 83, ALPHA = 7, TAU = 16, KAPPA = 64, THETA = 107 }; */

		/* NO PROVABLE REDUCTION TO UOV 
			#define PRIME_FIELD
			#define FIELDPRIME 127
			#define K 3
			#define SIGMA 37
			enum { M = 124, ALPHA = 2, TAU = 16, KAPPA = 64, THETA = 110 }; */

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
