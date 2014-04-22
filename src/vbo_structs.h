#ifndef _VBO_STRUCTS_H_
#define _VBO_STRUCTS_H_

//Copied from RPI Graphics hw3
//Defines different types of VBOs for use with rendering in OpenGl

//Helper for VBOs
#define BUFFER_OFFSET(i) ((char *)NULL + (i))

struct VBOPos{
	VBOPos() { }
	VBOPos(const Vec &v) {
		x=v.x();
		y=v.y();
		z=v.z();
	}
	float x, y, z;
};

struct VBOPosColor {
	VBOPosColor() { }
	VBOPosColor(const Vec &v, const Vec &c){
		x=v.x();
		y=v.y();
		z=v.z();
		r=c.r();
		g=c.g();
		b=c.b();
	}
	float x, y, z;
	float r, g, b;
};

struct VBOPosColor4 {
	VBOPosColor4() { }
	VBOPosColor4(const Vec &v, const Vec &c, double _alpha){
		x=v.x();
		y=v.y();
		z=v.z();
		r=c.r();
		g=c.g();
		b=c.b();
		alpha=_alpha;
	}
	float x, y, z;
	float r, g, b, alpha;
};

struct VBOPosNormal {
	VBOPosNormal() { }
	VBOPosNormal(const Vec &v,const Vec &n){
		x=v.x();
		y=v.y();
		z=v.z();
		nx=n.x();
		ny=n.y();
		nz=n.z();
	}
	float x, y, z;
	float nx, ny, nz;
};

struct VBOPosNormalColor {
	VBOPosNormalColor() { }
	VBOPosNormalColor(const Vec &v,const Vec &n,const Vec &c){
		x=v.x();
		y=v.y();
		z=v.z();
		nx=n.x();
		ny=n.y();
		nz=n.z();
		r=c.r();
		g=c.g();
		b=c.b();
	}
	float x, y, z;
	float nx, ny, nz;
	float r, g, b;
};

struct VBOPosNormalColorTexture {
	VBOPosNormalColorTexture() { }
	VBOPosNormalColorTexture(const Vec &v,const Vec &n,const Vec &c,float _s,float _t){
		x=v.x();
		y=v.y();
		z=v.z();
		nx=n.x();
		ny=n.y();
		nz=n.z();
		r=c.r();
		g=c.g();
		b=c.b();
		s=_s;
		t=_t;
	}
	float x, y, z;
	float nx, ny, nz;
	float r, g, b;
	float s, t;
};

struct VBOIndexedEdge{
	VBOIndexedEdge() { }
	VBOIndexedEdge(unsigned int a,unsigned int b){
		verts[0]=a;
		verts[1]=b;
	}
	unsigned int verts[2];
};

struct VBOIndexedTri{
	VBOIndexedTri() { }
	VBOIndexedTri(unsigned int a,unsigned int b,unsigned int c){
		verts[0]=a;
		verts[1]=b;
		verts[2]=c;
	}
	unsigned int verts[3];
};

struct VBOIndexedQuad{
	VBOIndexedQuad() { }
	VBOIndexedQuad(unsigned int a,unsigned int b,unsigned int c,unsigned int d){
		verts[0]=a;
		verts[1]=b;
		verts[2]=c;
		verts[3]=d;
	}
	unsigned int verts[4];
};

#endif
