#ifndef LINEARALGEBRA_H
#define LINEARALGEBRA_H

#include "csprng.h"
#include "twister.h"

#include "PrimeField.h"
#include "F16Field.h"
#include "F32Field.h"
#include "F48Field.h"
#include "F64Field.h"
#include "F80Field.h"

#define PRINTMATRIX(M) printf(#M " = \n"); printMatrix(M);

/*Matrix over F_Q*/
typedef struct {
	int rows;
	int cols;
	FELT** array;
} Matrix;

void serialize_matrix(writer* Buff, Matrix Mat);
void deserialize_matrix(reader *Buff, Matrix Mat);

Matrix newMatrix(unsigned int rows, unsigned int cols);
Matrix zeroMatrix(unsigned int rows, unsigned int cols);
void destroy(Matrix mat);
Matrix randomMatrixMT(int rows, int cols, twister * MT);
Matrix randomMatrixrng(int rows, int cols, csprng* rng);
void printMatrix(Matrix Mat);
void printMatrixt(Matrix Mat);

void csprng_seed_matrix(csprng* rng, Matrix Mat);

Matrix multiplyAB(Matrix A, Matrix B);

Matrix multiplyAtB(Matrix A, Matrix B);

void addInPlace(Matrix A, Matrix B);

Matrix copy(Matrix);

Matrix getSolution(Matrix A);

int lexindex(int a, int b);

Matrix subMatrix(Matrix A, int top, int left, int bottom, int right);

int equals(Matrix A, Matrix B);

Matrix findSolutionOfUOVSystem(Matrix system, Matrix target, csprng *rng);

#endif
