#include "parameters.h"

#ifdef UOVCLASSIC

#define CRYPTO_SECRETKEYBYTES ((O*V + D*M)*sizeof(FELT) + 32 ) /* Number of bytes it takes to encode the secret key */
#define CRYPTO_PUBLICKEYBYTES (4+ (D2*M)*sizeof(FELT) )        /* Number of bytes it takes to encode the public key */
#define CRYPTO_BYTES (N*sizeof(FELT))                          /* Number of bytes it takes to encode a signature */

#endif 
