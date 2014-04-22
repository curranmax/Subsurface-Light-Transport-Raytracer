#ifndef _VERTEX_H_
#define _VERTEX_H_

#include "vector.h"

//Copied from RPI Graphics hw3
//Vertex in the half edge data structure

class Vertex{
public:
	Vertex(int id,const Vec &pos)  { 
		position=pos;
		ind = id;
	}
	
	int getIndex() const { return ind; }
	const Vec& get() const { return position; }
	double x() const { return position.x(); }
	double y() const { return position.y(); }
	double z() const { return position.z(); }
private:
	int ind;
	Vec position;
};

#endif
