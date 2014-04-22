#ifndef _MATRIX_H_
#define _MATRIX_H_

#include <iostream>

#include "vector.h"

//Copied from RPI Graphics hw3

//Models a 4x4 matrix
//Can alter 3 dimensional vectors by making them into homogenous coordinates

class Matrix {
public:
	Matrix() {clear();}
	Matrix(const Matrix &m);
	Matrix& operator=(const Matrix &m);

	double get(int row,int col) const {
		return data[row+col*4];
	}

	void set(int row,int col,double v){
		data[row+col*4]=v;
	}
	void setToIdentity();
	void clear();

	friend Matrix operator+(const Matrix &m1,const Matrix &m2);
	friend Matrix operator-(const Matrix &m1,const Matrix &m2);
	friend Matrix operator*(const Matrix &m1,const Matrix &m2);
	friend Vec operator*(const Matrix &m1,const Vec &v2);
	friend Matrix operator*(const Matrix &m1,double v);
	friend Matrix operator*(double v,const Matrix &m1) { return m1 * v; }

	Matrix& operator+=(const Matrix &m){
		*this=*this+m;
		return *this;
	}
	Matrix& operator-=(const Matrix &m){
		*this=*this-m;
		return *this;
	}
	Matrix& operator*=(const Matrix &m){
		*this=*this*m;
		return *this;
	}
	Matrix& operator*=(double v){
		*this=*this*v;
		return *this;
	}

	static Matrix MakeTranslation(const Vec &v);
	static Matrix MakeAxisRotation(const Vec &v,double theta);
	static Matrix MakeOuterProduct(const Vec &v);

	void Transform(Vec &v) const;

	friend std::ostream& operator<<(std::ostream &ostr, const Matrix &m);

	static double det3x3(double a1,double a2,double a3,double b1,double b2,double b3,double c1,double c2,double c3);
	static double det2x2(double a,double b,double c,double d);

private:
	//Data stored in an array
	double data[16];
};

#endif
