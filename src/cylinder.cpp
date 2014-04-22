#include "cylinder.h"
#include "utils.h"
#include "matrix.h"
#include "mesh.h"
#include "inputparser.h"

//Checks for intersection between the ray and the cylinder
bool Cylinder::intersect(const Ray &r,Hit &h) const {
	//Checks intersection between infinite cylinder and the ray
	bool intersect = false;
	double t = -1;
	Vec norm;
	Vec d = Direction();
	Vec rd = r.getDirection();
	Vec ro = r.getOrigin();
	Vec o = ro - center1;
	Vec rdd = rd - (rd.Dot(d)) * d;
	Vec od = o - (o.Dot(d)) * d;

	double a = rdd.Dot(rdd);
	double b = 2 * rdd.Dot(od);
	double c = od.Dot(od) - radius * radius;

	double determinant = b * b - 4 * a * c;
	//Checks for real intersections and that the intersection is in the bounds of the cylinder
	if(determinant > 0){
		intersect = true;
		t = (-b - sqrt(determinant))/(2 * a);
		double t2 = (-b + sqrt(determinant))/(2 * a);
		if(t < INTERSECT_EPSILON || t2 < t || !collideInBounds(t,r)){
			t=t2;
			if(t < INTERSECT_EPSILON || !collideInBounds(t,r)){
				intersect = false;
			}
		}
		if(intersect){
			Vec point = rd * t + ro;
			norm =  point - getClosestPointOnCylinder(point);
			if(norm.Dot(rd)>0){
				norm*=-1;
			}
			norm.Normalize();
		}
	} else {
		intersect = false;
	}

	//Checks for intersections between the two end caps
	double n1 = d.Dot(center1 - ro);
	double d1 = rd.Dot(d);
	if(d1 != 0){
		double t1 = n1 / d1;
		if(radius >= (rd*t1+ro-center1).Length() && t1 > INTERSECT_EPSILON && t1 < t){
			t=t1;
			norm = -1 * d;
			intersect = true;
		}
	}
	double n2 = d.Dot(center2 - ro);
	double d2 = rd.Dot(d);
	if(d2 != 0){
		double t2 = n2 / d2;
		if(radius >= (rd*t2+ro-center2).Length() && t2 > INTERSECT_EPSILON && t2 < t){
			t=t2;
			norm = d;
			intersect = true;
		}
	}
	//Updates the intersection data if this is the closest so far
	if(intersect && h.getT() > t){
		h.set(t,material,norm);
		h.setObject(this);
	}
	return intersect;
}

//Adds a rasterized version to the mesh
void Cylinder::addRasterizedFaces(Mesh *m,InputParser *input){
	Vec axis = rasterAxis(), dir = Direction();
	int cp = input->cylinder_points;
	int offset = m->numVertices();

	m->addVertex(center1);
	m->addVertex(center2);
	for(int x = 0;x < cp;++x){
		double t = x/double(cp);
		m->addVertex(computeCirclePoint(radius,t,axis,dir,center1));
		m->addVertex(computeCirclePoint(radius,t,axis,dir,center2));
	}
	int ai, bi, ci, di;
	Vertex *a,*b,*c,*d;

	for(int i = 2;i < 2 * cp;i += 4){
		ai = 0 + offset;
		bi = i + offset;
		ci = i + 2 + offset;
		di = (i + 4) % (2 * cp) + offset;
		for(int j=0;j<2;++j,++ai,++bi,++ci,++di){
			a = m->getVertex(ai);
			b = m->getVertex(bi);
			c = m->getVertex(ci);
			d = m->getVertex(di);
			if(j==0){
				m->addFace(a,d,c,b,material,true);
			} else {
				m->addFace(a,b,c,d,material,true);
			}
		}
	}

	for(int i = 2; i <= 2 * cp; i+=2){
		ai = i + offset;
		bi = i + 1 + offset;
		ci = i + 3 + offset;
		di = i + 2 + offset;
		if(ci - offset > 2 * cp + 1){
			ci = 3 + offset;
			di = 2 + offset;
		}
		a = m->getVertex(ai);
		b = m->getVertex(bi);
		c = m->getVertex(ci);
		d = m->getVertex(di);
		m->addFace(a,d,c,b,material,true);
	}
}

//Gets a random point
Vec Cylinder::getRandomPoint() const{
	//Choose between top, middle, or bottom
	//Used in both to rotate from xy plane to plane with the tangent of Direction()
	Vec point;
	double rv_choose = GLOBAL_mtrand.rand() * (2 * SA_end + SA_mid);
	if(rv_choose <= SA_end * 2){
		//End cap
		//Find point in a circle in the xy plane of radius of "radius"
		//Then rotate and then translate to be end cap
		double x;
		double y;
		do{
			x = GLOBAL_mtrand.rand() * 2 * radius - radius;
			y = GLOBAL_mtrand.rand() * 2 * radius - radius;
		}while(x * x + y * y > radius * radius);		
		point = rotationMatrix * Vec(x,y,0);
		if(rv_choose <= SA_end){
			point += center1;
		}else{
			point += center2;
		}
	}else{
		//Finds random point on outside of cylinder
		double theta = GLOBAL_mtrand.rand() * 2 * M_PI;
		double h = GLOBAL_mtrand.rand() * Height();

		Vec xy = Vec(cos(theta)*radius,sin(theta)*radius,0);
		point = rotationMatrix * xy;

		point += Direction() * h + center1;
	}
	return point;
}

//Gets a random point within a given radius to point p
Vec Cylinder::getRandomPoint(const Vec &p, double valid_radius) const{
	Vec point;
	do{
		double rv_choose = GLOBAL_mtrand.rand() * (2 * SA_end + SA_mid);
		if(rv_choose <= SA_end * 2){
			//End cap
			//Find point in a circle in the xy plane of radius of "radius"
			//Then rotate and then translate to be end cap
			double x;
			double y;
			do{
				x = GLOBAL_mtrand.rand() * 2 * radius - radius;
				y = GLOBAL_mtrand.rand() * 2 * radius - radius;
			}while(x * x + y * y > radius * radius);		
			point = rotationMatrix * Vec(x,y,0);
			if(rv_choose <= SA_end){
				point += center1;
			}else{
				point += center2;
			}
		}else{
			double theta = GLOBAL_mtrand.rand() * 2 * M_PI;
			double h = GLOBAL_mtrand.rand() * Height();

			Vec xy = Vec(cos(theta)*radius,sin(theta)*radius,0);
			point = rotationMatrix * xy;

			point += Direction() * h + center1;
		}
	}while((point - p).Length() > radius + RANDOM_POINT_EPSILON);
	return point;
}

//Gets a ranodm point between the two given distances to point p
Vec Cylinder::getRandomPoint(const Vec &p, double radius1, double radius2) const{
	fixRadius(radius1,radius2);

	Vec point;
	do{
		double rv_choose = GLOBAL_mtrand.rand() * (2 * SA_end + SA_mid);
		if(rv_choose <= SA_end * 2){
			//End cap
			//Find point in a circle in the xy plane of radius of "radius"
			//Then rotate and then translate to be end cap
			double x;
			double y;
			do{
				x = GLOBAL_mtrand.rand() * 2 * radius - radius;
				y = GLOBAL_mtrand.rand() * 2 * radius - radius;
			}while(x * x + y * y > radius * radius);		
			point = rotationMatrix * Vec(x,y,0);
			if(rv_choose <= SA_end){
				point += center1;
			}else{
				point += center2;
			}
		}else{
			double theta = GLOBAL_mtrand.rand() * 2 * M_PI;
			double h = GLOBAL_mtrand.rand() * Height();

			Vec xy = Vec(cos(theta)*radius,sin(theta)*radius,0);
			point = rotationMatrix * xy;

			point += Direction() * h + center1;
		}
	}while((point - p).Length() > radius1 || (point - p).Length() < radius2);
	return point;
}

//Computes the surface areas of the end cap and cylinder, and computes the rotation matrix for making random points
void Cylinder::computeSA(){
	SA_end = M_PI * radius * radius;  //Surface area of the end caps
	SA_mid = M_PI * 2 * radius * Height(); //Surface area of the cylinder part

	Vec rot_axis;
	Vec::Cross(rot_axis,Direction(),Vec(0,0,1));
	double theta = acos(Direction().Dot(Vec(0,0,1)));
	rotationMatrix = Matrix::MakeAxisRotation(rot_axis,theta);
}
