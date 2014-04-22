#ifndef _CUBE_H_
#define _CUBE_H_

#include "primitive.h"
#include "material.h"
#include "ray.h"
#include "hit.h"
#include "mesh.h"
#include "inputparser.h"
#include "face.h"

#include <vector>

class Cube : public Primitive {
public:
	Cube(int v1,int v2,int v3,int v4,int v5,int v6,int v7,int v8,Material *m){
		material=m;
		vs={v1,v2,v3,v4,v5,v6,v7,v8};
	}

	virtual bool intersect(const Ray &r,Hit &h) const;

	void addRasterizedFaces(Mesh *m,InputParser *input);

	Vec getRandomPoint() const;
	Vec getRandomPoint(const Vec &p, double valid_radius) const;
	Vec getRandomPoint(const Vec &p, double radius1, double radius2) const;

private:

	void computeSA();

	//Used for getting random points
	std::vector<double> SA_faces;
	double total_SA;

	//Representation of the cube
	Material *material;
	std::vector<int> vs;
	std::vector<Face*> fs;

};

#endif
