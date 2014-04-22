#include "cone.h"
#include "matrix.h"
#include "utils.h"

//Finds the intersection between a ray and the cone
//Stores information about intersection in the Hit data structure if its the first intersection
bool Cone::intersect(const Ray &r,Hit &h) const{
	//Find intersection between ray and infinite cone of same parameters
	Vec p = r.getOrigin();
	Vec d = r.getDirection();
	double gamma = cos(theta);
	Matrix M;
	M.setToIdentity();
	M*=gamma*gamma;
	M=Matrix::MakeOuterProduct(dir) - M;
	M.set(3,3,1);
	Vec delta = p - origin;

	double a = d.Dot(M*d);
	double b = 2*d.Dot(M*delta);
	double c = delta.Dot(M*delta);
	double determinant=b*b-4.0*a*c;
	bool intersect = false;
	double t;
	Vec norm;
	//Checks if there are any real intersections with infinite cone
	if(determinant>0){
		intersect = true;
		t = (-b + sqrt(determinant)) / (2.0 * a);
		double t2 = (-b - sqrt(determinant)) / (2.0 * a);
		//Finds the intersection with a smaller t, and checks to see if the intersection is with the portion of the specified cone
		if(t2 < t || t < INTERSECT_EPSILON || !validIntersection(t,r)){
			t = t2;
			if(t < INTERSECT_EPSILON || !validIntersection(t,r)){
				intersect = false;
			}
		}
		if(intersect){
			if(h.getT()>t){
				norm = getNormal(r.getDirection()*t+r.getOrigin());
				norm.Normalize();
				if(norm.Dot(d)>0){
					norm *= -1;
				}
			}
		}
	}
	//checks for intersection with the end cap
	double num = dir.Dot(origin+dir*height-p);
	double den = dir.Dot(d);
	if(den!=0){
		double tc = num/den;
		double dist = (d*tc+p-(origin+dir*height)).Length();
		if(radius()>=dist  && tc > INTERSECT_EPSILON && tc < t){
			t = tc;
			intersect = true;
			norm = dir;
			if(norm.Dot(d)>0){
				norm *= -1;
			}
		}
	}
	//Checks if the intersection is closer than the one stored in the hit data structure
	if(intersect && h.getT() > t){
		h.set(t,material,norm);
		h.setObject(this);
	}
	return intersect;
}

//Adds rasterized quads for the cone to the mesh
void Cone::addRasterizedFaces(Mesh *m,InputParser *input){
	Vec raxis = rasterAxis();
	Vec center = origin + dir * height;

	int offset = m->numVertices();
	m->addVertex(origin);
	for(int i=0;i<input->cone_points;++i){
		m->addVertex(computeCirclePoint(radius(),i/double(input->cone_points),raxis,dir,center));
	}
	m->addVertex(center);

	Vertex *o = m->getVertex(offset);
	Vertex *c = m->getVertex(input->cone_points + 1 + offset);

	int ai, bi, ci;
	Vertex *av, *bv, *cv;
	for(int i=1; i <= input->cone_points ; i += 2){
		ai = i + offset;
		bi = i + 1 + offset;
		ci = (i + 1) % input->cone_points + 1 + offset;
		av = m->getVertex(ai);
		bv = m->getVertex(bi);
		cv = m->getVertex(ci);
		m->addFace(o,cv,bv,av,material,true);
		m->addFace(c,av,bv,cv,material,true);
	}
}

//Gets a random point on the cone
Vec Cone::getRandomPoint() const{
	double rad = radius();

	Vec point;
	double rv_choose = GLOBAL_mtrand.rand() * (SA_end + SA_cone);
	double x;
	double y;
	do{
		x = GLOBAL_mtrand.rand() * 2 * rad - rad;
		y = GLOBAL_mtrand.rand() * 2 * rad - rad;	
	}while(x * x + y * y > rad * rad);
	Vec xy_point = Vec(x,y,0);
	point = rotationMatrix * xy_point + origin;
	if(rv_choose <= SA_end){
		point += dir * height;
	}else{
		double r = (point - origin).Length();
		point += dir * height * r / rad ;
	}
	return point;
}

//Gets a random point within valid_radius of point p
Vec Cone::getRandomPoint(const Vec &p, double valid_radius) const{
	double rad = radius();

	Vec point;
	double rv_choose = GLOBAL_mtrand.rand() * (SA_end + SA_cone);
	double x;
	double y;
	do{
		do{
			x = GLOBAL_mtrand.rand() * 2 * rad - rad;
			y = GLOBAL_mtrand.rand() * 2 * rad - rad;	
		}while(x * x + y * y > rad * rad);
		Vec xy_point = Vec(x,y,0);
		point = rotationMatrix * xy_point + origin;
		if(rv_choose <= SA_end){
			point += dir * height;
		}else{
			double r = (point - origin).Length();
			point += dir * height * r / rad ;
		}
	}while((point - p).Length() > valid_radius + RANDOM_POINT_EPSILON);
	return point;
}

//Finds a point between between the two distances to point p
Vec Cone::getRandomPoint(const Vec &p, double radius1, double radius2) const{
	fixRadius(radius1,radius2);

	double rad = radius();

	Vec point;
	double rv_choose = GLOBAL_mtrand.rand() * (SA_end + SA_cone);
	double x;
	double y;
	double num_try = 0;
	do{
		do{
			x = GLOBAL_mtrand.rand() * 2 * rad - rad;
			y = GLOBAL_mtrand.rand() * 2 * rad - rad;	
		}while(x * x + y * y > rad * rad);
		Vec xy_point = Vec(x,y,0);
		point = rotationMatrix * xy_point + origin;
		if(rv_choose <= SA_end){
			point += dir * height;
		}else{
			double r = (point - origin).Length();
			point += dir * height * r / rad ;
		}
		num_try++;
	}while(((point - p).Length() > radius1 || (point - p).Length() < radius2) && num_try < MAX_RANDOM_TRIES);

	if(x >= MAX_RANDOM_TRIES){
		return getRandomPoint(p,radius1);
	}

	return point;
}
