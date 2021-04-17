#ifndef MATRIX_H_INCLUDED
#define MATRIX_H_INCLUDED

#include <iostream>
#include <type_traits>
#include <vector>
#include <functional>
#include <cmath>

namespace Math {
	template <typename ToCheck>
	struct is_matrix {
		template <typename T, void (T::*)() const> struct SFINAE {};
		template <typename T> static char test (SFINAE<T, &T::MatrixIdentificator>*);
		template <typename T> static int test (...);
		constexpr const static bool value = sizeof(test<ToCheck>(0)) == sizeof(char);
	};

	constexpr static const int MAX_MATRIX_SIZE = 10000;

	template <int row_count, int column_count, typename Type, bool bigMatrix>
	class MatrixContainer {
	public:
		static_assert(row_count >= 1, "matrix must have at least one element");
		static_assert(column_count >= 1, "matrix must have at least one element");
		std::vector<Type> matrix;

		MatrixContainer() : matrix(row_count * column_count, Type(0)) {}

		Type *operator [] (int index) {
			return &matrix[0] + index * column_count;
		}

		const Type *operator [] (int index) const {
			return (const Type *)(&matrix[0] + index * column_count);
		}
	};

	template <int row_count, int column_count, typename Type>
	class MatrixContainer <row_count, column_count, Type, false> {
	public: 
		Type matrix[row_count][column_count];

		MatrixContainer() {
			for (int i = 0; i < row_count; i++)
				for (int j = 0; j < column_count; j++)
					matrix[i][j] = Type(0);
		}

		Type *operator [] (int index) {
			return matrix[index];
		}

		const Type *operator [] (int index) const {
			return (const Type *)matrix[index];
		}

		~MatrixContainer() {}
	};

	template <int rows, int cols, typename Type>
	class MatrixToVectorContainer {
	public:
		using MatCont = MatrixContainer<rows, cols, Type, ((rows * cols) > MAX_MATRIX_SIZE)>;
		MatCont matrix;

		MatrixToVectorContainer() : matrix() {}
		~MatrixToVectorContainer() {}
	};

	template <typename Type>
	struct MatrixToVectorContainer <1, 1, Type> { 
		using MatCont = MatrixContainer<1, 1, Type, ((1 * 1) > MAX_MATRIX_SIZE)>;
		union {
			MatCont matrix;
			union {
				Type x;
				Type r;
			};
		};

		MatrixToVectorContainer() : matrix() {}
		~MatrixToVectorContainer() {}
	};

	template <typename Type>
	struct MatrixToVectorContainer <2, 1, Type> { 
		using MatCont = MatrixContainer<2, 1, Type, ((2 * 1) > MAX_MATRIX_SIZE)>;
		union {
			MatCont matrix;
			struct {
				union {
					Type x;
					Type r;
				};
				union {
					Type y;
					Type g;
				};
			};
		};

		MatrixToVectorContainer() : matrix() {}
		~MatrixToVectorContainer() {}
	};

	template <typename Type>
	struct MatrixToVectorContainer <3, 1, Type> { 
		using MatCont = MatrixContainer<3, 1, Type, ((3 * 1) > MAX_MATRIX_SIZE)>;
		union {
			MatCont matrix;
			struct {
				union {
					Type x;
					Type r;
				};
				union {
					Type y;
					Type g;
				};
				union {
					Type z;
					Type b;
				};
			};
		};

		MatrixToVectorContainer() : matrix() {}
		~MatrixToVectorContainer() {}
	};

	template <typename Type>
	struct MatrixToVectorContainer <4, 1, Type> { 
		using MatCont = MatrixContainer<4, 1, Type, ((4 * 1) > MAX_MATRIX_SIZE)>;
		union {
			MatCont matrix;
			struct {
				union {
					Type x;
					Type r;
				};
				union {
					Type y;
					Type g;
				};
				union {
					Type z;
					Type b;
				};
				union {
					Type w;
					Type a;
				};
			};
		};

		MatrixToVectorContainer() : matrix() {}
		~MatrixToVectorContainer() {}
	};

	template <int row_count, int column_count, typename Type>
	class Matrix : public MatrixToVectorContainer <row_count, column_count, Type> {
	public:
		constexpr static const int rows = row_count;
		constexpr static const int cols = column_count;

		using MatCont = MatrixToVectorContainer <row_count, column_count, Type>;

		void MatrixIdentificator() const {};

		static Type sqrtFunc (Type x) {
			return std::sqrt(x);
		}

		static Type absFunc (Type x) {
			return std::abs(x);
		}

		using SqrtType = decltype(&Matrix::sqrtFunc);
		using AbsType = decltype(&Matrix::absFunc);

		~Matrix() {}

		template <typename TypeCols>
		static constexpr int get_col_number() {
			if constexpr (is_matrix<TypeCols>::value) {
				return TypeCols::cols;
			}
			else {
				return 1;
			}
		}

		/// Mathematical Stuff:
		class MatrixEpsilon {
		public:
			Type epsilon;

			MatrixEpsilon() {
				if constexpr (std::is_arithmetic<Type>::value) {
					epsilon = std::numeric_limits<Type>::epsilon();
				}
				else {
					epsilon = Type(0.00001f);
				}
			}

			template <typename Abs_T = AbsType>
			bool areEqual (Type arg1, Type arg2, Abs_T abs = absFunc) {
				return (abs(arg1 - arg2) < epsilon);
			}

			template <typename Abs_T = AbsType>
			bool isZero (Type arg1, Abs_T abs = absFunc) {
				return (abs(arg1 - Type(0)) < epsilon);
			}

			MatrixEpsilon (Type& epsilon) : epsilon(epsilon) {}
			MatrixEpsilon (Type&& epsilon) : epsilon(epsilon) {}
		};

		static MatrixEpsilon defaultEpsilon;

		template <typename Sqrt_T = SqrtType, typename Abs_T = AbsType>
		Type getFrobeniusNorm (Sqrt_T sqrt = sqrtFunc, Abs_T abs = absFunc) {
			Type result = Type(0);

			for (int i = 0; i < rows; i++)
				for (int j = 0; j < cols; j++)
					result += abs(MatCont::matrix[i][j]) * abs(MatCont::matrix[i][j]);

			return sqrt(result);
		}

		template <typename Sqrt_T = SqrtType, typename Abs_T = AbsType>
		Type norm2 (Sqrt_T sqrt = sqrtFunc, Abs_T abs = absFunc) {
			Type result = Type(0);

			for (int i = 0; i < rows; i++)
				for (int j = 0; j < cols; j++)
					result += MatCont::matrix[i][j] * MatCont::matrix[i][j];

			return sqrt(result);	
		}

		template <typename Abs_T = AbsType>
		Type norm1 (Abs_T abs = absFunc) {
			Type result = Type(0);

			for (int i = 0; i < rows; i++)
				for (int j = 0; j < cols; j++)
					result += abs(MatCont::matrix[i][j]);

			return result;	
		}

		template <typename Abs_T = AbsType>
		Type normInf (Abs_T abs = absFunc) {
			Type result = abs(MatCont::matrix[0][0]);

			for (int i = 0; i < rows; i++)
				for (int j = 0; j < cols; j++)
					if (result < abs(MatCont::matrix[i][j]))
						result = abs(MatCont::matrix[i][j]);

			return result;	
		}

		template <typename TypeArg>
		Matrix<3, 1, decltype(Type() * TypeArg())> cross (Matrix<3, 1, TypeArg>& arg) {
			Matrix <3, 1, decltype(Type() * TypeArg())> result;

			static_assert((3 == rows && cols == 1), "The vectors must be 3D to use cross");

			return Matrix <3, 1, decltype(Type() * TypeArg())> (
				MatCont::y * arg.z - MatCont::z * arg.y,
				MatCont::z * arg.x - MatCont::x * arg.z,
				MatCont::x * arg.y - MatCont::y * arg.x
			);
		}

		template <typename TypeArg>
		Matrix<3, 1, decltype(Type() * TypeArg())> cross (Matrix<3, 1, TypeArg>&& arg) {
			return cross(arg);
		}

		template <int rowsArg, typename TypeArg>
		decltype(Type() * TypeArg()) dot (const Matrix<rowsArg, 1, TypeArg>& arg) const {
			static_assert((rowsArg == rows && cols == 1), "The vectors must be equal to use dot");

			decltype(Type() * TypeArg()) result = 0;
			for (int i = 0; i < rows; i++)
				result += MatCont::matrix[i][0] * arg[i][0];

			return result;
		}

		template <int rowsArg, typename TypeArg>
		decltype(Type() * TypeArg()) dot (Matrix<rowsArg, 1, TypeArg>&& arg) const {
			return dot(arg);
		}

		template <typename Sqrt_T = SqrtType, typename Abs_T = AbsType>
		MatrixEpsilon getSugestedEpsilon(Sqrt_T sqrt = sqrtFunc, Abs_T abs = absFunc) {
			
			Type norm = getFrobeniusNorm(sqrt, abs);
			if (abs(norm) < 1)
				norm = 1;
			if constexpr (std::is_arithmetic<Type>::value) {
				return MatrixEpsilon(Type(std::numeric_limits<Type>::epsilon()) * norm);
			}
			else {
				return MatrixEpsilon(Type(0.00001f) *  norm);
			}
		}

		template <typename Abs_T = AbsType>
		Type det (MatrixEpsilon &epsilon = defaultEpsilon, Abs_T abs = absFunc) {
			static_assert((rows == cols), "need to have a square matrix to use determinant!");

			Type result = Type(1);
			Type sign = Type(1);
			auto temp = *this;
			for (int k = 0; k < rows; k++) {
				Type maxVal = abs(temp[k][k]);
				std::pair <int, int> pivot(k, k);

				for (int i = k; i < rows; i++)
					for (int j = k; j < cols; j++)
						if (abs(temp[i][j]) > maxVal)
							maxVal = abs(temp[i][j]),
							pivot = std::pair<int, int>(i, j);

				temp.swapLines(pivot.first, k);
				temp.swapColls(pivot.second, k);

				if (pivot.first != k)
					sign *= -1;

				if (pivot.second != k)
					sign *= -1;

				if (epsilon.isZero(temp[k][k]))
					return Type(0);
				
				for (int i = k + 1; i < cols; i++)
					temp[k][i] /= temp[k][k];

				result *= temp[k][k];
				temp[k][k] = Type(1);

				for (int i = k + 1; i < rows; i++) {
					for (int j = k + 1; j < cols; j++) {
						temp[i][j] -= temp[k][j] * temp[i][k];
					}
					temp[i][k] = 0;
				}
			}
			return result * sign;
		}

		void swapLines (int l1, int l2) {
			for (int i = 0; i < cols; i++)
				std::swap(row(l1)[i], row(l2)[i]);
		}

		void swapColls (int c1, int c2) {
			for (int i = 0; i < rows; i++)
				std::swap(coll(c1)[i], coll(c2)[i]);
		}

		/// Line, column indexers:
	// private:
		class LineIndexer {
		public:
			using MatType = Matrix<1, cols, Type>;
			
			Matrix <rows, cols, Type>& parentMatrix;
			int line;

		// public:
			LineIndexer(Matrix <rows, cols, Type>& parentMatrix, int line) 
			: parentMatrix(parentMatrix), line(line) {}
		
		// private:	
			LineIndexer (LineIndexer&& lineIndex) : parentMatrix(lineIndex.parentMatrix) {
				for (int i = 0; i < cols; i++)
					(*this)[i] = lineIndex[i];
			}

			LineIndexer (LineIndexer& lineIndex) : parentMatrix(lineIndex.parentMatrix) {
				for (int i = 0; i < cols; i++)
					(*this)[i] = lineIndex[i];
			}

			LineIndexer operator = (LineIndexer& lineIndex) {
				for (int i = 0; i < cols; i++)
					(*this)[i] = lineIndex[i];

				return (*this);
			}
			
			LineIndexer operator = (LineIndexer&& lineIndex) {
				for (int i = 0; i < cols; i++)
					(*this)[i] = lineIndex[i];

				return (*this);
			}

		// public:
			MatType getAsMatrix() {
				return MatType(*this);
			}

			operator MatType () {
				MatType result;

				for (int i = 0; i < cols; i++)
					result[0][i] = parentMatrix[line][i];

				return result;
			}

			Type& operator [] (int index) {
				return parentMatrix[line][index];
			}

			Type& operator () (int index) {
				return parentMatrix[line][index];
			}		
		};

		class CollumnIndexer {
		public:
			using MatType = Matrix<rows, 1, Type>;
			
			Matrix <rows, cols, Type>& parentMatrix;
			int collumn;

		// public:
			CollumnIndexer (Matrix <rows, cols, Type>& parentMatrix, int collumn) 
			: parentMatrix(parentMatrix), collumn(collumn) {}

		// private:
			CollumnIndexer (CollumnIndexer&& colIndex) : parentMatrix(colIndex.parentMatrix) {
				for (int i = 0; i < rows; i++)
					(*this)[i] = colIndex[i];
			}

			CollumnIndexer (CollumnIndexer& colIndex) : parentMatrix(colIndex.parentMatrix) {
				for (int i = 0; i < rows; i++)
					(*this)[i] = colIndex[i];
			}

			CollumnIndexer operator = (CollumnIndexer& colIndex) {
				for (int i = 0; i < rows; i++)
					(*this)[i] = colIndex[i];

				return (*this);
			}

			CollumnIndexer operator = (CollumnIndexer&& colIndex) {
				for (int i = 0; i < rows; i++)
					(*this)[i] = colIndex[i];

				return (*this);
			}

		// public:
			MatType getAsMatrix() {
				return MatType(*this);
			}

			operator MatType () {
				MatType result;

				for (int i = 0; i < rows; i++)
					result[i][0] = parentMatrix[i][collumn];

				return result;
			}

			Type& operator [] (int index) {
				return parentMatrix[index][collumn];
			}

			Type& operator () (int index) {
				return parentMatrix[index][collumn];
			}
		};

	// public:
		friend LineIndexer;
		friend CollumnIndexer;

		LineIndexer row(int line) {
			return LineIndexer(*this, line);
		}

		CollumnIndexer coll(int coll) {
			return CollumnIndexer(*this, coll);
		}

		Type& operator () (int lin, int col) {
			return MatCont::matrix[lin][col];
		}

		Type& operator () (int index) {
			static_assert (cols == 1, "Can use this operator only on vectors");
			return MatCont::matrix[index][0];
		}

		/// main operators
		template <int rowsB, int colsB, typename TypeB>
		Matrix<rows, cols, decltype(Type() + TypeB())>& operator = (const Matrix<rowsB, colsB, TypeB>& mat) {
			static_assert((colsB <= cols && rowsB <= rows), "Cannot equal, sizes don't match");

			for (int i = 0; i < rows; i++)
				for (int j = 0; j < cols; j++)
					MatCont::matrix[i][j] = mat[i][j];

			return (*this);
		}

		template <int rowsB, int colsB, typename TypeB>
		Matrix<rows, cols, decltype(Type() + TypeB())>& operator = (Matrix<rowsB, colsB, TypeB>&& mat) {
			static_assert((colsB <= cols && rowsB <= rows), "Cannot equal, sizes don't match");

			for (int i = 0; i < rows; i++)
				for (int j = 0; j < cols; j++)
					MatCont::matrix[i][j] = mat[i][j];

			return (*this);
		}

		template <int rowsB, int colsB, typename TypeB>
		Matrix<rows, cols, decltype(Type() + TypeB())>
		operator + (const Matrix<rowsB, colsB, TypeB>& mat) const {
			Matrix <rows, cols, decltype(Type() + TypeB())> result;

			static_assert((cols == colsB && rows == rowsB), "Cannot add, sizes don't match");

			for (int i = 0; i < rows; i++)
				for (int j = 0; j < cols; j++)
					result[i][j] = MatCont::matrix[i][j] + mat[i][j];

			return result;
		}

		template <int rowsB, int colsB, typename TypeB>
		Matrix<rows, cols, decltype(Type() - TypeB())>
		operator - (const Matrix<rowsB, colsB, TypeB>& mat) const {
			Matrix <rows, cols, decltype(Type() - TypeB())> result;

			static_assert((cols == colsB && rows == rowsB), "Cannot substract, sizes don't match");

			for (int i = 0; i < rows; i++)
				for (int j = 0; j < cols; j++)
					result[i][j] = MatCont::matrix[i][j] - mat[i][j];

			return result;
		}

		template <int rowsB, int colsB, typename TypeB>
		Matrix<rows, colsB, decltype(Type() * TypeB())>
		operator * (const Matrix<rowsB, colsB, TypeB>& mat) const {
			Matrix <rows, colsB, decltype(Type() * TypeB())> result;

			static_assert((cols == rowsB), "Cannot multiply, sizes don't match");

			for (int i = 0; i < rows; i++)
				for (int j = 0; j < colsB; j++)
					for (int k = 0; k < cols; k++)
						result[i][j] += MatCont::matrix[i][k] * mat[k][j];

			return result;
		}

		Matrix<rows, cols, Type> operator - ()  const {
			return (*this) * Type(-1);
		}
		
		template <typename ScalarType>
		typename std::enable_if<!is_matrix<ScalarType>::value, 
				Matrix<rows, cols, decltype(Type() * ScalarType())>>::type
		operator * (const ScalarType& scalar) const {
			Matrix<rows, cols, decltype(Type() * ScalarType())> result;
				
			for (int i = 0; i < rows; i++)
				for (int j = 0; j < cols; j++)
					result[i][j] = MatCont::matrix[i][j] * scalar;

			return result;
		}

		template <typename ScalarType>
		typename std::enable_if<!is_matrix<ScalarType>::value, 
				Matrix<rows, cols, decltype(Type() / ScalarType())>>::type
		operator / (const ScalarType& scalar) const {
			Matrix<rows, cols, decltype(Type() / ScalarType())> result;
			
			for (int i = 0; i < rows; i++)
				for (int j = 0; j < cols; j++)
					result[i][j] = MatCont::matrix[i][j] / scalar;

			return result;
		}

		/// friends, references and rvalues
		template <typename ScalarType>
		friend typename std::enable_if<!is_matrix<ScalarType>::value, 
				Matrix<rows, cols, decltype(Type() * ScalarType())>>::type
		operator * (const ScalarType& scalar, const Matrix<rows, cols, decltype(Type() * ScalarType())>& mat) 
		{
			return (mat * scalar);
		}

		template <typename ScalarType>
		friend typename std::enable_if<!is_matrix<ScalarType>::value, 
				Matrix<rows, cols, decltype(Type() * ScalarType())>>::type
		operator * (const ScalarType& scalar, const Matrix<rows, cols, decltype(Type() * ScalarType())>&& mat) 
		{
			return (mat * scalar);
		}

		template <typename ScalarType>
		friend typename std::enable_if<!is_matrix<ScalarType>::value, 
				Matrix<rows, cols, decltype(Type() * ScalarType())>>::type
		operator * (const ScalarType&& scalar, const Matrix<rows, cols, decltype(Type() * ScalarType())>& mat) 
		{
			return (mat * scalar);
		}

		template <typename ScalarType>
		friend typename std::enable_if<!is_matrix<ScalarType>::value, 
				Matrix<rows, cols, decltype(Type() * ScalarType())>>::type
		operator * (const ScalarType&& scalar, const Matrix<rows, cols, decltype(Type() * ScalarType())>&& mat) 
		{
			return (mat * scalar);
		}

		template <typename ScalarType>
		typename std::enable_if<!is_matrix<ScalarType>::value, 
				Matrix<rows, cols, decltype(Type() / ScalarType())>>::type
		operator / (const ScalarType&& scalar) const {
			return (*this / scalar);
		}

		template <typename ScalarType>
		typename std::enable_if<!is_matrix<ScalarType>::value, 
				Matrix<rows, cols, decltype(Type() * ScalarType())>>::type
		operator * (const ScalarType&& scalar) const {
			return (*this * scalar);
		}

		template <int rowsB, int colsB, typename TypeB>
		Matrix<rows, colsB, decltype(Type() * TypeB())>
		operator * (const Matrix<rowsB, colsB, TypeB>&& mat) const {
			return (*this * mat);
		}

		template <int rowsB, int colsB, typename TypeB>
		Matrix<rows, cols, decltype(Type() - TypeB())>
		operator - (const Matrix<rowsB, colsB, TypeB>&& mat) const {
			return (*this - mat);
		}

		template <int rowsB, int colsB, typename TypeB>
		Matrix<rows, cols, decltype(Type() + TypeB())>
		operator + (const Matrix<rowsB, colsB, TypeB>&& mat) const {
			return (*this + mat);
		}
		
		/// operator <something>=
		template <int rowsB, int colsB, typename TypeB>
		Matrix<rows, cols, decltype(Type() + TypeB())>
		operator += (const Matrix<rowsB, colsB, TypeB>& mat) {
			return ((*this) = (*this) + mat);
		}
		
		template <int rowsB, int colsB, typename TypeB>
		Matrix<rows, cols, decltype(Type() - TypeB())>
		operator -= (const Matrix<rowsB, colsB, TypeB>& mat) {
			return ((*this) = (*this) - mat);
		}
		
		template <int rowsB, int colsB, typename TypeB>
		Matrix<rows, colsB, decltype(Type() * TypeB())>
		operator *= (const Matrix<rowsB, colsB, TypeB>& mat) {
			return ((*this) = (*this) * mat);
		}

		template <typename ScalarType>
		typename std::enable_if<!is_matrix<ScalarType>::value, 
				Matrix<rows, cols, decltype(Type() * ScalarType())>>::type
		operator *= (const ScalarType& scalar) {
			return ((*this) = (*this) * scalar);
		}

		template <typename ScalarType>
		typename std::enable_if<!is_matrix<ScalarType>::value, 
				Matrix<rows, cols, decltype(Type() / ScalarType())>>::type
		operator /= (const ScalarType& scalar) {
			return ((*this) = (*this) / scalar);
		}

		template <int rowsB, int colsB, typename TypeB>
		bool operator == (const Matrix<rowsB, colsB, TypeB>& mat) const {
			if (row_count != rowsB || column_count != colsB)
				return false;
			for (int i = 0; i < rowsB; i++)
				for (int j = 0; j < colsB; j++)
					if ((*this)[i][j] != mat[i][j])
						return false;
			return true;
		}

		template <int rowsB, int colsB, typename TypeB>
		bool operator < (const Matrix<rowsB, colsB, TypeB>& mat) const {
			if (row_count != rowsB || column_count != colsB)
				return false;
			for (int i = 0; i < rowsB; i++)
				for (int j = 0; j < colsB; j++) {
					if ((*this)[i][j] < mat[i][j])
						return true;
					if ((*this)[i][j] > mat[i][j])
						return false;
				}
			return false;
		}

		Type* operator [] (int index) {
			return MatCont::matrix[index];
		}

		const Type* operator [] (int index) const {
			return MatCont::matrix[index];
		}

		Matrix<cols, rows, Type> tr() const {
			Matrix<cols, rows, Type> result;

			for (int i = 0; i < rows; i++)
				for (int j = 0; j < cols; j++)
					result[j][i] = MatCont::matrix[i][j];

			return result;
		}

		Type *getPtr() {
			return &MatCont::matrix[0][0];
		}

		const Type *getPtr() const {
			return &MatCont::matrix[0][0];
		}

		// The matrix will be constructed as follows: 
		// -> we will consider basic types to be a matrix of 1 X 1
		// -> we will take the first matrix and place it at (0, 0)
		// -> if the second matrix has space on the right of the first matrix then we place it on the right
		// -> else we jump on the line under the matrix we just placed at collumn 0 and 
		// continue placing the next matrix from there

		Matrix () {}

		template <typename ArgType>
		Matrix (ArgType *array) {
			for (int i = 0; i < rows; i++)
				for (int j = 0; j < cols; j++)
					MatCont::matrix[i][j] = array[i * cols + j];
		}

		template <typename ArgType>
		Matrix (ArgType array[][cols]) {
			for (int i = 0; i < rows; i++)
				for (int j = 0; j < cols; j++)
					MatCont::matrix[i][j] = array[i][j];
		}

		template <typename ArgType, typename ...Args>
		Matrix (const ArgType& arg, Args ...args) {
			fill_mat <0, 0, is_matrix<ArgType>::value, ArgType, Args...> (arg, args...);
		}

		template <typename ArgType, typename ...Args>
		Matrix (const ArgType&& arg, Args ...args) {
			fill_mat <0, 0, is_matrix<ArgType>::value, ArgType, Args...> (arg, args...);
		}

		// I want to modify this function to be more general
		// for now I will leave it be
		template <int lin, int col, bool is_matrix_val, typename ArgType, typename NextType, typename... Args>
		typename std::enable_if<!is_matrix_val, void>::type fill_mat(const ArgType& arg, const NextType& nextArg, Args ...args) {
			if constexpr (col + 1 <= cols && lin + 1 <= rows) {
				MatCont::matrix[lin][col] = arg;
			}

			if constexpr (col + 1 + get_col_number<NextType>() <= cols)	// we have space for the next matrix
				fill_mat <lin, col + 1, is_matrix<NextType>::value, NextType, Args...> (nextArg, args...);
			else { 														// we don't have space for the matrix
				fill_mat <lin + 1, 0, is_matrix<NextType>::value, NextType, Args...> (nextArg, args...);
			}
		}

		template <int lin, int col, bool is_matrix_val, typename ArgType, typename NextType, typename... Args>
		typename std::enable_if<is_matrix_val, void>::type fill_mat(const ArgType& arg, const NextType& nextArg, Args ...args) {
			if constexpr (col + ArgType::cols <= cols && lin + ArgType::rows <= rows) {
				for (int i = 0; i < ArgType::rows; i++)
					for (int j = 0; j < ArgType::cols; j++)
						MatCont::matrix[lin + i][col + j] = arg[i][j];
			}

			if constexpr (col + ArgType::cols + get_col_number<NextType>() <= cols) {
				fill_mat <lin, col + ArgType::cols, is_matrix<NextType>::value, NextType, Args...> (nextArg, args...);
			}
			else { 												// we don't have space for the matrix
				fill_mat <lin + ArgType::rows, 0, is_matrix<NextType>::value, NextType, Args...> (nextArg, args...);
			}
		}

		template <int lin, int col, bool is_matrix_val, typename ArgType, typename NextType>
		typename std::enable_if<!is_matrix_val, void>::type fill_mat(const ArgType& arg, const NextType& nextArg) {
			if constexpr (col + 1 <= cols && lin + 1 <= rows) {
				MatCont::matrix[lin][col] = arg;
			}

			if constexpr (col + 1 + get_col_number<NextType>() <= cols) {
				fill_mat <lin, col + 1, is_matrix<NextType>::value, NextType> (nextArg);
			}
			else { 												// we don't have space for the matrix
				fill_mat <lin + 1, 0, is_matrix<NextType>::value, NextType> (nextArg);
			}
		}

		template <int lin, int col, bool is_matrix_val, typename ArgType, typename NextType>
		typename std::enable_if<is_matrix_val, void>::type fill_mat(const ArgType& arg, const NextType& nextArg) {
			if constexpr (col + ArgType::cols <= cols && lin + ArgType::rows <= rows) {
				for (int i = 0; i < ArgType::rows; i++)
					for (int j = 0; j < ArgType::cols; j++)
						MatCont::matrix[lin + i][col + j] = arg[i][j];
			}

			if constexpr (col + ArgType::cols + get_col_number<NextType>() <= cols) {
				fill_mat <lin, col + ArgType::cols, is_matrix<NextType>::value, NextType> (nextArg);
			}
			else { 												// we don't have space for the matrix
				fill_mat <lin + ArgType::rows, 0, is_matrix<NextType>::value, NextType> (nextArg);
			}
		}

		template <int lin, int col, bool is_matrix_val, typename ArgType>
		typename std::enable_if<!is_matrix_val, void>::type fill_mat(const ArgType& arg) {
			if constexpr (col + 1 <= cols && lin + 1 <= rows) {
				MatCont::matrix[lin][col] = arg;
			}
		}

		template <int lin, int col, bool is_matrix_val, typename ArgType>
		typename std::enable_if<is_matrix_val, void>::type fill_mat(const ArgType& arg) {
			if constexpr (col + ArgType::cols <= cols && lin + ArgType::rows <= rows) {
				for (int i = 0; i < ArgType::rows; i++)
					for (int j = 0; j < ArgType::cols; j++)
						MatCont::matrix[lin + i][col + j] = arg[i][j];
			}
		}

		/// ostream, istream:
		friend std::ostream& operator << (std::ostream& stream, const Matrix<rows, cols, Type>& arg) {
			for (int i = 0; i < rows; i++) {
				for (int j = 0; j < cols; j++) {
					stream << arg[i][j] << " ";
				}
				if (i < rows - 1)
					stream << std::endl;
			}
			
			return stream;
		}

		friend std::ostream& operator << (std::ostream& stream, Matrix<rows, cols, Type>&& arg) {
			for (int i = 0; i < rows; i++) {
				for (int j = 0; j < cols; j++) {
					stream << arg[i][j] << " ";
				}
				if (i < rows - 1)
					stream << std::endl;
			}
			
			return stream;
		}

		friend std::istream& operator >> (std::istream& stream, Matrix<rows, cols, Type>& arg) {
			for (int i = 0; i < rows; i++) {
				for (int j = 0; j < cols; j++) {
					stream >> arg[i][j];
				}
			}
			
			return stream;
		}
	};

	template <int rows, int cols, typename Type>
	typename Matrix<rows, cols, Type>::MatrixEpsilon Matrix<rows, cols, Type>::defaultEpsilon;
}
#endif
