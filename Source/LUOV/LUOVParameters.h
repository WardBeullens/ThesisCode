#ifndef LUOVPARAMETERS_H
#define LUOVPARAMETERS_H

#ifdef LUOV

/*
We define which field to use (options are F16, F32, F48, F64 and F80 ),
M, the number of equations in the UOV system,
and V, the number of vinegar variables
*/

/* 100 BITS OF SECURITY */
	/* CLASSICAL 
		#define F32
		#define M 30
		#define V 105 */

	/* QUANTUM 
		#define F32
		#define M 37
		#define V 200 */

/* 128 BITS OF SECURITY */
	/* CLASSICAL */
		#define F48
		#define M 37
		#define V 137

	/* QUANTUM 
		#define F48
		#define M 45
		#define V 256 */

/* 192 BITS OF SECURITY */
	/* CLASSICAL 
		#define F64
		#define M 54
		#define V 215 */

	/* QUANTUM 
		#define F64
		#define M 65
		#define V 384 */

/* 256 BITS OF SECURITY */ 
	/* CLASSICAL 
		#define F80
		#define M 70
		#define V 293 */

	/* QUANTUM
		#define F80
		#define M 87
		#define V 526*/


/* derived parameters */
#define O M
#define N (O+V)
#define D (V*(V + 1) / 2 + O*V)
#define D2 (O*(O + 1) / 2)

#endif

/*bitcontainer*/

#if M > 64
#define LARGECONTAINER
#endif // M > 64


#ifndef LARGECONTAINER

#define bitcontainer uint64_t
#define xor(a,b) a^b
#define getBit(container,bit) (container & ((uint64_t)1) << bit)
#define empty (0)
#define randomBitcontainerKeccak(rng) (csprn_next_uint64_t(rng))
#define randomBitcontainerMT(MT) (((uint64_t)randomMT(MT)) | (((uint64_t)randomMT(MT)) << 32))
#define serialize_bitcontainer(W,container) serialize_uint64_t(W, container , M)
#define deserialize_bitcontainer(R) deserialize_uint64_t(R,M)

#else

#include "128Bitcontainer.h"
#define bitcontainer bitcontainer128
#define xor xor128
#define getBit getBit128
#define empty empty128
#define randomBitcontainerKeccak randomBitcontainer128Keccak
#define randomBitcontainerMT randomBitcontainer128MT
#define serialize_bitcontainer(Buff,container) serialize_bitcontainer128(Buff,container,M)
#define deserialize_bitcontainer(Buff) deserialize_bitcontainer128(Buff,M)

#endif 

#endif
