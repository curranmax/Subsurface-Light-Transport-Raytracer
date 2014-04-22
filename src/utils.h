#ifndef _UTILS_H_
#define _UTILS_H_

#include "MersenneTwister.h"
#include "matrix.h"
#include "vector.h"

#define SRGB_ALPHA .055
#define INTERSECT_EPSILON 0.00001
#define RANDOM_POINT_EPSILON .05
#define MAX_RANDOM_TRIES 100

//Adapted from RPI Graphics hw3
//Useful functions that don't fit in other classes

extern MTRand GLOBAL_mtrand;

//functions to convert color between normal representation and for display
inline double linear_to_srgb(double x){
	double ans;
	if(x<=.0031308){
		ans=12.92*x;
	}else{
		ans=(1+SRGB_ALPHA)*(pow(x,1/2.4)-SRGB_ALPHA);
	}
	return ans;
}

inline double srgb_to_linear(double x){
	double ans;
	if(x <= 0.04045){
		ans = x / 12.92;
	} else {
		ans = pow((x+SRGB_ALPHA)/(1+SRGB_ALPHA),2.4);
	}
	return ans;
}

inline double dist2Vec(const Vec &p1,const Vec &p2){
	return (p1-p2).Length();
}

inline double AreaOfTriangle(double a,double b,double c){
	double s = (a+b+c)/2.0;
	return sqrt(s*(s-a)*(s-b)*(s-c));
}

//Computes Fresnel Reflectance term
inline double FresnelReflectance(double n,double theta){
	double cosi = cos(theta);
	double cost = sqrt(1 - n * n * sin(theta) * sin(theta));
	double Rs = (n * cosi - cost)/(n * cosi + cost);
	Rs *= Rs;
	double Rt = (n * cost - cosi)/(n * cost + cosi);
	Rt *= Rt;
	if(isnan(Rs)){
		Rs = 0;
	}
	if(isnan(Rt)){
		Rt = 0;
	}
	double R = (Rt + Rs)/2;
	return R;
}

//computes transmission term
inline double FresnelTransmission(double n,double theta){
	return 1-FresnelReflectance(n,theta);
}

//Helper for use when rasterizing primitives
inline Vec computeCirclePoint(double r,double t,Vec a,Vec d,Vec c) {
	a.Normalize();
	Vec p = r*a;
	Matrix m = Matrix::MakeAxisRotation(d,2 * M_PI * t);
	m.Transform(p);
	return p + c;
}

//Makes radius1 the larger of the two, and ensures that the differnce between them is at least 10*RANDOM_POINT_EPSLION
inline void fixRadius(double &radius1,double &radius2){
	if(radius1 < radius2){
		double temp = radius1;
		radius1 = radius2;
		radius2 = temp;
	}
	if(radius1 - radius2 < RANDOM_POINT_EPSILON * 10){
		radius2 = radius1 - RANDOM_POINT_EPSILON * 10;
	}
}

#endif
