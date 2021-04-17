#include <iostream>
#include "MathLib.h"
#include "neunet.h"

//Test


void testVMat();
void testBasic();
int testBasicOld();

double eye(double x) {
	return x;
}

int main(int argc, char const *argv[])
{
	using namespace Math;
	using namespace std;

	Neunet nn;
	nn.add_layer(eye, 3,3);
	nn.add_layer(eye, 3,3);

	Math::VMatrix x = Math::VMatrix<double> (3);
	auto y = nn.feedforward(x);

	//std::cout << "x=\n" << x << std::endl;
	//std::cout << "y=\n" << y << std::endl;

	testVMat();
	// testBasic();
	// testBasicOld();
}



void testVMat() {
	using namespace Math;

	/*
	const	VMatrix<float>			_0;
	const	VMatrix<int>			_1;
	const	VMatrix<VMatrix<float>>	_2;
	const	VMatrix<float>			_3(_0);
	const	VMatrix<float>			_4(_1);
	const	VMatrix<float>			_5(10);
			VMatrix<float>			_6(10, 10);
			VMatrix<int>			_6a(10, 10);
			VMatrix<float>			_6b(10, 10);
	const	VMatrix<float>			_7(10, 10, 3);
	const	VMatrix<float>			_9(_6);
	*/

	VMatrix<float> A({
			{1, 4, 6}
	
		});

	VMatrix<float> B({
			{8, 8, 3}
		
		});

	
	VMatrix<float> C;

	//std::cout << "C: " << std::endl;

	//C = B * A;


	std::cout << B.tr().norm_inf() << std::endl;

	

	// if (sizeof(_27) < 2)
	// 	std::cin >> _27;
	// std::cout << _27 << std::endl;

	// _26.do_inv();
	// _26.do_tr();
	// _26.swap_rows(2, 3);
	// _26.swap_cols(2, 3);
	// _14.insert_rows(0, 2, _6);
	// _15.insert_cols(0, 2, _6);

	// _6.inv();
	// _6.det();
	// _6.tr();
	// _6.partition(1, 1, 3, 3);
	// _6.size();

	// _5.dot();
	// _5.norm1();
	// _5.norm2();
	// _5.norm_inf();

}

void testBasic() {
	using namespace Math;

	float vec[16];
	for (int i = 0; i < 16; ++i) {
		vec[i] = i;
	}

	Matrix<4, 4, float> matFromPtr(vec);
	std::cout << "matFromPtr: " << matFromPtr << std::endl;

	std::cout << "mat * scal" << Mat2f(1, 2) * 2 << " "
			<< 2 * Mat2f(2, 1) << std::endl;

	double mat[4][4];
	for (int i = 0; i < 16; ++i) {
		mat[i / 4][i % 4] = i;
	}
	std::cout << "Mat from mat ptr " << Mat4f(mat) << std::endl;
	std::cout << "Composed matrix: " <<
	 	Mat4f(
	 		Mat2f({11, 21, 31, 41}), Mat2f(10, 20, 30, 40),
	 		Mat2f(12, 22, 32, 42), Mat2f(13, 23, 33, 43)) << std::endl;

	Mat4f mat1(
		1, 0, 0, 0,
		0, 2, 0, 0,
		0, 0, 2, 0,
		0, 0, 0, 1);

	Vec4f vec1(0, 1, 0, 0);
	Vec4f vec2(0, 1, 1, 0);

	Mat4f mat2 = mat1;

	std::cout << vec1.x << std::endl;
	std::cout << vec1.y << std::endl;
	std::cout << vec1.z << std::endl;
	
	std::cout << mat1 << std::endl;
	std::cout << mat2 << std::endl;

	std::cout << vec1 * vec2.tr() << std::endl;
	std::cout << vec2.tr() * vec1 << std::endl;

	std::cout << "Ident:" << std::endl;
	Mat4f testMat(
		1, Matrix<1, 3, float>(),
		2, Matrix<1, 3, int>(),
		3, Matrix<1, 3, double>(),
		4, Matrix<1, 3, float>());
	std::cout << testMat << std::endl;

	testMat = identity<4, int>();
	std::cout << testMat << std::endl;
	std::cout << rot4<float>(30, 0, 1, 0) << std::endl;

	std::cout << Mat3f(Mat2f(1, 2, 3, 4), 5, 6, 7, 8, 9) << std::endl;
	float val = 10;
	auto mMat = Mat3f(
		1, 0, 0,
		0, 1, 0,
		0, 0, 1
	);
	std::cout << val * mMat * val << std::endl;
	std::cout << Math::is_matrix<decltype(mMat)>::value << std::endl;
	std::cout << Math::is_matrix<float>::value << std::endl;
}

int testBasicOld() {
	using namespace Math;
	using namespace std;

	Matf <1000, 20> testDim;

	// std::cout << testDim << std::endl;

	Vecf <3> vec1 (1, 0, 0);
	Vecf <3> vec2 (0, 2, 0);
	Vecf <3> vec3 (0, 0, 4);

	Matf <3, 3> test(vec1, vec2, vec3);
	Matd <1, 1> test2(1.2f);
	Mati <1, 1> test3(1.2f);

	Vecf <3> vec21 (2, 2, 2);
	
	std::cout << test << std::endl;
	std::cout << test2 << std::endl;
	std::cout << test3 << std::endl;
	std::cout << "-----------------" << std::endl;
	
	std::cout << vec21 << std::endl;
	std::cout << "-----------------" << std::endl;
	std::cout << test * vec21 << std::endl;
	std::cout << "-----------------" << std::endl;
	std::cout << test * 4 << std::endl;
	std::cout << "-----------------" << std::endl;
	std::cout << test.row(0)[0] << std::endl;

	auto line = test.row(2);

	std::cout << test.row(0).getAsMatrix() << std::endl;
	std::cout << test.coll(0).getAsMatrix() << std::endl;

	std::cout << test << std::endl;

	test.swapLines(0, 2);

	std::cout << test << std::endl;

	std::cout << test.det() << std::endl;

	std::cout << Math::Matf<3, 3> (
		1, 2, 3,
		4, 5, 6,
		7, 8, 9
	).det() << std::endl;

	std::cout << Matrix<3, 3, float> (
		1, 0, 0,
		0, 5, 0,
		0, 0, 1
	).det() << std::endl;

	std::cout << Math::Matf<3, 3> (
		1, 2, 3,
		4, 5, 6,
		7, 10, 9
	).det() << std::endl;

	std::cout << Math::Matf<4, 4> (
		1, 0, 2, -1, 
		3, 0, 0, 5,
		2, 1, 4, -3, 
		1, 0, 5, 0
	).det() << std::endl;

	std::cout << ";;;;;;;;;;;;;;;;;;;;;" << std::endl;

	Matrix<4, 4, float> mat(
		1, 0, 0, 0,
		0, 5, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	);

	Vector<4, float> vec(1, 2, 3, 4);

	std::cout << "LLLLLLLLLLLLLLLLLLLL" << std::endl;
	std::cout << mat << std::endl;	
	std::cout << "VVVVVVVVVVVVVVVVVVVV" << std::endl;
	std::cout << vec << std::endl;
	std::cout << ";;;;;;;;;;;;;;;;;;;;;" << std::endl;

	auto res = (mat * vec);

	std::cout << " ??? " << res << std::endl;

	Matrix <2, 2, float> smallMat (1, 2, 3, 4);

	Matrix <4, 4, float> bigMat (smallMat, smallMat, smallMat, smallMat);

	std::cout << bigMat << std::endl;

	Vector <3, float> vecPart (10, 20, 30);

	std::cout << sizeof (vecPart) / sizeof(float) << std::endl;
	std::cout << vecPart.x << std::endl;
	std::cout << vecPart.y << std::endl;
	std::cout << vecPart.z << std::endl;

	std::cout << "sizeof: " << std::endl;
	std::cout << sizeof (Matrix <2, 2, double>) << std::endl;

	std::cout << vecPart(0) << std::endl;
	std::cout << bigMat(1, 1) << std::endl;

	cout << " DDDDDDDDDDDDDDDDDDDDDDDDD " << endl;

	Math::Matf<4, 4> gMat(
		1, 0, 2, -1, 
		3, 0, 0, 5,
		2, 1, 4, -3, 
		1, 0, 5, 0
	);

	cout << gMat << endl;
	cout << "Det: " << gMat.det() << endl;

	completePivotGE(gMat);

	cout << gMat << endl;

	return 0;
}