#include "Matrix2x2.h"

#include"Vector2.h"
#include"Math.hpp"

//void Matrix2x2::Printf(const int& x, const int& y) {
//	for (int i = 0; i < 2; i++) {
//		for (int j = 0; j < 2; j++) {
//			//Novice::ScreenPrintf(x + j * TextWidth, y + i * TextHeight, "%6.2f", m[i][j]);
//		}
//	}
//}

Matrix2x2 Matrix2x2::operator+(const Matrix2x2& Second) const {
	return (Matrix2x2{
		this->m[0][0] + Second.m[0][0],
		this->m[0][1] + Second.m[0][1],
		this->m[1][0] + Second.m[1][0],
		this->m[1][1] + Second.m[1][1]
		});
}

Matrix2x2 Matrix2x2::operator-(const Matrix2x2& Second) const {
	return (Matrix2x2{
		this->m[0][0] - Second.m[0][0],
		this->m[0][1] - Second.m[0][1],
		this->m[1][0] - Second.m[1][0],
		this->m[1][1] - Second.m[1][1]
		});
}

Matrix2x2 Matrix2x2::operator*(const Matrix2x2& Second) const {
	const Vector2 a[2]{ {this->m[0][0],this->m[0][1]},{this->m[1][0],this->m[1][1]} };
	const Vector2 b[2]{ {Second.m[0][0],Second.m[1][0]},{Second.m[0][1],Second.m[1][1]}};

	return (Matrix2x2{
		(a[0]* b[0]),
		(a[0]* b[1]),
		(a[1]* b[0]),
		(a[1]* b[1])
		});
}


Matrix2x2 Matrix2x2::operator*(const float& Second) const {
	return (Matrix2x2{
		this->m[0][0] * Second,
		this->m[0][1] * Second,
		this->m[1][0] * Second,
		this->m[1][1] * Second
		});
}

Matrix2x2 Matrix2x2::operator/(const float& Second) const {
	return (Matrix2x2{
		this->m[0][0] / Second,
		this->m[0][1] / Second,
		this->m[1][0] / Second,
		this->m[1][1] / Second
		});
}





void Matrix2x2::operator+=(const Matrix2x2& Second) {

	this->m[0][0] += Second.m[0][0];
	this->m[0][1] += Second.m[0][1];
	this->m[1][0] += Second.m[1][0];
	this->m[1][1] += Second.m[1][1];
}

void Matrix2x2::operator-=(const Matrix2x2& Second) {

	this->m[0][0] -= Second.m[0][0];
	this->m[0][1] -= Second.m[0][1];
	this->m[1][0] -= Second.m[1][0];
	this->m[1][1] -= Second.m[1][1];
}

void Matrix2x2::operator*=(const Matrix2x2& Second) {
	const Vector2 a[2]{ {this->m[0][0],this->m[0][1]},{this->m[1][0],this->m[1][1]} };
	const Vector2 b[2]{ {Second.m[0][0],Second.m[1][0]},{Second.m[0][1],Second.m[1][1]} };

	*this = {
		(a[0]* b[0]),
		(a[0]* b[1]),
		(a[1]* b[0]),
		(a[1]* b[1])
	};
}


void Matrix2x2::operator*=(const float& Second) {
	this->m[0][0] *= Second;
	this->m[0][1] *= Second;
	this->m[1][0] *= Second;
	this->m[1][1] *= Second;
}

void Matrix2x2::operator/=(const float& Second) {
	this->m[0][0] /= Second;
	this->m[0][1] /= Second;
	this->m[1][0] /= Second;
	this->m[1][1] /= Second;
}



Matrix2x2::Matrix2x2() :m{ {0,0},{0,0} }
{

}
Matrix2x2::Matrix2x2(float A) :m{ {A,0},{0,A} }
{

}

Matrix2x2::Matrix2x2(float A, float B, float C, float D) :m{ {A,B},{C,D} }
{

}