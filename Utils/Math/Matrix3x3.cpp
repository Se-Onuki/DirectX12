#include "Matrix3x3.h"

//#include <Novice.h>
#include "Math.hpp"

//void Matrix3x3::Printf(const int &x, const int &y) const {
//	for (int i = 0; i < 3; i++) {
//		for (int j = 0; j < 3; j++) {
//			//Novice::ScreenPrintf(x + j * TextWidth * 2, y + i * TextHeight, "%10.6f", m[i][j]);
//		}
//	}
//}

Matrix3x3 Matrix3x3::Inverse() {
	return Matrix3x3{
	  this->m[1][1] * this->m[2][2] - this->m[1][2] * this->m[2][1],
	  this->m[0][2] * this->m[2][1] - this->m[0][1] * this->m[2][2],
	  this->m[0][1] * this->m[1][2] - this->m[0][2] * this->m[1][1],

	  this->m[1][2] * this->m[2][0] - this->m[1][0] * this->m[2][2],
	  this->m[0][0] * this->m[2][2] - this->m[0][2] * this->m[2][0],
	  this->m[0][2] * this->m[1][0] - this->m[0][0] * this->m[1][2],

	  this->m[1][0] * this->m[2][1] - this->m[1][1] * this->m[2][0],
	  this->m[0][1] * this->m[2][0] - this->m[0][0] * this->m[2][1],
	  this->m[0][0] * this->m[1][1] - this->m[0][1] * this->m[1][0]
	}
	/ (this->m[0][0] * this->m[1][1] * this->m[2][2] + this->m[0][1] * this->m[1][2] * this->m[2][0] + this->m[0][2] * this->m[1][0] * this->m[2][1] - this->m[0][0] * this->m[1][2] * this->m[2][1] - this->m[0][1] * this->m[1][0] * this->m[2][2] - this->m[0][2] * this->m[1][1] * this->m[2][0]);
}

Matrix3x3 Matrix3x3::Transpose() {
	return Matrix3x3{
	  this->m[0][0], this->m[1][0], this->m[2][0],
	  this->m[0][1], this->m[1][1], this->m[2][1],
	  this->m[0][2], this->m[1][2], this->m[2][2]
	};
}

Matrix3x3 Matrix3x3::operator+(const Matrix3x3 &Second) const {
	return (Matrix3x3{
		this->m[0][0] + Second.m[0][0],
		this->m[0][1] + Second.m[0][1],
		this->m[0][2] + Second.m[0][2],

		this->m[1][0] + Second.m[1][0],
		this->m[1][1] + Second.m[1][1],
		this->m[1][2] + Second.m[1][2],

		this->m[2][0] + Second.m[2][0],
		this->m[2][1] + Second.m[2][1],
		this->m[2][2] + Second.m[2][2],
		});
}

Matrix3x3 Matrix3x3::operator-(const Matrix3x3 &Second) const {
	return (Matrix3x3{
		this->m[0][0] - Second.m[0][0],
		this->m[0][1] - Second.m[0][1],
		this->m[0][2] - Second.m[0][2],

		this->m[1][0] - Second.m[1][0],
		this->m[1][1] - Second.m[1][1],
		this->m[1][2] - Second.m[1][2],

		this->m[2][0] - Second.m[2][0],
		this->m[2][1] - Second.m[2][1],
		this->m[2][2] - Second.m[2][2],
		});
}

Matrix3x3 Matrix3x3::operator*(const Matrix3x3 &Second) const {

	return (Matrix3x3{
		this->m[0][0] * Second.m[0][0] + this->m[0][1] * Second.m[1][0] + this->m[0][2] * Second.m[2][0],
		this->m[0][0] * Second.m[0][1] + this->m[0][1] * Second.m[1][1] + this->m[0][2] * Second.m[2][1],
		this->m[0][0] * Second.m[0][2] + this->m[0][2] * Second.m[1][2] + this->m[0][2] * Second.m[2][2],

		this->m[1][0] * Second.m[0][0] + this->m[1][1] * Second.m[1][0] + this->m[1][2] * Second.m[2][0],
		this->m[1][0] * Second.m[0][1] + this->m[1][1] * Second.m[1][1] + this->m[1][2] * Second.m[2][1],
		this->m[1][0] * Second.m[0][2] + this->m[1][2] * Second.m[1][2] + this->m[1][2] * Second.m[2][2],

		this->m[2][0] * Second.m[0][0] + this->m[2][1] * Second.m[1][0] + this->m[2][2] * Second.m[2][0],
		this->m[2][0] * Second.m[0][1] + this->m[2][1] * Second.m[1][1] + this->m[2][2] * Second.m[2][1],
		this->m[2][0] * Second.m[0][2] + this->m[2][2] * Second.m[1][2] + this->m[2][2] * Second.m[2][2],
		});
}




Matrix3x3 Matrix3x3::operator*(const float &Second) const {
	return (Matrix3x3{
	  this->m[0][0] * Second,
	  this->m[0][1] * Second,
	  this->m[0][2] * Second,
	  this->m[1][0] * Second,
	  this->m[1][1] * Second,
	  this->m[1][2] * Second,
	  this->m[2][0] * Second,
	  this->m[2][1] * Second,
	  this->m[2][2] * Second
		});
}

Matrix3x3 Matrix3x3::operator/(const float &Second) const {
	return (Matrix3x3{
	  this->m[0][0] / Second,
	  this->m[0][1] / Second,
	  this->m[0][2] / Second,
	  this->m[1][0] / Second,
	  this->m[1][1] / Second,
	  this->m[1][2] / Second,
	  this->m[2][0] / Second,
	  this->m[2][1] / Second,
	  this->m[2][2] / Second
		});
}

Matrix3x3 Matrix3x3::operator+=(const Matrix3x3& Second) {

	this->m[0][0] += Second.m[0][0];
	this->m[0][1] += Second.m[0][1];
	this->m[0][2] += Second.m[0][2];

	this->m[1][0] += Second.m[1][0];
	this->m[1][1] += Second.m[1][1];
	this->m[1][2] += Second.m[1][2];

	this->m[2][0] += Second.m[2][0];
	this->m[2][1] += Second.m[2][1];
	this->m[2][2] += Second.m[2][2];

	return *this;
}

Matrix3x3 Matrix3x3::operator-=(const Matrix3x3& Second) {
	this->m[0][0] -= Second.m[0][0];
	this->m[0][1] -= Second.m[0][1];
	this->m[0][2] -= Second.m[0][2];

	this->m[1][0] -= Second.m[1][0];
	this->m[1][1] -= Second.m[1][1];
	this->m[1][2] -= Second.m[1][2];

	this->m[2][0] -= Second.m[2][0];
	this->m[2][1] -= Second.m[2][1];
	this->m[2][2] -= Second.m[2][2];

	return *this;
}

Matrix3x3 Matrix3x3::operator*=(const Matrix3x3& Second) {

	return *this = Matrix3x3{
		this->m[0][0] * Second.m[0][0] + this->m[0][1] * Second.m[1][0] + this->m[0][2] * Second.m[2][0],
		this->m[0][0] * Second.m[0][1] + this->m[0][1] * Second.m[1][1] + this->m[0][2] * Second.m[2][1],
		this->m[0][0] * Second.m[0][2] + this->m[0][2] * Second.m[1][2] + this->m[0][2] * Second.m[2][2],

		this->m[1][0] * Second.m[0][0] + this->m[1][1] * Second.m[1][0] + this->m[1][2] * Second.m[2][0],
		this->m[1][0] * Second.m[0][1] + this->m[1][1] * Second.m[1][1] + this->m[1][2] * Second.m[2][1],
		this->m[1][0] * Second.m[0][2] + this->m[1][2] * Second.m[1][2] + this->m[1][2] * Second.m[2][2],

		this->m[2][0] * Second.m[0][0] + this->m[2][1] * Second.m[1][0] + this->m[2][2] * Second.m[2][0],
		this->m[2][0] * Second.m[0][1] + this->m[2][1] * Second.m[1][1] + this->m[2][2] * Second.m[2][1],
		this->m[2][0] * Second.m[0][2] + this->m[2][2] * Second.m[1][2] + this->m[2][2] * Second.m[2][2],
	};

}
Matrix3x3 Matrix3x3::operator*=(const float& Second) {

	this->m[0][0] *= Second;
	this->m[0][1] *= Second;
	this->m[0][2] *= Second;
	this->m[1][0] *= Second;
	this->m[1][1] *= Second;
	this->m[1][2] *= Second;
	this->m[2][0] *= Second;
	this->m[2][1] *= Second;
	this->m[2][2] *= Second;

	return *this;
}

Matrix3x3 Matrix3x3::operator/=(const float& Second) {

	this->m[0][0] /= Second;
	this->m[0][1] /= Second;
	this->m[0][2] /= Second;
	this->m[1][0] /= Second;
	this->m[1][1] /= Second;
	this->m[1][2] /= Second;
	this->m[2][0] /= Second;
	this->m[2][1] /= Second;
	this->m[2][2] /= Second;

	return *this;
}