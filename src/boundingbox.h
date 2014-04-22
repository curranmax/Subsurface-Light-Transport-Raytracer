#ifndef _BOUNDING_BOX_H_
#define _BOUNDING_BOX_H_

#include "vector.h"
#include "canvas.h"

//Copied from RPI Graphics hw3

class BoundingBox{
public:
	BoundingBox() { Set(Vec(0,0,0),Vec(0,0,0)); }
	BoundingBox(const Vec &v) { Set(v,v); }
	BoundingBox(const Vec &min, const Vec &max) { Set(min,max); }
	~BoundingBox() { }

	void get(Vec &min,Vec &max){
		min=minimum;
		max=maximum;
	}
	const Vec& getMin() const { return minimum; }
	const Vec& getMax() const { return maximum; }
	void getCenter(Vec &c){
		c = (minimum+maximum)/2.0;
	}

	void Set(const BoundingBox &bb){
		Set(bb.minimum,bb.maximum);
	}
	void Set(const Vec &min,const Vec &max){
		assert(min.x()<=max.x() && min.y()<=max.y() && min.z()<=max.z());
		maximum=max;
		minimum=min;
	}
	void Extend(const Vec &v){
		maximum = Vec(max(v.x(),maximum.x()),max(v.y(),maximum.y()),max(v.z(),maximum.z()));
		minimum = Vec(min(v.x(),minimum.x()),min(v.y(),minimum.y()),min(v.z(),minimum.z()));
	}

	//VBOs
	void initializeVBOs();
	void setupVBOs();
	void drawVBOs();
	void cleanupVBOs();

private:
	double min(double a,double b){
		if(a>=b){
			return b;
		}
		return a;
	}
	double max(double a,double b){
		if(a>=b){
			return a;
		}
		return b;
	}

	Vec minimum;
	Vec maximum;

	GLuint bb_verts_VBO;
	GLuint bb_edge_indices_VBO;
};

#endif
