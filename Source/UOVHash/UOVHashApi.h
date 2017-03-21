#include "parameters.h"

#ifdef UOVHASH

#define CRYPTO_SECRETKEYBYTES (4+KAPPA + (1<<(TAU/2))*KAPPA )                                 /* Number of bytes it takes to encode the secret key */
#define CRYPTO_PUBLICKEYBYTES (4+KAPPA)                                                       /* Number of bytes it takes to encode the public key */
#define CRYPTO_BYTES ((N + D2*ALPHA)*sizeof(FELT) + (THETA*(KAPPA*TAU + M * sizeof(FELT))) )  /* Number of bytes it takes to encode a signature */

#endif 
