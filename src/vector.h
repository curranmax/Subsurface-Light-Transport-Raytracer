#ifndef _VECTOR_H_
#define _VECTOR_H_

//Adapted from RPI Graphics hw3

//Represents a 3 dimensional vector

#include <iostream>
#include <cassert>
#include <cmath>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

class Vec{
public:
	//Constructors and Assignment
	Vec(){
		data[0]=0;
		data[1]=0;
		data[2]=0;
	}
	Vec(const Vec &V) {
		data[0] = V.data[0];
		data[1] = V.data[1];
		data[2] = V.data[2];
	}
	Vec(double a,double b,double c){
		data[0]=a;
		data[1]=b;
		data[2]=c;
	}
	const Vec& operator=(const Vec &V) {
		data[0] = V.data[0];
		data[1] = V.data[1];
		data[2] = V.data[2];
		return *this;
	}

	//Getters and Setters
	double operator[](int i) const{
		assert (i >= 0 && i < 3);
		return data[i];
	}
	double x() const { return data[0]; }
	double y() const { return data[1]; }
	double z() const { return data[2]; }
	double r() const { return data[0]; }
	double g() const { return data[1]; }
	double b() const { return data[2]; }

	//Common Vector Operations
	double Length() const{
		return sqrt(data[0]*data[0]+data[1]*data[1]+data[2]*data[2]);
	}
	void Normalize(){
		double length=Length();
		if( length > 0 ){
			Scale(1/length);
		}
	}
	void Scale(double d){
		Scale(d,d,d);
	}
	void Scale(double d0,double d1,double d2){
		data[0] *= d0;
		data[1] *= d1;
		data[2] *= d2;
	}
	void Scale(const Vec &V){
		Scale(V.data[0],V.data[1],V.data[2]);
	}
	void Negate(){
		Scale(-1.0);
	}
	double Dot(const Vec &V) const{
		return data[0]*V.data[0]+data[1]*V.data[1]+data[2]*V.data[2];
	}
	double Average() const{
		return (data[0]+data[1]+data[2])/3;
	}

	Vec Exp() const{
		return Vec(exp(data[0]),exp(data[1]),exp(data[2]));
	}
	Vec Sqrt() const{
		return Vec(sqrt(data[0]),sqrt(data[1]),sqrt(data[2]));
	}

	void Invert() {
		data[0]=1/data[0];
		data[1]=1/data[1];
		data[2]=1/data[2];
	}

	void Abs() {
		data[0]=fabs(data[0]);
		data[1]=fabs(data[1]);
		data[2]=fabs(data[2]);
	}

	static void Cross(Vec &c,const Vec &a,const Vec &b){
		c=Vec(a[1]*b[2]-a[2]*b[1],
			a[2]*b[0]-a[0]*b[2],
			a[0]*b[1]-a[1]*b[0]);
	}

	//Operators
	Vec& operator+=(const Vec &V){
		data[0]+=V.data[0];
		data[1]+=V.data[1];
		data[2]+=V.data[2];
		return *this;
	}
	Vec& operator-=(const Vec &V){
		data[0]-=V.data[0];
		data[1]-=V.data[1];
		data[2]-=V.data[2];
		return *this;
	}
	Vec& operator*=(double d){
		data[0]*=d;
		data[1]*=d;
		data[2]*=d;
		return *this;
	}
	Vec& operator/=(double d){
		data[0]/=d;
		data[1]/=d;
		data[2]/=d;
		return *this;
	}
	friend Vec operator+(const Vec &v1,const Vec &v2){
		Vec v3 = v1;
		v3 += v2;
		return v3;
	}
	friend Vec operator-(const Vec &v1,const Vec &v2){
		Vec v3 = v1;
		v3 -= v2;
		return v3;
	}
	friend Vec operator*(const Vec &v1,double d){
		Vec v2 = v1;
		v2.Scale(d);
		return v2;
	}
	friend Vec operator*(const Vec &v1,const Vec &v2){
		Vec v3=v1;
		v3.Scale(v2.x(),v2.y(),v2.z());
		return v3;
	}
	friend Vec operator*(double d,const Vec &v1){
		return v1 * d;
	}
	friend Vec operator/(const Vec &v1,double d){
		Vec v2 = v1;
		v2.Scale(1/d);
		return v2;
	}


    //Input and Output
	friend std::ostream& operator<< (std::ostream &ostr, const Vec &v) {
		ostr << v.data[0] << " " << v.data[1] << " " << v.data[2] << std::endl; 
		return ostr;
	}
	friend std::istream& operator>> (std::istream &istr, Vec &v) {
		istr >> v.data[0] >> v.data[1] >> v.data[2];
		return istr;
	}
private:
	//internal representation of the vector
	double data[3];
};

#endif
