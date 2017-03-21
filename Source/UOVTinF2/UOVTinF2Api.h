#include "parameters.h"

#ifdef UOVTINF2

#define CRYPTO_SECRETKEYBYTES ( 36 )              /* Number of bytes it takes to encode the secret key */
#define CRYPTO_PUBLICKEYBYTES ( 5 + ((D2*M)/8) )  /* Number of bytes it takes to encode the public key */
#define CRYPTO_BYTES ( N*sizeof(FELT) )           /* Number of bytes it takes to encode a signature */

#endif 