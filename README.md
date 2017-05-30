# ThesisCode

Contains an implementation of the Unbalanced Oil and Vinegar signature scheme and two new signature schemes based on UOV. This repository merely serves as a demonstration of the algorithms and is by no means a secure implementation. For example, this implementation does not use cryptographically secure randomness and makes no attempt at preventing cache timing attacks.

## Structure of the project

The source code is divided in 4 folders: 

- common/ : Contains code which is used by all 3 algorithms such as field arithmetic. This folder also contains code for running and timing the algorithms in the "main.c" file.
- UOVClassic/ : Contains code for the implementation of UOV, with Petzold's method of reducing the size of the public key
- LUOV/ : Contains the implementation for a version of UOV with public and private with coeficients in F_2, but lifted to a large extension field of F_2
- UOVHash : Contains the implementation for a version of UOV which reduced the public key size by using techniques from hash based crypto

## Choosing parameters:

In the parameters.h file one can specify which algorithm has to be used to preform a test, as well as how many key pairs and signatures have to be produced

The parameters that are specific for each of the three algorithms are defined in the files "UOVClassicParameters.h" , "UOVTinF2Parameters.h" and "UOVHashParameters.h"
