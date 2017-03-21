#ifdef UOVCLASSIC

#ifndef CLASSICUOVPARAMETERS_H
#define CLASSICUOVPARAMETERS_H

/* 
	We define which field to use (options are F16, F32, F48, F64 and F80 ),
	and M, the number of equations in the UOV system 
*/

/* 100-bit */
#define F32
enum { M = 29 };

/* 128-bit : q = 2^48 
#define F48
enum { M = 36 };*/ 

/* 192-bit : q > 2^64
#define F64
enum { M = 52 };*/

/* 256-bit
#define F80
enum { M = 68 };*/

/*derived parameters */
enum { O = M };
enum { V = 2 * O + 1 };
enum { N = O + V };
enum { D = V*(V + 1) / 2 + O*V };
enum { D2 = O*(O + 1) / 2 };

#endif

#endif
