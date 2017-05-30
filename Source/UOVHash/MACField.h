#ifndef MACFIELD_H
#define MACFIELD_H

#include "parameters.h"
#include "buffer.h"
#include "PrimeField.h"
#include "F16Field.h"
#include "F32Field.h"
#include "F48Field.h"
#include "F64Field.h"
#include "F80Field.h"

#ifndef K
#define K 1
#endif

typedef struct MAC_FELT
{
	unsigned char coef[K];
} MAC_FELT;

MAC_FELT MAC_multiply(MAC_FELT a, MAC_FELT b);
MAC_FELT MAC_add(MAC_FELT a, MAC_FELT b);
void serialize_MAC_FELT(writer* W, MAC_FELT a);
MAC_FELT deserialize_MAC_FELT(reader* R);
MAC_FELT MAC_scalarMultiply(unsigned char a, MAC_FELT b);
int MAC_isEqual(MAC_FELT a, MAC_FELT b);
MAC_FELT intToMAC_FELT(uint64_t a);

#if K==3
	static const MAC_FELT MAC_ZERO = { { 0,0,0 } };
#endif
#if K==4
	static const MAC_FELT MAC_ZERO = { { 0,0,0,0 } };
#endif

#if K==1
	#define MACFELT FELT
	#define	MACmultiply multiply
	#define	MACadd add
	#define serialize_MACFELT serialize_FELT
	#define deserialize_MACFELT deserialize_FELT
	#define MACZERO ZERO
	#define MACscalarMultiply multiply
	#define setCoef(x,y,z) x = z
	#define MACisEqual isEqual
	#define intToMACFELT intToFELT
#else
	#define MACFELT MAC_FELT
	#define	MACmultiply MAC_multiply
	#define	MACadd MAC_add
	#define serialize_MACFELT serialize_MAC_FELT
	#define deserialize_MACFELT deserialize_MAC_FELT
	#define MACZERO MAC_ZERO
	#define MACscalarMultiply MAC_scalarMultiply
	#define setCoef(x,y,z) x.coef[y] = z
	#define MACisEqual MAC_isEqual
	#define intToMACFELT intToMAC_FELT
#endif


#endif 
