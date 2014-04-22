#ifndef _CONE_H_
#define _CONE_H_

#include "primitive.h"
#include "ray.h"
#include "hit.h"
#include "inputparser.h"
#include "mesh.h"
#include "vector.h"
#include "material.h"
#include "matrix.h"

class Cone : public Primitive {
public:
	Cone(const Vec &o,const Vec &d,double h,double t,Material *m){
		dir = d;
		dir.Normalize();
		origin = o;
		height = h;
		theta = t;
		material = m;

		computeSA();
	}

	virtual bool intersect(const Ray &r,Hit &h) const;

	void addRasterizedFaces(Mesh *m,InputParser *input);

	Vec getRandomPoint() const;
	Vec getRandomPoint(const Vec &p, double valid_radius) const;
	Vec getRandomPoint(const Vec &p, double radius1, double radius2) const;

private:
	//Helper functions

	//Checks if on the 
	bool validIntersection(double t,const Ray &r) const {
		Vec x = r.getDirection()*t+r.getOrigin();
		Vec y = dir * dir.Dot(x-origin);
		return dir.Dot(y) >=0  && dir.Dot(y) <= height;
	}

	//Gets normal at a given point on the cone
	Vec getNormal(Vec x) const {
		double k = (x-origin).Dot(x-origin)/dir.Dot(x-origin);
		Vec y =  dir * k + origin;
		Vec norm = x-y;
		norm.Normalize();
		return norm;
	}

	//Radius of the end cap
	double radius() const{
		return tan(theta)*height;
	}

	//Distance between point of cone and outside of end cap
	double side() const{
		double rad = radius();
		return sqrt(rad * rad + height * height);
	}

	//Direction used in making rasterized version of cone
	Vec rasterAxis() const {
		Vec x(1,0,0);
		Vec axis = x - dir.Dot(x) * dir;
		axis.Normalize();
		return axis;
	}

	//Computes variables necessary for choosing random points
	void computeSA(){
		double rad = radius();	
		SA_end = M_PI * rad * rad;
		SA_cone = M_PI * rad * side();

		Vec rot_axis;
		Vec::Cross(rot_axis,dir,Vec(0,0,1));
		double rot_theta = acos(dir.Dot(Vec(0,0,1)));
		rotationMatrix = Matrix::MakeAxisRotation(rot_axis,rot_theta);
	}
	
	//Variables for choosing random points
	double SA_end;
	double SA_cone;
	Matrix rotationMatrix;

	//Describe the cone
	Vec origin;
	Vec dir;
	double height;
	double theta;
	Material *material;

};

#endif
