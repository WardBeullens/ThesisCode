#ifndef PARAMETERS_H
#define PARAMETERS_H

#define UOVHASH                   /* Determines which algorithm is used, options are UOVCLASSIC, UOVTINF2 and UOVHASH */
#define NUMBER_OF_KEYPAIRS 1      /* Number of keypairs that is generated during test */
#define SIGNATURES_PER_KEYPAIR 10 /* Number of times each keypair is used to sign a random document, and verify the signature */

#include "UOVHashParameters.h"
#include "UOVClassicParameters.h"
#include "UOVTinF2Parameters.h"

#endif