#ifndef _SPHERE_H_
#define _SPHERE_H_

#include "primitive.h"
#include "vector.h"

//Adapted from RPI Graphics hw3

class Sphere : public Primitive{
public:
	Sphere(const Vec &c,double r,Material *m){
		center = c;
		radius = r;
		material = m;
	}

	virtual bool intersect(const Ray &r,Hit &h) const;

	void addRasterizedFaces(Mesh *m,InputParser *input);

	Vec getRandomPoint() const;
	Vec getRandomPoint(const Vec &p, double valid_radius) const;
	Vec getRandomPoint(const Vec &p, double radius1, double radius2) const;

private:
	Vec center;
	double radius;
};

#endif
