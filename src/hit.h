#ifndef _HIT_H_
#define _HIT_H_

#include <float.h>
#include "vector.h"
#include "ray.h"
#include "primitive.h"

//Adapted from RPI Graphics hw3
//Data structure that holds the detail of the intersection between a ray and an object

class Hit{
public:
	Hit() {
		t = FLT_MAX;
		material = NULL;
		normal = Vec(0,0,0);
		object = NULL;
	}
	Hit(const Hit &h) {
		t = h.t;
		material = h.material;
		normal = h.normal;
		object = h.object;
	}
	~Hit() {}

	double getT() const { return t;}
	Material* getMaterial() const { return material; }
	Vec getNormal() const { return normal; }
	bool hasObject() const { return object!=NULL; }
	const Primitive* getObject() const { return object; }

	void set(double _t, Material *m, Vec n){
		t = _t;
		material = m;
		normal = n;
	}
	void setObject(const Primitive *obj){
		object = obj;
	}
	void setFaceCollide(){
		object = NULL;
	}
	
private:
	//Time that intersection occurs
	double t;
	//Material at the point of the intersection
	Material *material;
	//Object that it intersected with
	const Primitive *object;
	//Normal of point of intersectoin
	Vec normal;
};

#endif
