
#include <iostream>
#include <fstream>
#include <string>
#include <assert.h>
#include <utility>

#include "boundingbox.h"
#include "mesh.h"
#include "vector.h"
#include "vertex.h"
#include "face.h"
#include "material.h"
#include "primitive.h"
#include "camera.h"

#include "sphere.h"
#include "cylinder.h"
#include "cone.h"
#include "cube.h"

//Adapted from RPI Graphics hw3

//Reads the input file in
void Mesh::LoadInput(const std::string &ifile,InputParser *_input){
	input=_input;
	std::ifstream istream(ifile.c_str());
	if (istream == NULL) {
		std::cout << "ERROR! CANNOT OPEN " << ifile << std::endl;
		return;
	}

	int lastslash = ifile.rfind("/");
	std::string directory =ifile.substr(0,lastslash+1);

	std::string token;
	Material *active_material = NULL;
	camera = NULL;
	background_color = Vec(1,1,1);

	while(istream >> token){
		if(token == "v"){
			//adds vertex
			double x,y,z;
			istream >> x >> y >> z;
			addVertex(Vec(x,y,z));
		} else if(token == "f"){
			//adds a face
			int a,b,c,d;
			istream >> a >> b >> c >> d;
			a--; b--; c--; d--;
			assert(a >= 0 && a < numVertices());
			assert(b >= 0 && b < numVertices());
			assert(c >= 0 && c < numVertices());
			assert(d >= 0 && d < numVertices());
			assert(active_material != NULL);
			addFace(getVertex(a),getVertex(b),getVertex(c),getVertex(d),active_material,false);
		} else if(token == "cube"){
			//adds a cube
			int a,b,c,d,e,f,g,h;
			istream >> a >> b >> c >> d >> e >> f >> g >> h;
			a--; b--; c--; d--; e--; f--; g--; h--;
			assert(a >= 0 && a < numVertices());
			assert(b >= 0 && b < numVertices());
			assert(c >= 0 && c < numVertices());
			assert(d >= 0 && d < numVertices());
			assert(e >= 0 && e < numVertices());
			assert(f >= 0 && f < numVertices());
			assert(g >= 0 && g < numVertices());
			assert(h >= 0 && h < numVertices());
			assert(active_material != NULL);
			addPrimitive(new Cube(a,b,c,d,e,f,g,h,active_material));
		} else if(token == "s"){
			//adds a sphere
			double x,y,z,r;
			istream >> x >> y >> z >> r;
			Vec center(x,y,z);
			addPrimitive(new Sphere(center,r,active_material));
		} else if(token == "c"){
			//Adds a cylinder
			double x,y,z,a,b,c,r;
			istream >> x >> y >> z >> a >> b >> c >>r;
			Vec center1(x,y,z);
			Vec center2(a,b,c);
			addPrimitive(new Cylinder(center1,center2,r,active_material));
		} else if(token == "cone"){
			//Adds a cone
			double x,y,z,a,b,c,h,t;
			istream >> x >> y >> z >> a >> b >> c >> h >> t;
			Vec o(x,y,z);
			Vec d(a,b,c);
			addPrimitive(new Cone(o,d,h,t,active_material));
		} else if(token == "background_color"){
			//Sets background color of the scene
			double r,g,b;
			istream >> r >> g >> b;
			background_color=Vec(r,g,b);
		} else if (token == "PerspectiveCamera") {
			//Sets the camera used for the raytracer and previewer
			camera= new PerspectiveCamera();
			istream >> *(PerspectiveCamera*)camera;
		} else if (token == "m"){
			//Selects the material for all subsequent objects
			int m;
			istream >> m;
			active_material = materials[m];
			//select material
		} else if (token == "material"){
			//Reads in a material and adds it to the stack
			double r,g,b,ind = 0;
			Vec diffuse,reflective,emitted,sig_s,sig_a;
			istream >> token;
			assert(token == "diffuse");
			istream >> r >> g >> b;
			diffuse=Vec(r,g,b);
			istream >> token;
			assert(token == "reflective");
			istream >> r >> g >> b;
			reflective=Vec(r,g,b);
			istream >> token;
			//Optional parameters
			if(token == "sigma_s"){
				istream >> r >> g >> b;
				sig_s = Vec(r,g,b);
				istream >> token;
			}
			if(token == "sigma_a"){
				istream >> r >> g >> b;
				sig_a = Vec(r,g,b);
				istream >> token;
			}
			if(token == "index" || token == "index_of_refraction"){
				istream >> ind;
				istream >> token;
			}
			assert(token == "emitted");
			istream >> r >> g >> b;
			emitted=Vec(r,g,b);
			materials.push_back(new Material(diffuse,reflective,emitted,sig_s,sig_a,ind));
			//make material
		} else {
			std::cout << "UNKNOWN TOKEN " << token << std::endl;
			exit(0);
		}
	}
	std::cout<<"Mesh Loaded"<<std::endl;
}

//Adds a vertex and adjusts bounding box
Vertex* Mesh::addVertex(const Vec &position){
	int ind = numVertices();
	vertices.push_back(new Vertex(ind,position));
	if(bbox==NULL){
		bbox=new BoundingBox(position,position);
	} else {
		bbox->Extend(position);
	}
	return vertices[ind];
}

//Adds a quad with the four vertices
Face* Mesh::addFace(Vertex *a,Vertex *b,Vertex *c,Vertex *d,Material 
	*material,bool primitive){
	Face *f=new Face(material); //Make Face

	Edge *ea = new Edge(a,b,f); //Make Edge
	Edge *eb = new Edge(b,c,f);
	Edge *ec = new Edge(c,d,f);
	Edge *ed = new Edge(d,a,f);

	//sets the edge the face knows about
	f->setEdge(ea);

	//Links edges to next edge
	ea->setNext(eb);
	eb->setNext(ec);
	ec->setNext(ed);
	ed->setNext(ea);

	//sets the opposite edge
	edgeshashtype::iterator op_ea=edges.find(std::make_pair(b,a));
	edgeshashtype::iterator op_eb=edges.find(std::make_pair(c,b));
	edgeshashtype::iterator op_ec=edges.find(std::make_pair(d,c));
	edgeshashtype::iterator op_ed=edges.find(std::make_pair(a,d));
	if(op_ea != edges.end()){
		op_ea->second->setOpposite(ea);
	}
	if(op_eb != edges.end()){
		op_eb->second->setOpposite(eb);
	}
	if(op_ec != edges.end()){
		op_ec->second->setOpposite(ec);
	}
	if(op_ed != edges.end()){
		op_ed->second->setOpposite(ed);
	}

	//Adds face to appropriate lists
	if(!primitive){
		faces.push_back(f);
	} else {
		primitive_faces.push_back(f);
	}
	if(material->getEmittedColor().Length() > 0){
		lights.push_back(f);
	}
	return f;
}

//Adds a primitive to the mesh
void Mesh::addPrimitive(Primitive *p){
	primitives.push_back(p);
	p->addRasterizedFaces(this,input);
}
