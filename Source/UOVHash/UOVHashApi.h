#include "parameters.h"

#ifdef UOVHASH

#ifndef BITS_PER_FELT
#define BITS_PER_FELT sizeof(FELT)*8
#endif

#define DIVIDE_AND_ROUND_UP(N, S) (((N) + (S) - 1) / (S))

#define CRYPTO_SECRETKEYBYTES (4+KAPPA + (1<<(TAU/2))*KAPPA )                                     /* Number of bytes it takes to encode the secret key */
#define CRYPTO_PUBLICKEYBYTES (4+KAPPA)                                                           /* Number of bytes it takes to encode the public key */
#define CRYPTO_BYTES (DIVIDE_AND_ROUND_UP((N + D2*ALPHA)*BITS_PER_FELT,8) +  MERKLE_PATHS_SIZE )  /* Number of bytes it takes to encode a signature */

#endif 
