#ifndef MESH_H
#define MESH_H

#include <vector>
#include <assert.h>

#include "vector.h"
#include "inputparser.h"
#include "vertex.h"
#include "hash.h"

//Adapted from RPI Graphics hw3

//Holds all objects for the previewer and for checking for intersection

class BoundingBox;
class Material;
class Face;
class Vertex;
class Primitive;
class Camera;

class Mesh{
public:
	Mesh() { bbox = NULL; }
	void LoadInput(const std::string &ifile,InputParser *_input);

	//Vertices
	Vertex* addVertex(const Vec &position);
	int numVertices() const { return vertices.size(); }
	Vertex* getVertex(int i) const{
		assert(i >= 0 && i < numVertices());
		return vertices[i];
	}

	//Faces
	Face* addFace(Vertex *a,Vertex *b, Vertex *c,Vertex *d,Material *material,bool primitive);
	int numFaces() const { return faces.size()+primitive_faces.size(); }
	int numOriginalFaces() const { return faces.size(); }
	Face* getOriginalFace(int i) const { return faces[i]; }
	Face* getFace(int i) const {
		if(i<(int)faces.size()){
			return faces[i];
		}else{
			return primitive_faces[i-faces.size()];
		}
	}
	std::vector<Face*>& getLights() { return lights;}

	//Primitives
	int numPrimitives() const { return primitives.size(); }
	Primitive* getPrimitive(int i) const { return primitives[i]; }	

private:
	void addPrimitive(Primitive *p);

public:
	std::vector<Material*> materials;
	Vec background_color;
	Camera *camera;

private:
	BoundingBox *bbox;
	InputParser *input;
	
	std::vector<Vertex*> vertices;
	edgeshashtype edges;

	std::vector<Face*> faces;
	std::vector<Face*> lights;

	std::vector<Primitive*> primitives;
	std::vector<Face*> primitive_faces;	
};


#endif
