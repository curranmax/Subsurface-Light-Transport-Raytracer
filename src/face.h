#ifndef _FACE_H_
#define _FACE_H_

#include "edge.h"
#include "vertex.h"

//Copied from RPI Graphics hw3

class Ray;
class Hit;
class Material;

class Face{
public:
	Face(Material *m){
		material=m;
		edge=NULL;
	}

	Vertex* operator[](int i) const{
		assert(edge != NULL);
		assert(i >= 0 && i < 4);
		Edge *e = edge;
		while(i > 0){
			e = e->getNext();
			--i;
		}
		return e->getStartVertex();
	}
	Edge* getEdge() const {
		assert(edge != NULL);
		return edge;
	}
	Vec computeCenter() const {
		Vec sum;
		for(int i = 0;i < 4;++i){
			sum+=(*this)[i]->get();
		}
		return sum/4.0;
	}
	Material* getMaterial() const { return material; }
	double getArea() const;
	Vec RandomPoint() const;
	Vec computeNormal() const;

	void setEdge(Edge *e){
		assert(edge == NULL);
		assert(e != NULL);
		edge = e;
	}

	bool intersect(const Ray &r, Hit &h, bool intersect_backfacing) const;
protected:
	bool triangle_intersect(const Ray &r,Hit &h,Vertex *av,Vertex *bv, Vertex *cv,bool intersect_backfacing) const;
	bool plane_intersect(const Ray &r,Hit &h,bool intersect_backfacing) const;

	Face& operator=(const Face&) { exit(0); }
	
private:
	Material *material;
	Edge *edge;
};

#endif
