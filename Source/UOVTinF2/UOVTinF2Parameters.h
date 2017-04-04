#ifndef UOVTINF2PARAMETERS_H
#define UOVTINF2PARAMETERS_H

#ifdef UOVTINF2

/*
We define which field to use (options are F16, F32, F48, F64 and F80 ),
M, the number of equations in the UOV system,
and V, the number of vinegar variables
*/

/* 100-bit 
#define F32
enum { M = 29, V = 83 }; */

/* 128- bit 
#define F48
enum { M = 37, V = 136 }; */

/* 128- bit quantum */
#define F80
enum { M = 41, V = 264 };

/* 192-bit  
#define F64
enum { M = 54, V = 225 };*/

/* 256-bit 
#define F80
#define LARGECONTAINER
enum { M = 70, V = 302 }; */


/* derived parameters */
enum { O = M  };
enum { N = O + V };
enum { D = V*(V + 1) / 2 + O*V };
enum { D2 = O*(O + 1) / 2 };

#endif

/*bitcontainer*/

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
