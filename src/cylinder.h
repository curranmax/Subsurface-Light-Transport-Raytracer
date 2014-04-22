#ifndef _CYLINDER_H_
#define _CYLINDER_H_

#include "primitive.h"
#include "ray.h"
#include "hit.h"
#include "vector.h"
#include "matrix.h"

class Cylinder : public Primitive {
public:
	Cylinder(const Vec &c1, const Vec &c2, double r,Material *m){
		center1 = c1;
		center2 = c2;
		radius = r;
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
	double Height() const {
		return (center1-center2).Length();
	}

	Vec Direction() const {
		Vec d = center2 - center1;
		d.Normalize();
		return d;
	}

	Vec getClosestPointOnCylinder(Vec point) const{
		Vec d = Direction();
		return (d.Dot(point-center1))*d + center1;
	}

	bool collideInBounds(double t,const Ray &r) const {
		Vec p = r.getDirection() * t + r.getOrigin();
		Vec y = getClosestPointOnCylinder(p);
		double h = (y-center1).Dot(Direction());
		return (h >= 0 && h <= Height());
	}

	Vec rasterAxis() const {
		Vec d = Direction();
		Vec x(1,0,0);
		Vec axis = x - d.Dot(x) * d;
		axis.Normalize();
		return axis;
	}

	//Used for making random points
	void computeSA();
	double SA_end;
	double SA_mid;
	Matrix rotationMatrix;

	//Representation
	Vec center1;
	Vec center2;
	double radius;
};

#endif
