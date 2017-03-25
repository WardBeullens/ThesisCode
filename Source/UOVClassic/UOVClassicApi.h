#include "parameters.h"

#ifdef UOVCLASSIC

#define DIVIDE_AND_ROUND_UP(N, S) (((N) + (S) - 1) / (S))

#define CRYPTO_SECRETKEYBYTES (DIVIDE_AND_ROUND_UP((O*V + D*M)*BITS_PER_FELT,8) + 32 ) /* Number of bytes it takes to encode the secret key */
#define CRYPTO_PUBLICKEYBYTES (4+ DIVIDE_AND_ROUND_UP((D2*M)*BITS_PER_FELT,8) )        /* Number of bytes it takes to encode the public key */
#define CRYPTO_BYTES (DIVIDE_AND_ROUND_UP(N*BITS_PER_FELT,8))                          /* Number of bytes it takes to encode a signature */

#endif 
