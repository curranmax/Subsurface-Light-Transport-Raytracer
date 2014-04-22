#ifndef _PRIMITIVE_H_
#define _PRIMITIVE_H_

class Mesh;
class Ray;
class Hit;
class InputParser;
class Material;
class Vec;

//Adapted from RPI Graphics hw3

class Primitive{
public:
	virtual ~Primitive() {}

	Material* getMaterial() const { return material; }

	virtual bool intersect(const Ray &r,Hit &h) const = 0;

	virtual void addRasterizedFaces(Mesh *m, InputParser *input) = 0;

	virtual Vec getRandomPoint() const = 0;
	virtual Vec getRandomPoint(const Vec &p, double valid_radius) const = 0;
	virtual Vec getRandomPoint(const Vec &p, double radius1, double radius2) const = 0;
	
protected:
	Material *material;
};

#endif
