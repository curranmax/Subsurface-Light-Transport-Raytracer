
#include "face.h"
#include "hit.h"
#include "ray.h"
#include "matrix.h"
#include "utils.h"

//Copied from RPI Graphics hw3

//Gets normal of the plane through three of points
inline Vec normalOfTriangle(const Vec &a,const Vec &b,const Vec &c){
	Vec difba=b-a;
	Vec difcb=c-b;
	Vec normal;
	Vec::Cross(normal,difba,difcb);
	normal.Normalize();
	return normal;
}

//Averages the normal of two sets of three vertices of the face
Vec Face::computeNormal() const {
	Vec a = (*this)[0]->get();
	Vec b = (*this)[1]->get();
	Vec c = (*this)[2]->get();
	Vec d = (*this)[3]->get();
	return .5 * (normalOfTriangle(a,b,c)+normalOfTriangle(a,c,d));
}

//Gets a random point on the quad
Vec Face::RandomPoint() const {
	Vec a = (*this)[0]->get();
	Vec b = (*this)[1]->get();
	Vec c = (*this)[2]->get();
	Vec d = (*this)[3]->get();
	double xw = GLOBAL_mtrand.rand();
	double yw = GLOBAL_mtrand.rand();
	Vec ab = xw * a + (1-xw) * b;
	Vec cd = xw * d + (1-xw) * c;
	Vec abcd = yw * ab + (1-yw) * cd;
	return abcd;
}

//Finds the intersection with the plane by intersecting with the triangle between two sets of three vertices of the quad
bool Face::intersect(const Ray &r, Hit &h, bool intersect_backfacing) const{
	Vertex *a = (*this)[0];
	Vertex *b = (*this)[1];
	Vertex *c = (*this)[2];
	Vertex *d = (*this)[3];
	return triangle_intersect(r,h,a,b,c,intersect_backfacing) || triangle_intersect(r,h,a,c,d,intersect_backfacing);
}

//Barycentric Coordinates
bool Face::triangle_intersect(const Ray &r,Hit &h,Vertex *av,Vertex *bv, Vertex *cv,bool intersect_backfacing) const{

	//Computes intersection with the plane of the triangle
	Hit h2 = Hit(h);
	if(!plane_intersect(r,h2,intersect_backfacing)){
		return false;
	}
	Vec a=av->get(),b=bv->get(),c=cv->get();

	Vec Ro = r.getOrigin();
	Vec Rd = r.getDirection();

	double detA = Matrix::det3x3(
		a.x()-b.x(),	a.x()-c.x(),	Rd.x(),
		a.y()-b.y(),	a.y()-c.y(),	Rd.y(),
		a.z()-b.z(),	a.z()-c.z(),	Rd.z());

	if(fabs(detA)<=.000001){
		return false;
	}

	double beta = Matrix::det3x3(
		a.x()-Ro.x(),	a.x()-c.x(),	Rd.x(),
		a.y()-Ro.y(),	a.y()-c.y(),	Rd.y(),
		a.z()-Ro.z(),	a.z()-c.z(),	Rd.z()) / detA;

	double gamma = Matrix::det3x3(
		a.x()-b.x(),	a.x()-Ro.x(),	Rd.x(),
		a.y()-b.y(),	a.y()-Ro.y(),	Rd.y(),
		a.z()-b.z(),	a.z()-Ro.z(),	Rd.z()) / detA;

	if(beta >= -0.00001 && beta <= 1.00001 && gamma >= -0.00001 && gamma <=1.00001 && beta + gamma <= 1.00001){	
		h = h2;
		return true;
	}
	return 0;
}

//Finds the intersection between the ray and the infinite plane
bool Face::plane_intersect(const Ray &r,Hit &h,bool intersect_backfacing) const{
	Vec normal =computeNormal();
	double d = normal.Dot((*this)[0]->get());

	double numer = d - r.getOrigin().Dot(normal);
	double denom = r.getDirection().Dot(normal);

	if(denom == 0){
		return false;
	}

	if(!intersect_backfacing && normal.Dot(r.getDirection()) >= 0){
		return false;
	}
	double t=numer/denom;

	if(t > INTERSECT_EPSILON && t < h.getT()){
		h.set(t,this->getMaterial(),normal);
		h.setFaceCollide();
		return true;
	}
	return false;
}

//Gets area of the quad by adding the area of two triangles that make up the quad
double Face::getArea() const {
	Vec a = (*this)[0]->get();
	Vec b = (*this)[1]->get();
	Vec c = (*this)[2]->get();
	Vec d = (*this)[3]->get();
	return 	AreaOfTriangle(	dist2Vec(a,b),
							dist2Vec(a,c),
							dist2Vec(b,c)) +
			AreaOfTriangle(	dist2Vec(c,d),
							dist2Vec(a,d),
							dist2Vec(a,c));
}


