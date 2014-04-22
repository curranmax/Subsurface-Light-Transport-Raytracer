#ifndef _RAY_TRACER_
#define _RAY_TRACER_

#include <vector>

#include "vector.h"
#include "canvas.h"
#include "vbo_structs.h"
#include <string>

class Mesh;
class InputParser;
class Ray;
class Hit;

//Adapted from RPI Graphics hw3

class RayTracer {
public:
	RayTracer(Mesh *m,InputParser *in){
		mesh=m;
		input=in;
	}
	~RayTracer(){ cleanupVBOs(); };

	bool CastRay(const Ray &ray,Hit &h,bool use_rasterized_patches) const;

	Vec TraceRay(Ray &ray,Hit &hit,int bounce) const;

	void initializeVBOs();
	void setupVBOs();
	void drawVBOs();
	void cleanupVBOs();

private:
	Mesh *mesh;
	InputParser *input;

	//VBO stuff
	GLuint mesh_quad_verts_VBO;
	GLuint mesh_quad_indices_VBO;
	GLuint mesh_textured_quad_indices_VBO;
	GLuint mesh_interior_edge_indices_VBO;
	GLuint mesh_border_edge_indices_VBO;

	std::vector<VBOPosNormalColorTexture> mesh_quad_verts;
	std::vector<VBOIndexedQuad> mesh_quad_indices;
	std::vector<VBOIndexedQuad> mesh_textured_quad_indices;
	std::vector<VBOIndexedEdge> mesh_interior_edge_indices;
	std::vector<VBOIndexedEdge> mesh_border_edge_indices;
};

#endif
