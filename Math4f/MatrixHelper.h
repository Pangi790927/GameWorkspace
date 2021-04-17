#ifndef MATRIX_HELPER_H
#define MATRIX_HELPER_H

#include "Matrix.h"

namespace Math {
	template <int size, typename Type>
	using Vector = Matrix <size, 1, Type>;

	template <int x, int y>
	using Matd = Matrix <x, y, double>;

	template <int x, int y>
	using Matf = Matrix <x, y, float>;

	template <int x, int y>
	using Mati = Matrix <x, y, int>;

	template <int x>
	using Vecd = Vector <x, double>;

	template <int x>
	using Vecf = Vector <x, float>;

	template <int x>
	using Veci = Vector <x, int>;

	using Vec3d = Vecd<3>;
	using Vec3f = Vecf<3>;
	using Vec3i = Veci<3>;

	using Point3d = Vec3d;
	using Point3f = Vec3f;
	using Point3i = Vec3i;

	using Vec4d = Vecd<4>;
	using Vec4f = Vecf<4>;
	using Vec4i = Veci<4>;

	using Point4d = Vec4d;
	using Point4f = Vec4f;
	using Point4i = Vec4i;

	using Vec2d = Vecd<2>;
	using Vec2f = Vecf<2>;
	using Vec2i = Veci<2>;

	using Point2d = Vec2d;
	using Point2f = Vec2f;
	using Point2i = Vec2i;

	using Mat2d = Matd<2, 2>;
	using Mat2f = Matf<2, 2>;
	using Mat2i = Mati<2, 2>;

	using Mat3d = Matd<3, 3>;
	using Mat3f = Matf<3, 3>;
	using Mat3i = Mati<3, 3>;

	using Mat4d = Matd<4, 4>;
	using Mat4f = Matf<4, 4>;
	using Mat4i = Mati<4, 4>;

	template <typename Type>
	using Vec2 = Vector<2, Type>;

	template <typename Type>
	using Vec3 = Vector<3, Type>;

	template <typename Type>
	using Vec4 = Vector<4, Type>;

	template <typename Type>
	using Mat2 = Matrix<2, 2, Type>;

	template <typename Type>
	using Mat3 = Matrix<3, 3, Type>;

	template <typename Type>
	using Mat4 = Matrix<4, 4, Type>;

	template <int rows, typename Type>
	Matrix <rows, rows, Type> identity () {
		Matrix <rows, rows, Type> mat;

		for (int i = 0; i < rows; i++)
			mat[i][i] = Type(1);

		return mat;
	}

	template <typename Type,
			typename SinType = double(*)(double),
			typename CosType = double(*)(double),
			typename A = float, typename B = float, typename C = float,
			typename D = float, typename E = float>
	Mat4<Type> projection (A yFov, B aspect, C zNear, D zFar,
			E pi = 3.141592653589, SinType sin = std::sin,
			CosType cos = std::cos)
	{
		// TO DO
		auto ctan = [&] (Type x) -> Type {
	        return cos(x) / sin(x);
	    };

		float f = ctan(yFov * pi / 180.0 / 2.0);

		float firstExpresion = (zFar + zNear) / (zNear - zFar);
		float secondExpresion = 2.0f * zFar * zNear / (zNear - zFar);

		return Mat4<Type>( 
			f / aspect,	0.0f,	0.0f,			0.0f,
			0.0f,		f,		0.0f,			0.0f,
			0.0f,		0.0f,	firstExpresion,	secondExpresion,
			0.0f,		0.0f,	-1.0f,			0.0f               
		);
	}

	template <typename Type, typename SinType = double(*)(double),
			typename CosType = double(*)(double),
			typename A = float, typename B = float,
			typename C = float, typename D = float>
	Mat3<Type> rot3 (A deg, B x, C y, D z,
			Type pi = 3.141592653589, SinType sin = std::sin,
			CosType cos = std::cos)
	{
		Type co = cos(deg * pi / 180.0f);
		Type si = sin(deg * pi / 180.0f);
		return Mat3<Type>(
			x * x * (1.0f - co) + co,
			x * y * (1.0f - co) + z * si,
			x * z * (1.0f - co) - y * si,

			y * x * (1.0f - co) - z * si,
			y * y * (1.0f - co) + co,
			y * z * (1.0f - co) + x * si,
			
			z * x * (1.0f - co) + y * si,
			z * y * (1.0f - co) - x * si,
			z * z * (1.0f - co) + co
		);
	}

	template <typename Type, typename SinType = double(*)(double),
			typename CosType = double(*)(double),
			typename A = float, typename B = float>
	Mat3<Type> rot3 (A deg, const Vec3<B>& vec,
			Type pi = 3.141592653589, SinType sin = std::sin,
			CosType cos = std::cos)
	{
		return rot3(deg, vec.x, vec.y, vec.z, pi, sin, cos);
	}

	template <typename Type, typename SinType = double(*)(double),
			typename CosType = double(*)(double),
			typename A = float, typename B = float,
			typename C = float, typename D = float>
	Mat4<Type> rot4 (A deg, B x, C y, D z,
			Type pi = 3.141592653589, SinType sin = std::sin,
			CosType cos = std::cos)
	{
		return Mat4<Type>(
			rot3(deg, x, y, z, pi, sin, cos),	Vec3<Type>(),
			Vec3<Type>().tr(),					1);
	}

	template <typename Type, typename SinType = double(*)(double),
			typename CosType = double(*)(double),
			typename A = float, typename B = float>
	Mat4<Type> rot4 (A deg, const Vec3<B>& vec,
			Type pi = 3.141592653589, SinType sin = std::sin,
			CosType cos = std::cos)
	{
		return Mat4<Type>(
			rot3(deg, vec, pi, sin, cos),	Vec3<Type>(),
			Vec3<Type>().tr(),				1);
	}

	template <typename Type,
			typename A = float, typename B = float, typename C = float>
	Mat4<Type> translation (A x, B y, C z) {
		return Mat4<Type>(
			1, 0, 0, x,
			0, 1, 0, y,
			0, 0, 1, z,
			0, 0, 0, 1
		);
	}

	template <typename Type, typename A = float>
	Mat4<Type> translation (const Vec3<A>& vec) {
		return translation<Type>(vec.x, vec.y, vec.z);
	}

	template <typename Type,
			typename A = float, typename B = float, typename C = float>
	Mat3<Type> scale (A x = 0, B y = 0, C z = 0) {
		return Mat3<Type> (
			x, 0, 0,
			0, y, 0,
			0, 0, z
		);
	}

	template <typename Type, typename A>
	Mat3<Type> scale (const Vec3<A>& vec) {
		return scale(vec.x, vec.y, vec.z);
	}

	template <typename Type,
			typename A = float, typename B = float, typename C = float>
	Mat4<Type> scale4 (A x = 0, B y = 0, C z = 0) {
		return Mat4<Type> (
			x, 0, 0, 0,
			0, y, 0, 0,
			0, 0, z, 0,
			0, 0, 0, 1
		);
	}

	template <typename Type, typename A = float>
	Mat4<Type> scale4 (const Vec3<A>& vec) {
		return scale4<Type>(vec.x, vec.y, vec.z);
	}

	template <int resCol, int resRow, typename Type, int col, int row>
	Matrix<resRow, resCol, Type>
	trunc (const Matrix<row, col, Type>& mat) {
		using ResType = Matrix<resRow, resCol, Type>;
		ResType res;

		for (int i = 0; i < resRow && i < row; i++)
			for (int j = 0; j < resCol && j < col; j++)
				res[i][j] = mat[i][j];

		return res;
	}

	template <typename MatType, typename Type, int col, int row>
	MatType
	trunc (const Matrix<row, col, Type>& mat) {
		using ResType = MatType;
		ResType res;

		for (int i = 0; i < MatType::rows && i < row; i++)
			for (int j = 0; j < MatType::cols && j < col; j++)
				res[i][j] = mat[i][j];

		return res;
	}

	template <typename Type>
	Mat4<Type>
	rigidTransformInverse (const Mat4<Type>& mat) {
		return Mat4<Type> (
			trunc<3, 3>(mat).tr(),
			-trunc<3, 3>(mat).tr() * Vec3<Type>(mat[0][3], mat[1][3], mat[2][3]),
			Vec4<Type>(0, 0, 0, 1).tr()
		);
	}
}

#endif