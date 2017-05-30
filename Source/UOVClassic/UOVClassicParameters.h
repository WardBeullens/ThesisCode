#ifdef UOVCLASSIC

#ifndef CLASSICUOVPARAMETERS_H
#define CLASSICUOVPARAMETERS_H

/* 
	We define which field to use (options are PRIME_FIELD, F16, F32, F48, F64 and F80 ),
	and M, the number of equations in the UOV system 
*/

/* example1 : 100 bits of security */
	#define PRIME_FIELD
	#define FIELDPRIME 127
	#define M 39

/* example1 : 100-bits of security  
	#define F32
	#define M 29 */

/*derived parameters */
enum { O = M };
enum { V = 2 * O + 1 };
enum { N = O + V };
enum { D = V*(V + 1) / 2 + O*V };
enum { D2 = O*(O + 1) / 2 };

#endif

#endif
