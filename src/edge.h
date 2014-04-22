#ifndef _EDGE_H_
#define _EDGE_H_

class Vertex;
class Face;

//Copied from RPI Graphics hw3
//Edge in the half edge data structure

class Edge{
public:
	Edge(Vertex *vstart,Vertex *vend,Face *f);
	~Edge();

	Vertex* getStartVertex() const{ return startVertex; }
	Vertex* getEndVertext() const{ return endVertex; }
	Edge* getNext() const { return next; }
	Face* getFace() const { return face; }
	Edge* getOpposite() const { return opposite; }
	float Length() const;

	void setOpposite(Edge *e){
		opposite=e;
		e->opposite=this;
	}
	void setNext(Edge *e){
		next=e;
	}

private:
	Vertex *startVertex;
	Vertex *endVertex;
	Face *face;
	Edge *opposite;
	Edge *next;
};

#endif
