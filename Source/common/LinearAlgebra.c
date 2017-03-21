#include <stdio.h>
#include <stdlib.h>

#include "LinearAlgebra.h"
#include "buffer.h"

/*
	Write a matrix to a char array

	W : The writer object
	M : The matrix to write
*/
void serialize_matrix(writer* W, Matrix M) {
	int i, j;
	for (i = 0; i < M.rows; i++) {
		for (j = 0; j < M.cols; j++) {
			serialize_FELT(W,M.array[i][j]);
		}
	}
}

/*
	Reads a matrix from a char array

	R : The reader object
	M : The matrix to read out (this matrix should already have the desired dimensions)
*/
void deserialize_matrix(reader *R, Matrix M) {
	int i, j;
	for (i = 0; i < M.rows; i++) {
		for (j = 0; j < M.cols; j++) {
			M.array[i][j] = deserialize_FELT(R);
		}
	}
}

/*
	Creates a new matrix

	rows : the number of rows of the new matrix
	cols : the number of columns of the new matrix

	returns : The new matrix
*/
Matrix newMatrix(unsigned int rows, unsigned int cols) {
	unsigned int i;
	Matrix new;
	new.rows = rows;
	new.cols = cols;
	new.array = malloc(rows * sizeof(FELT*));
	for (i = 0; i < rows; i++) {
		new.array[i] = malloc(cols * sizeof(FELT));
	}
	return new;
}

/*
	Creates a new matrix whose entries are zero

	rows : the number of rows of the new matrix
	cols : the number of columns of the new matrix

	returns : The new matrix
*/
Matrix zeroMatrix(unsigned int rows, unsigned int cols) {
	int i, j;
	Matrix newMat = newMatrix(rows, cols);
	for (i = 0; i < rows; i++) {
		for (j = 0; j < cols; j++) {
			newMat.array[i][j] = ZERO;
		}
	}
	return newMat;
}

/*
	Free the memory of a matrix

	mat : the matrix to destroy
*/
void destroy(Matrix mat) {
	int i;
	for (i = 0; i < mat.rows; i++) {
		free(mat.array[i]);
	}
	free(mat.array);
}

/*
	Creates a new matrix, whose entries are generated by a Mersenne twister

	rows : The number of rows of the new matrix
	cols : The number of columns of the new matrix
	MT :  The mersenne twister used to generate the entries

	returns : The new matrix
*/
Matrix randomMatrixMT(int rows, int cols, twister * MT) {
	int i, j;
	Matrix mat = newMatrix(rows, cols);
	for (i = 0; i < rows; i++) {
		for (j = 0; j < cols; j++) {
			mat.array[i][j] = randomMTFELT(MT);
		}
	}
	return mat;
}

/*
	Creates a new matrix, whose entries are generated by a Keccak csprng

	rows : The number of rows of the new matrix
	cols : The number of columns of the new matrix
	rng :  The csprng used to generate the entries

	returns : The new matrix
*/
Matrix randomMatrixrng(int rows, int cols, csprng *rng) {
	int i,j;
	Matrix mat = newMatrix(rows, cols);
	for (i = 0; i < rows; i++) {
		for (j = 0; j < cols; j++) {
			mat.array[i][j] = randomFELT(rng);
		}
	}
	return mat;
}

/*
	Prints the matrix
*/
void printMatrix(Matrix Mat) {
	int i, j;
	for (i = 0; i < Mat.rows; i++) {
		for (j = 0; j < Mat.cols; j++) {
			printFELT(Mat.array[i][j]);
		}
		printf("\n");
	}
	printf("\n");
}

/*
	Prints the transpose of a matrix
*/
void printMatrixt(Matrix Mat) {
	int i, j;
	for (i = 0; i < Mat.cols; i++) {
		for (j = 0; j < Mat.rows; j++) {
			printFELT(Mat.array[j][i]);
		}
		printf("\n");
	}
	printf("\n");
}

/*
	Multiplies two matrices

	A,B : Two matrices

	returns : The matrix AB
*/
Matrix multiplyAB(Matrix A, Matrix B) {
	int i, j, k;
	Matrix AB = newMatrix(A.rows, B.cols);

	for (i = 0; i < A.rows; i++) {
		for (j = 0; j < B.cols; j++) {
			AB.array[i][j] = ZERO;
			for (k = 0; k < A.cols; k++) {

				AB.array[i][j] = add(AB.array[i][j], multiply(A.array[i][k], B.array[k][j]));
			}
		}
	}

	return AB;
}

/*
	Multiplies a matrix A with a submatrix of a matrix B

	A,B : Two matrices
	Btop,Bbottom : The rows defining the top (inclusive) and bottom (exclusive) of a submatrix of B
	Bleft , Bright : The columns defining the left (inclusive) and right (exclusive) of a submatrix of B

	returns : The product of A with the submatrix of B
*/
Matrix multiplyASubB(Matrix A, Matrix B, int Btop, int Bleft, int Bbottom, int Bright) {
	int i, j, k;
	Matrix AB = newMatrix(A.rows, Bright - Bleft);

	for (i = 0; i < A.rows; i++) {
		for (j = 0; j < Bright - Bleft; j++) {
			AB.array[i][j] = ZERO;
			for (k = 0; k < A.cols; k++) {
				AB.array[i][j] = add(AB.array[i][j], multiply(A.array[i][k], B.array[k + Bleft][j + Btop]));
			}
		}
	}

	return AB;
}

/*
Multiplies a matrix A with the transpose of a submatrix of a matrix B

A,B : Two matrices
Btop,Bbottom : The rows defining the top (inclusive) and bottom (exclusive) of a submatrix of B
Bleft , Bright : The columns defining the left (inclusive) and right (exclusive) of a submatrix of B

returns : The product of A with the transpose of the submatrix of B
*/
Matrix multiplyASubBt(Matrix A, Matrix B, int Btop, int Bleft, int Bbottom, int Bright) {
	int i, j, k;
	Matrix AB = newMatrix(A.rows, Bbottom - Btop);

	for (i = 0; i < A.rows; i++) {
		for (j = 0; j < Bbottom - Btop; j++) {
			AB.array[i][j] = ZERO;
			for (k = 0; k < A.cols; k++) {
				AB.array[i][j] = add(AB.array[i][j], multiply(A.array[i][k], B.array[j + Btop][k + Bleft]));
			}
		}
	}

	return AB;
}

/*
	Multiplies the transpose of a matrix A with a matrix B

	A,B : Two matrices

	returns : The product of the transpose of A and B
*/
Matrix multiplyAtB(Matrix A, Matrix B) {
	int i, j, k;
	Matrix AtB = newMatrix(A.cols, B.cols);

	for (i = 0; i < A.cols; i++) {
		for (j = 0; j < B.cols; j++) {
			AtB.array[i][j] = ZERO;
			for (k = 0; k < A.rows; k++) {
				AtB.array[i][j] = add(AtB.array[i][j], multiply(A.array[k][i], B.array[k][j]));
			}
		}
	}

	return AtB;
}

/*
	Add a matrix B to a matrix A

	A : The matrix to add B to
	B : The matrix to add to A
*/
void addInPlace(Matrix A, Matrix B) {
	int i, j;
	for (i = 0; i < A.rows; i++) {
		for (j = 0; j < A.cols; j++) {
			A.array[i][j] = add(A.array[i][j], B.array[i][j]);
		}
	}
	return;
}

/*
	Creates a copy of a matrix

	M : A matrix

	returns : A copy of M
*/
Matrix copy(Matrix M) {
	int i, j;
	Matrix copy = newMatrix(M.rows, M.cols);
	for (i = 0; i < M.rows; i++) {
		for (j = 0; j < M.cols; j++) {
			copy.array[i][j] = M.array[i][j];
		}
	}
	return copy;
}

/*
	Flips swaps two rows of a matrix

	A : A matrix
	row1 , row2 : The rows of A that have to be swapped
*/
void flipRows(Matrix A, int row1, int row2) {
	FELT *temp = A.array[row1];
	A.array[row1] = A.array[row2];
	A.array[row2] = temp;
}

/*
	Rescale a row of a matrix

	A : A matrix
	row : the index of the row that has to be rescaled
	a : A field element
*/
void scaleRow(Matrix A, int row, FELT a) {
	int i;
	for (i = 0; i < A.cols; i++) {
		A.array[row][i] = multiply(A.array[row][i], a);
	}
}

/*
	Add a part of the scalar multiple of one row of a matrix to another row of that matrix

	A : A matrix
	destrow : The row to add to
	sourcerow  : The row that is multiplied by a scalar and added to destrow
	constant : The contant that sourcerow is multiplied with
	offset : Only the entries in columns with index larger than or equal to offset are affected
*/
void rowOp(Matrix A, int destrow, int sourcerow, FELT constant, int offset)
{
	int j;
	FELT T;
	if (isEqual(constant, ZERO))
		return;

	for (j = offset; j < A.cols; ++j)
	{
		T = multiply(A.array[sourcerow][j], constant);
		addBtoA(&A.array[destrow][j], &T);
	}
}

/* 
	Puts the first part of the augmented matrix in row echelon form.
	
	A : A matrix

	returns : The rank ok the first part of the row echelon form of A
*/
int rowEchelonAugmented(Matrix A)
{
	int i,col;
	int row = 0;
	for (col = 0; col < A.cols - 1; ++col)
	{
		for (i = row; i < A.rows; ++i)
		{
			if (!isEqual(A.array[i][col], ZERO))
			{
				if (i != row)
				{
					flipRows(A, i, row);
				}
				break;
			}
		}

		if (i == A.rows)
		{
			continue;
		}

		scaleRow(A, row, inverse(A.array[row][col]));

		for (i++; i < A.rows; ++i)
		{
			rowOp(A, i, row, minus(A.array[i][col]), col);
		}

		row++;

		if (row == A.rows)
		{
			break;
		}
	}
	return row;
}

/* 
	Calculates a solution to a linear system described by an augmented matrix

	A : The augmented matrix of some linear system of equations

	returns : a solution to the linear system, or a matrix with 0 colums if no solutions exist 
*/
Matrix getSolution(Matrix A) {
	int i,j,col,row;
	FELT T;
	Matrix solution;
	int rank = rowEchelonAugmented(A);

	for (i = rank; i < A.rows; i++) {
		if (!isEqual(A.array[i][A.cols - 1], ZERO)) {
			/* Rouch�-Capelli says there are no solutions. */
			Matrix M;
			M.cols = 0;
			return M;
		}
	}

    solution = zeroMatrix(A.cols - 1, 1);
	for (row = rank - 1; row >= 0; row--) {
		col = row;
		while (isEqual(A.array[row][col], ZERO))
		{
			col++;
		}

		solution.array[col][0] = A.array[row][A.cols - 1];
		for (j = col + 1; j < A.cols - 1; j++) {
			T = minus(multiply(solution.array[j][0], A.array[row][j]));
			addBtoA(&solution.array[col][0], &T);
		}
	}

	return solution;
}

/*
	Creates a copy of a submatrix of a matrix

	A : A matrix
	top , bottom : The rows of the top (inclusive) and bottom (exclusive) of a submatrix of A
	left, right  : The columns of the left (inclusive) and right (exclusive) of a submatrix of A

	returns : A copy of the submatrix of A
*/
Matrix subMatrix(Matrix A, int top, int left, int bottom, int right) {
	int i, j;
	Matrix sub = newMatrix(bottom - top, right - left);
	for (i = top; i < bottom; i++) {
		for (j = left; j < right; j++) {
			sub.array[i - top][j - left] = A.array[i][j];
		}
	}
	return sub;
}

/*
	Checks whether two matrices are the same

	A,B : Two matrices

	returns : 1 if A and B have the same dimensions and entries, 0 otherwise
*/
int equals(Matrix A, Matrix B) {
	int i, j;
	if (A.cols != B.cols)
		return 0;
	if (A.rows != B.rows)
		return 0;
	for (i = 0; i < A.rows; i++) {
		for (j = 0; j < A.cols; j++) {
			if (!isEqual(A.array[i][j], B.array[i][j]))
				return 0;
		}
	}
	return 1;
}

/*
	Use a matrix to seed a Keccak csprng

	rng : the pseudorandom number generator to seed
	M : The matrix whose entries are the seed for rng
*/
void csprng_seed_matrix(csprng* rng, Matrix M) {
	unsigned char* m = malloc(M.cols * M.rows * sizeof(FELT));
	writer W = newWriter(m);

	serialize_matrix(&W, M);
	csprng_seed(rng, W.next, m);
	free(m);
}

/*
	Builds the augmented matrix for the linear system resulting from fixing the vinegar variables in a UOV system

	A : The augmented matrix
	signature : A N-by-1 matrix whose first V entries are the vinegar variables
	target : The target for the UOV system
	system : The transpose of the first V*(V+1)/2 + V*O columns of the macaulay matrix for the UOV system (the last O*(O+1)/2 columns are zero anyway )
*/
void BuildAugmentedMatrix(Matrix A, Matrix signature, Matrix target, Matrix system) {
	int i, j, k;
	for (k = 0; k < M; k++) {
		A.array[k][O] = target.array[k][0];
	}

	int col = 0;
	for (i = 0; i < V; i++) {
		for (j = i; j < V; j++) {
			FELT prod = multiply(signature.array[i][0], signature.array[j][0]);
			for (k = 0; k < M; k++) {
				A.array[k][O] = add(A.array[k][O], minus(multiply(prod, system.array[col][k])));
			}
			col++;
		}

		for (j = 0; j < O; j++) {
			for (k = 0; k < M; k++) {
				A.array[k][j] = add(A.array[k][j], multiply(signature.array[i][0], system.array[col][k]));
			}
			col++;
		}
	}
}

/*
	Finds a solution to a UOVSystem

	system : The transpose of the first V*(V+1)/2 + V*O columns of the macaulay matrix for the UOV system (the last O*(O+1)/2 columns are zero anyway )
	target : The target for the UOV system
	rng : The pseudorandom generator used for randomly choosing the vinegar variables

	returns : A N-by-1 matrix containing a solution 
*/
Matrix findSolutionOfUOVSystem(Matrix system, Matrix target, csprng *rng) {
	int i;
	Matrix signature;
	int tries = 0;

	Matrix A = zeroMatrix(M, O + 1);
	Matrix x;
	x.cols = 0;
	while (x.cols == 0) {
		tries++;
		signature = randomMatrixrng(N, 1, rng);
		signature.array[0][0] = ONE;

		BuildAugmentedMatrix(A, signature, target, system);
		x = getSolution(A);

		if (x.cols == 0) {
			destroy(signature);
		}
	}
	for (i = 0; i < O; i++) {
		signature.array[i + signature.rows - O][0] = x.array[i][0];
	}

	destroy(x);
	destroy(A);

	return signature;
}

/*
	returns the index of the column of the Macaulay matrix correspoding to the monomial x_a*x_b, where a<=b
*/
int lexindex(int a, int b) {
	return N*a - ((a*(a + 1)) / 2) + b;
}