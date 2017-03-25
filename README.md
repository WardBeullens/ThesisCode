# ThesisCode

Contains an implementation of Unbalanced Oil and Vinegar, and two new signature schemes based on UOV

## Structure of the project

The source code is divided in 4 folders: 

- common/ : Contains code which is used by all 3 algorithms, in particular main.c contains code for running and timing the algorithms
- UOVClassic/ : Contains code for the implementation of UOV, with Petzold's method of reducing the size of the public key
- UOVTinF2/ : Contains the implementation for a version of UOV with public and private keys over F_2, but with signatures in an extension field of F_2
- UOVHash : Contains the implementation for a version of UOV which reduced the public key size by using techniques from hash based crypto

## Choosing parameters:

In the parameters.h file one can specify which algorithm has to be used to preform a test, as well as how many key pairs and signatures have to be produced

The parameters that are specific for each of the three algorithms are defined in the files "UOVClassicParameters.h" , "UOVTinF2Parameters.h" and "UOVHashParameters.h"
