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

/* 100-bit */
#define F32
enum { M = 29, ALPHA = 3, TAU = 16, KAPPA = 13, THETA = 14 }; 

/* 128-bit 
#define F48
enum { M = 36, ALPHA = 3 , TAU = 16 , KAPPA = 16 , THETA = 20}; */

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

#endif

#else
#define ALPHA 1
#define TAU 1
#define KAPPA 1
#define THETA 1
#endif