#ifndef _RAY_H_
#define _RAY_H_

#include "vector.h"

//Adapted from RPI Graphics hw3

//Represents a ray of light coming from an eye going into the scene

class Ray {
public:
	Ray(const Vec &orig, const Vec &dir){
		origin = orig;
		direction = dir;
	}

	const Vec& getOrigin() const {
		return origin;
	}
	const Vec& getDirection() const {
		return direction;
	}
	Vec pointAtT(double t) const {
		return origin+direction*t;
	}
	
private:
	//Origin and direction
	Vec origin;
	Vec direction;
};

#endif
