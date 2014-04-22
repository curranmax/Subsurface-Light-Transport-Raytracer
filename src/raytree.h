#ifndef _RAY_TREE_H_
#define _RAY_TREE_H_

#include "ray.h"
#include <vector>
#include "canvas.h"
#include "vbo_structs.h"

//Adapted from RPI Graphics hw3

//Datastructure for a single segment for tracing a ray
class Segment {
public:
	Segment(const Ray &ray,double tstart,double tstop){
		if(tstart < -1000){
			tstart = -1000;
		}
		if(tstop > 1000){
			tstop = 1000;
		}
		a = ray.pointAtT(tstart);
		b = ray.pointAtT(tstop);
	}
	const Vec& getStart() const { return a; }
	const Vec& getEnd() const { return b; }
private:
	Vec a;
	Vec b;
};

//Stores the segments and visualizes them
class RayTree {
public:
	static void Activate() { Clear(); activated = 1; }
	static void Deactivate() { activated = 0; }

	//Static method to add different types of rays
	static void AddMainSegment(const Ray &ray,double tstart,double tstop){
		if(activated){
			main_segments.push_back(Segment(ray,tstart,tstop));
		}
	}
	static void AddShadowSegment(const Ray &ray,double tstart,double tstop){
		if(activated){
			shadow_segments.push_back(Segment(ray,tstart,tstop));
		}
	}
	static void AddReflectedSegment(const Ray &ray,double tstart,double tstop){
		if(activated){
			reflected_segments.push_back(Segment(ray,tstart,tstop));
		}
	}
	static void AddSingleScatterSegment(const Ray &ray,double tstart,double tstop){
		if(activated){
			single_scatter_segments.push_back(Segment(ray,tstart,tstop));
		}
	}
	static void AddMultipleScatterSegment(const Ray &ray,double tstart,double tstop){
		if(activated){
			multiple_scatter_segments.push_back(Segment(ray,tstart,tstop));
		}
	}

	static void initializeVBOs();
	static void setupVBOs();
	static void drawVBOs();
	static void cleanupVBOs();

private:
	static void Clear(){
		main_segments.clear();
		shadow_segments.clear();
		reflected_segments.clear();
		single_scatter_segments.clear();
		multiple_scatter_segments.clear();
	}

	static int activated;
	static std::vector<Segment> main_segments;
	static std::vector<Segment> shadow_segments;
	static std::vector<Segment> reflected_segments;
	static std::vector<Segment> single_scatter_segments;
	static std::vector<Segment> multiple_scatter_segments;

	static GLuint raytree_verts_VBO;
	static GLuint raytree_edge_indices_VBO;
	static std::vector<VBOPosColor4> raytree_verts;
	static std::vector<VBOIndexedEdge> raytree_edge_indices;

};

#endif
