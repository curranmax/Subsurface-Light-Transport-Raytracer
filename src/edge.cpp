#include "vertex.h"
#include "edge.h"

//Copied from RPI Graphics hw3
//Edge in the half edge data structure

Edge::Edge(Vertex *vstart,Vertex *vend,Face *f){
	startVertex=vstart;
	endVertex=vend;
	face=f;
	next=NULL;
	opposite=NULL;
}

Edge::~Edge(){
	if(opposite != NULL){
		opposite->opposite=NULL;
	}
}

float Edge::Length() const {
	Vec dif=startVertex->get() - endVertex->get();
	return dif.Length();
}
