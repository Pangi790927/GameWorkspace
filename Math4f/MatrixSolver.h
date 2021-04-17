#ifndef MATRIX_SOLVER_H
#define MATRIX_SOLVER_H

#include "Matrix.h"

namespace Math
{
	/** Lower Triangular Matrix Ax = b
	*	v 0 0 0
	*	v v 0 0
	*	v v v 0
	*	v v v v
	**/
	template <int size, typename Type>
	Vector<size, Type> solveLTM(Matrix <size, size, Type> &A, Vector<size, Type> &b) {
		Vector<size, Type> x;

		for (int i = 0; i < size; i++) {
			Type solPart = b(i);

			if (i > 0)
				for (int j = 0; j < i; j++)
					solPart -= A(i, j) * x(j);

			x(i) = solPart / A(i, i);
		}

		return x;
	}

	/** Upper Triangular Matrix Ax = b
	*	v v v v
	*	0 v v v
	*	0 0 v v
	*	0 0 0 v
	**/
	template <int size, typename Type>
	Vector<size, Type> solveUTM (Matrix <size, size, Type> &A, Vector<size, Type> &b) {
		Vector<size, Type> x;

		for (int i = size - 1; i >= 0; i++) {
			Type solPart = b(i);

			if (i < size - 1)
				for (int j = size - 1; j > i; j--)
					solPart -= A(i, j) * x(j);

			x(i) = solPart / A(i, i);
		}

		return x;
	}

	/** Gussian Elimination Step - the matrix, will be modified and the transformations will be stored in A
	*							   (this is a helper function)
	*	U U U U U
	*	M U U U U
	*	M M U U U
	*	M M M U U
	*	M M M M U
	**/
	template <int size, typename Type>
	void gaussianEliminationKStep (Matrix <size, size, Type> &A, int k) {
		for (int i = k + 1; i < size; i++) {
			A(i, k) /= A(k, k);
		}
		for (int i = k + 1; i < size; i++)
			for (int j = k + 1; j < size; j++)
				A(i, j) -= A(i, k) * A(k, j);
	}

	/** Gussian Elimination A - the matrix, will be modified and the transformations will be stored in A
	*	U U U U U
	*	M U U U U
	*	M M U U U
	*	M M M U U
	*	M M M M U
	**/
	template <int size, typename Type>
	void gaussianElimination (Matrix <size, size, Type> &A) {
		for (int k = 0; k < size - 1; k++) {
			gaussianEliminationKStep(A, k);
		}
	}

	/** Gussian Elimination A - the matrix, will be modified and the transformations will be stored in A,
	* 							pivot = max(row(k)[i]), k <- i, size; (returns the pivots)
	*	U U U U U
	*	M U U U U
	*	M M U U U
	*	M M M U U
	*	M M M M U
	**/
	template <int size, typename Type>
	Vector <size, Type> partialPivotGE (Matrix <size, size, Type> &A) {
		Vector <size, Type> P;

		for (int k = 0; k < size - 1; k++) {
			Type maxVal = abs(A(k, k));
			int ik = k;

			for (int i = k; i < size; i++)
				if (abs(A(i, k)) > maxVal)
					maxVal = abs(A(i, k)),
					ik = i;

			P(k) = ik;

			/// swap only in the superior triangle (not in the gaussian multipliers)
			for (int j = k; j < size; j++)
				std::swap(A(k, j), A(ik, j));

			gaussianEliminationKStep(A, k);
		}

		return P;
	}

	/** Gussian Elimination A - the matrix, will be modified and the transformations will be stored in A,
	* 							pivot = max(row(k)[i]), k <- i, size; (returns the pivots)
	*	U U U U U
	*	M U U U U
	*	M M U U U
	*	M M M U U
	*	M M M M U
	**/
	template <int size, typename Type>
	std::pair<Vector <size, Type>, Vector <size, Type>> completePivotGE (Matrix <size, size, Type> &A) {
		Vector <size, Type> P;
		Vector <size, Type> Q;

		for (int k = 0; k < size - 1; k++) {
			Type maxVal = abs(A(k, k));
			int ik = k;
			int jk = k;

			for (int i = k; i < size; i++)
				for (int j = k; j < size; j++)
					if (abs(A(i, j)) > maxVal)
						maxVal = abs(A(i, j)),
						ik = i, 
						jk = j;

			P(k) = ik;
			Q(k) = jk;

			/// swap only in the superior triangle (not in the gaussian multipliers)
			for (int j = k; j < size; j++)
				std::swap(A(k, j), A(ik, j));

			for (int i = k; i < size; i++)
				std::swap(A(i, k), A(i, jk));

			gaussianEliminationKStep(A, k);
		}

		return std::pair<Vector <size, Type>, Vector <size, Type>>(std::move(P), std::move(Q));
	}

	/**	Solve using partial Gaussian Elimination 
	*	Modifies A, B, returns solution
	**/
	template <int size, typename Type>
	Vector<size, Type> solvePartialGE (Matrix<size, size, Type> &A, Vector<size, Type> &B) {
		auto P = partialPivotGE(A);
		for (int k = 0; k < size - 1; k++) {
			// apply perm to B
			std::swap(B(k), B(P(k)));

			// apply M to B
			for (int i = k + 1; i < size; i++) {
				B(i) -= A(i, k) * B(k);
			}
		}
		return solveUTM(A, B);
	}

	/**	Solve using complete Gaussian Elimination 
	*	Modifies A, B, returns solution
	**/
	template <int size, typename Type>
	Vector<size, Type> solveCompleteGE (Matrix<size, size, Type> &A, Vector<size, Type> &B) {
		auto PQ = completePivotGE(A);
		for (int k = 0; k < size - 1; k++) {
			// apply perm P to B
			std::swap(B(k), B(PQ.first(k)));

			// apply M to B
			for (int i = k + 1; i < size; i++) {
				B(i) -= A(i, k) * B(k);
			}
		}
		auto X = solveUTM(A, B);
		for (int k = size  - 2; k >= 0; k--) {
			std::swap(X(k), X(PQ.second(k)));
		}
		return X;
	}

	/** Crout Factorization: A = LU
	*	A A A A ->	L U U U
	*	A A A A		L L U U 
	*	A A A A		L L L U
	*	A A A A		L L L L
	**/
	template <int size, typename Type>
	void factCrout (Matrix <size, size, Type>& A) {
		// for (int i = 0; i < size; ++i)
		// 		A(i, 0) = L(i, 0)

		for (int j = 1; j < size; ++j)
			A(0, j) /= A(0, 0);
		
		for (int k = 1; k < size; k++) {
			for (int i = k; i < size; i++) {
				Type sum = Type(0);
				
				for (int l = 0; l < k; l++)
					sum += A(i, l) * A(l, k);

				A(i, k) -= sum;
			}
			for (int j = k + 1; j < size; j++) { /// if (k != size - 1) // (size - 1) + 1 < size => false
				Type sum = Type(0);
			
				for (int l = 0; l < k; l++)
					sum += A(k, l) * A(l, j);

				A(k, j) -= sum;
				A(k, j) /= A(k, k);
			}
		}
	}

	/** Cholesky Factorization: A = L(L^T), also tels if a is positive definite
	*	A A A A	->	L A A A	
	*	A A A A		L L A A
	*	A A A A		L L L A
	*	A A A A		L L L L
	**/
	template <int size, typename Type, typename Sqrt_T = double(*)(double)>
	bool factCholesky (Matrix <size, size, Type>& A, Sqrt_T sqrt = std::sqrt) {
		if (A(0, 0) <= Type(0)) 
			return false;

		A(0, 0) = sqrt(A(0, 0));

		for (int i = 1; i < size; i++)
			A(i, 0) /= A(0, 0);

		for (int k = 1; k < size; k++) {
			Type sum = Type(0);
			for (int l = 0; l < k; l++)
				sum += A(k, l) * A(k, l);
			Type alfa = A(k, k) - sum;

			if (alfa <= Type(0))
				return false;

			A(k, k) = sqrt(alfa);
			for (int i = k + i; i < size; i++) {
				Type sum = Type(0);
				for (int l = 0; l < k; l++)
					sum += A(i, l) * A(k, l);
				A(i, k) = (A(i, k) - sum) / A(k, k);
			}
		}
		return true;
	}

	/// -------------------------------------------------------------------------------------------
	/// Bellow are rvalues + references

	/// solveLTM
	template <int size, typename Type>
	auto solveLTM (Matrix <size, size, Type> &&mat, Vector<size, Type> &&vec) {
		return solveLTM(mat, vec);
	}

	template <int size, typename Type>
	auto solveLTM (Matrix <size, size, Type> &&mat, Vector<size, Type> &vec) {
		return solveLTM(mat, vec);
	}

	template <int size, typename Type>
	auto solveLTM (Matrix <size, size, Type> &mat, Vector<size, Type> &&vec) {
		return solveLTM(mat, vec);
	}

	/// solveUTM
	template <int size, typename Type>
	auto solveUTM (Matrix <size, size, Type> &&mat, Vector<size, Type> &&vec) {
		return solveUTM(mat, vec);
	}

	template <int size, typename Type>
	auto solveUTM (Matrix <size, size, Type> &&mat, Vector<size, Type> &vec) {
		return solveUTM(mat, vec);
	}

	template <int size, typename Type>
	auto solveUTM (Matrix <size, size, Type> &mat, Vector<size, Type> &&vec) {
		return solveUTM(mat, vec);
	}

	/// gaussianElimination (no sense to be with rvalue)
	// template <int size, typename Type>
	// void gaussianElimination (Matrix <size, size, Type>&& A) {
	// 	gaussianElimination(A);
	// }

	/// partialPivotGE (no sense to be with rvalue)
	// template <int size, typename Type>
	// Vector <size, Type> partialPivotGE (Matrix <size, size, Type>&& A) {
	// 	return partialPivotGE(A);
	// }

	/// completePivotGE (no sense to be with rvalue)
	// template <int size, typename Type>
	// std::pair<Vector <size, Type>, Vector <size, Type>> completePivotGE (Matrix <size, size, Type>&& A) {
	// 	return completePivotGE(A);
	// }

	/// solvePartialGE
	template <int size, typename Type>
	auto solvePartialGE (Matrix<size, size, Type> &A, Vector<size, Type> &&B) {
		return solvePartialGE(A, B);
	}

	template <int size, typename Type>
	auto solvePartialGE (Matrix<size, size, Type> &&A, Vector<size, Type> &B) {
		return solvePartialGE(A, B);
	}

	template <int size, typename Type>
	auto solvePartialGE (Matrix<size, size, Type> &&A, Vector<size, Type> &&B) {
		return solvePartialGE(A, B);
	}

	/// solveCompleteGE
	template <int size, typename Type>
	auto solveCompleteGE (Matrix<size, size, Type> &A, Vector<size, Type> &&B) {
		return solveCompleteGE(A, B);
	}

	template <int size, typename Type>
	auto solveCompleteGE (Matrix<size, size, Type> &&A, Vector<size, Type> &B) {
		return solveCompleteGE(A, B);
	}

	template <int size, typename Type>
	auto solveCompleteGE (Matrix<size, size, Type> &&A, Vector<size, Type> &&B) {
		return solveCompleteGE(A, B);
	}

	// factCrout (no sense to be with rvalue)
	// template <int size, typename Type>
	// void factCrout (Matrix <size, size, Type>&& A) {
	// 	factCrout(A);
	// }

	// factCholesky (no sense to be with rvalue)
	// template <int size, typename Type>
	// void factCholesky (Matrix <size, size, Type>& A) {
	// 	factCholesky(A);
	// }
	
}

#endif