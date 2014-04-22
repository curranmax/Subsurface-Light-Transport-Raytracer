#include "sphere.h"
#include "inputparser.h"
#include "vertex.h"
#include "mesh.h"
#include "ray.h"
#include "hit.h"
#include "utils.h"

#include <iostream>

//Adapted from RPI Graphics hw3

//Finds intersection between sphere and ray
bool Sphere::intersect(const Ray &r,Hit &h) const{
	Vec Ro = r.getOrigin();
	Vec Rd = r.getDirection();

	double a=Rd.Dot(Rd);
	double b=2*Rd.Dot(Ro-center);
	double c=(Ro-center).Dot(Ro-center)-radius*radius;
	double determinant=b*b-4.0*a*c;
	if(determinant>0){
		double t1 = (-b + sqrt(determinant)) / (2.0 * a);
		double t2 = (-b - sqrt(determinant)) / (2.0 * a);
		double t = t2;
		if(t1 < t || t < INTERSECT_EPSILON){
			if(t1 < INTERSECT_EPSILON){
				return false;
			}
			t=t1;
		}
		if(h.getT()>t){
			Vec norm=(r.pointAtT(t)-center);
			norm.Normalize();
			h.set(t,material,norm);
			h.setObject(this);
		}
		return true;
	}
	return false;
}

//helper for adding rasterized faces
Vec computeSpherePoint(double s,double t,const Vec c,double r){
	double angle=2*M_PI*s;
	double y=-cos(M_PI*t);
	double factor=sqrt(1-y*y);
	double x=factor*cos(angle);
	double z=factor*-sin(angle);
	Vec ans(x,y,z);
	return ans*r+c;
}

//Adds a rasterized version of the sphere to the mesh
void Sphere::addRasterizedFaces(Mesh *m,InputParser *input){
	int h=input->sphere_horizontal;
	int v=input->sphere_vertical;
	int i,j;
	int va,vb,vc,vd;
	Vertex *a,*b,*c,*d;
	int offset = m->numVertices();

	m->addVertex(center+Vec(0,-radius,0));
	for(j=1;j<v;++j){
		for(i=0;i<h;++i){
			double s=i/double(h);
			double t=j/double(v);
			m->addVertex(computeSpherePoint(s,t,center,radius));
		}
	}
	m->addVertex(center+Vec(0,radius,0));

	for(j=1;j<v-1;++j){
		for(i=0;i<h;++i){
			va= 1 + i + h*(j-1);
			vb= 1 + (i+1)%h + h*(j-1);
			vc= 1 + i + h*(j);
			vd= 1 + (i+1)%h + h*(j);
			a=m->getVertex(offset + va);
			b=m->getVertex(offset + vb);
			c=m->getVertex(offset + vc);
			d=m->getVertex(offset + vd);
			m->addFace(a,b,d,c,material,true);
		}
	}


	for(i=0;i<h;i+=2){
		va=0;
		vb=1+i;
		vc=1+(i+1)%h;
		vd=1+(i+2)%h;
		a=m->getVertex(offset + va);
		b=m->getVertex(offset + vb);
		c=m->getVertex(offset + vc);
		d=m->getVertex(offset + vd);
		m->addFace(d,c,b,a,material,true);
		va=1+h*(v-1);
		vb=1+i+h*(v-2);
		vc=1+(i+1)%h+h*(v-2);
		vd=1+(i+2)%h+h*(v-2);
		a=m->getVertex(offset + va);
		b=m->getVertex(offset + vb);
		c=m->getVertex(offset + vc);
		d=m->getVertex(offset + vd);
		m->addFace(b,c,d,a,material,true);
	}
}

Vec Sphere::getRandomPoint() const{
	Vec point;
	double x,y,z;
	do{
		x = GLOBAL_mtrand.rand() * 2 * radius - radius;
		y = GLOBAL_mtrand.rand() * 2 * radius - radius;
		z = GLOBAL_mtrand.rand() * 2 * radius - radius;	
	}while(x * x + y * y + z * z > radius * radius);
	point = Vec(x,y,z);
	point.Normalize();
	point = point * radius + center;
	return point;
}

Vec Sphere::getRandomPoint(const Vec &p, double valid_radius) const{
	Vec point;
	double x,y,z;
	do{
		do{
			x = GLOBAL_mtrand.rand() * 2 * radius - radius;
			y = GLOBAL_mtrand.rand() * 2 * radius - radius;
			z = GLOBAL_mtrand.rand() * 2 * radius - radius;	
		}while(x * x + y * y + z * z > radius * radius);
		point = Vec(x,y,z);
		point.Normalize();
		point = point * radius + center;
	}while((point - p).Length() > valid_radius + RANDOM_POINT_EPSILON);
	return point;
}

Vec Sphere::getRandomPoint(const Vec &p, double radius1, double radius2) const{
	fixRadius(radius1,radius2);

	Vec point;
	double x,y,z;
	int num_try = 0;
	do{
		do{
			x = GLOBAL_mtrand.rand() * 2 * radius - radius;
			y = GLOBAL_mtrand.rand() * 2 * radius - radius;
			z = GLOBAL_mtrand.rand() * 2 * radius - radius;	
		}while(x * x + y * y + z * z > radius * radius);
		point = Vec(x,y,z);
		point.Normalize();
		point = point * radius + center;
		num_try++;
	}while(((point - p).Length() > radius1 || (point - p).Length() < radius2) && num_try < MAX_RANDOM_TRIES);

	if(x >= MAX_RANDOM_TRIES){
		return getRandomPoint(p,radius1);
	}
	
	return point;
}
