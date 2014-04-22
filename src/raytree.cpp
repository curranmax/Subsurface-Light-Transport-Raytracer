#include "raytree.h"

//Adapted from RPI Graphics hw3

//Allows for rays for one pixel to be visualized in the previewer
//useful for debugging

int RayTree::activated = 0;
std::vector<Segment> RayTree::main_segments;
std::vector<Segment> RayTree::shadow_segments;
std::vector<Segment> RayTree::reflected_segments;
std::vector<Segment> RayTree::single_scatter_segments;
std::vector<Segment> RayTree::multiple_scatter_segments;

GLuint RayTree::raytree_verts_VBO;
GLuint RayTree::raytree_edge_indices_VBO;
std::vector<VBOPosColor4> RayTree::raytree_verts;
std::vector<VBOIndexedEdge> RayTree::raytree_edge_indices;

void RayTree::initializeVBOs() {
	glGenBuffers(1, &raytree_verts_VBO);
	glGenBuffers(1, &raytree_edge_indices_VBO);
}

//Draws a line for each ray that is slightly transparent
void RayTree::setupVBOs() {
	raytree_verts.clear();
	raytree_edge_indices.clear();

	//Sets different colors for each type and the constant for making the lines transparent
	double alpha = .5;
	Vec main_color(.7,.7,.7);
	Vec shadow_color(.1,.9,.1);
	Vec reflected_color(.9,.1,.1);
	Vec single_scatter_color(.1,.1,.9);
	Vec multiple_scatter_color(.9,.1,.9);

	unsigned int i;
	int count = 0;
	for(i = 0;i < main_segments.size();++i){
		raytree_verts.push_back(VBOPosColor4(main_segments[i].getStart(),main_color,alpha));
		raytree_verts.push_back(VBOPosColor4(main_segments[i].getEnd(),main_color,alpha));
		raytree_edge_indices.push_back(VBOIndexedEdge(count,count+1));
		count+=2;
	}
	for(i = 0;i < shadow_segments.size();++i){
		raytree_verts.push_back(VBOPosColor4(shadow_segments[i].getStart(),shadow_color,alpha));
		raytree_verts.push_back(VBOPosColor4(shadow_segments[i].getEnd(),shadow_color,alpha));
		raytree_edge_indices.push_back(VBOIndexedEdge(count,count+1));
		count+=2;
	}
	for(i = 0;i < reflected_segments.size();++i){
		raytree_verts.push_back(VBOPosColor4(reflected_segments[i].getStart(),reflected_color,alpha));
		raytree_verts.push_back(VBOPosColor4(reflected_segments[i].getEnd(),reflected_color,alpha));
		raytree_edge_indices.push_back(VBOIndexedEdge(count,count+1));
		count+=2;
	}
	for(i = 0;i < single_scatter_segments.size();++i){
		raytree_verts.push_back(VBOPosColor4(single_scatter_segments[i].getStart(),single_scatter_color,alpha));
		raytree_verts.push_back(VBOPosColor4(single_scatter_segments[i].getEnd(),single_scatter_color,alpha));
		raytree_edge_indices.push_back(VBOIndexedEdge(count,count+1));
		count+=2;
	}
	for(i = 0;i < multiple_scatter_segments.size();++i){
		raytree_verts.push_back(VBOPosColor4(multiple_scatter_segments[i].getStart(),multiple_scatter_color,alpha));
		raytree_verts.push_back(VBOPosColor4(multiple_scatter_segments[i].getEnd(),multiple_scatter_color,alpha));
		raytree_edge_indices.push_back(VBOIndexedEdge(count,count+1));
		count+=2;
	}

	int ne = raytree_edge_indices.size();

	cleanupVBOs();

	if(ne > 0) {
		glBindBuffer(GL_ARRAY_BUFFER,raytree_verts_VBO);
		glBufferData(GL_ARRAY_BUFFER,sizeof(VBOPosColor4) * ne *2,&raytree_verts[0],GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,raytree_edge_indices_VBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(VBOIndexedEdge) * ne,&raytree_edge_indices[0],GL_STATIC_DRAW);
	}
}

//Draws the raytree
void RayTree::drawVBOs() {
	int ne=raytree_edge_indices.size();
	if(ne == 0){
		return;
	}

	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	glDisable(GL_LIGHTING);
	glLineWidth(2);
	glBindBuffer(GL_ARRAY_BUFFER,raytree_verts_VBO);
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3,GL_FLOAT,sizeof(VBOPosColor4),BUFFER_OFFSET(0));
	glEnableClientState(GL_COLOR_ARRAY);
	glColorPointer(4,GL_FLOAT,sizeof(VBOPosColor4),BUFFER_OFFSET(12));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,raytree_edge_indices_VBO);
	glDrawElements(GL_LINES,ne*2,GL_UNSIGNED_INT,BUFFER_OFFSET(0));
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
	glEnable(GL_DEPTH_TEST);
}

void RayTree::cleanupVBOs() {
	glDeleteBuffers(1, &raytree_verts_VBO);
	glDeleteBuffers(1, &raytree_edge_indices_VBO);
}