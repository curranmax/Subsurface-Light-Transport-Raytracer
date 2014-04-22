#include "canvas.h"
#include "boundingbox.h"
#include "vbo_structs.h"

//Copied from RPI Graphics hw3

//intializes VBO buffers
void BoundingBox::initializeVBOs(){
	glGenBuffers(1, &bb_verts_VBO);
	glGenBuffers(1, &bb_edge_indices_VBO);
}

//binds data to VBO buffers
void BoundingBox::setupVBOs(){
	HandleGLError("setup VBOs a ");
	VBOPos bb_verts[8];
	VBOIndexedEdge bb_edges[12];

	bb_verts[0] = VBOPos(Vec(minimum.x(),minimum.y(),minimum.z()));
	bb_verts[1] = VBOPos(Vec(minimum.x(),minimum.y(),maximum.z()));
	bb_verts[2] = VBOPos(Vec(minimum.x(),maximum.y(),minimum.z()));
	bb_verts[3] = VBOPos(Vec(minimum.x(),maximum.y(),maximum.z()));
	bb_verts[4] = VBOPos(Vec(maximum.x(),minimum.y(),minimum.z()));
	bb_verts[5] = VBOPos(Vec(maximum.x(),minimum.y(),maximum.z()));
	bb_verts[6] = VBOPos(Vec(maximum.x(),maximum.y(),minimum.z()));
	bb_verts[7] = VBOPos(Vec(maximum.x(),maximum.y(),maximum.z()));

	bb_edges[ 0] = VBOIndexedEdge(0,1);
	bb_edges[ 1] = VBOIndexedEdge(1,3);
	bb_edges[ 2] = VBOIndexedEdge(3,2);
	bb_edges[ 3] = VBOIndexedEdge(2,0);
	bb_edges[ 4] = VBOIndexedEdge(0,4);
	bb_edges[ 5] = VBOIndexedEdge(1,5);
	bb_edges[ 6] = VBOIndexedEdge(2,6);
	bb_edges[ 7] = VBOIndexedEdge(3,7);
	bb_edges[ 8] = VBOIndexedEdge(4,5);
	bb_edges[ 9] = VBOIndexedEdge(5,7);
	bb_edges[10] = VBOIndexedEdge(7,6);
	bb_edges[11] = VBOIndexedEdge(6,4);

	HandleGLError("setup VBOs b ");
	glBindBuffer(GL_ARRAY_BUFFER,bb_verts_VBO);
	HandleGLError("setup VBOs c ");
	glBufferData(GL_ARRAY_BUFFER,sizeof(VBOPos)*8,bb_verts,GL_STATIC_DRAW);
	HandleGLError("setup VBOs d ");
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,bb_edge_indices_VBO);
	HandleGLError("setup VBOs e ");
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(VBOIndexedEdge)*12,bb_edges,GL_STATIC_DRAW);
	HandleGLError("setup VBOs bf ");
}

//Draws the buffers
void BoundingBox::drawVBOs(){
	HandleGLError("draw VBOs a ");

	glBindBuffer(GL_ARRAY_BUFFER,bb_verts_VBO);
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3,GL_FLOAT,sizeof(VBOPos),BUFFER_OFFSET(0));
	glLineWidth(1);
	glColor3f(0,0,0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,bb_edge_indices_VBO);
	HandleGLError("draw VBOS b ");
	glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT,0);
	HandleGLError("draw VBOs c ");
	glDisableClientState(GL_VERTEX_ARRAY);
}

//Deletes the buffers
void BoundingBox::cleanupVBOs(){
	glDeleteBuffers(1,&bb_verts_VBO);
	glDeleteBuffers(1,&bb_edge_indices_VBO);
}
