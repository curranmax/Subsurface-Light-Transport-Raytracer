#include "cube.h"
#include "utils.h"

//Computes intersection with the ray and cube by checking intersection with each face of the cube
bool Cube::intersect(const Ray &r,Hit &h) const{
	bool intersect=false;
	for(unsigned int i=0; i < fs.size(); ++i){
		if(fs[i]->intersect(r,h,true)){
			h.setObject(this);
			intersect = true;
		}
	}
	return intersect;
}

//Creates faces which are used in the previewer and for intersection
void Cube::addRasterizedFaces(Mesh *m,InputParser *input){
	Vertex *v1,*v2,*v3,*v4;

	//top
	v1 = m->getVertex(vs[0]);
	v2 = m->getVertex(vs[1]);
	v3 = m->getVertex(vs[2]);
	v4 = m->getVertex(vs[3]);

	fs.push_back(m->addFace(v1,v2,v3,v4,material,true));

	//bottom
	v1 = m->getVertex(vs[4]);
	v2 = m->getVertex(vs[7]);
	v3 = m->getVertex(vs[6]);
	v4 = m->getVertex(vs[5]);

	fs.push_back(m->addFace(v1,v2,v3,v4,material,true));

	//sides
	int i1,i2,i3,i4;
	for(int i=0;i<4;++i){
		i1 = i;
		i2 = i + 4;
		i3 = (i + 1) % 4 + 4;
		i4 = (i + 1) % 4;
		v1 = m->getVertex(vs[i1]);
		v2 = m->getVertex(vs[i2]);
		v3 = m->getVertex(vs[i3]);
		v4 = m->getVertex(vs[i4]);
		fs.push_back(m->addFace(v1,v2,v3,v4,material,true));
	}

	computeSA();
}

//Computes the surface for each face
void Cube::computeSA(){
	total_SA = 0;
	for(int i=0;i<6;++i){
		double SA_temp = fs[i]->getArea();
		SA_faces.push_back(SA_temp);
		total_SA += SA_temp;
	}
}

//Gets a random point on the cube
Vec Cube::getRandomPoint() const{
	Vec point;
	double rv_choose = GLOBAL_mtrand.rand() * total_SA;
	for(int i=0;i<6;++i){
		if(i==5 || rv_choose < SA_faces[i]){
			point = fs[i]->RandomPoint();
			break;
		}
		rv_choose -= SA_faces[i];
	}

	return point;
}

//Gets a random point with a given distance to point p
Vec Cube::getRandomPoint(const Vec &p, double valid_radius) const{
	Vec point;
	double rv_choose;
	do{
		rv_choose = GLOBAL_mtrand.rand() * total_SA;
		for(int i=0;i<6;++i){
			if(i==5 || rv_choose < SA_faces[i]){
				point = fs[i]->RandomPoint();
				break;
			}
			rv_choose -= SA_faces[i];
		}
	}while((point - p).Length() > valid_radius + RANDOM_POINT_EPSILON);

	return point;
}

//Gets a random point between the given distances to point p
Vec Cube::getRandomPoint(const Vec &p, double radius1, double radius2) const{
	fixRadius(radius1,radius2);

	Vec point;
	double rv_choose;
	int x = 0;
	do{
		rv_choose = GLOBAL_mtrand.rand() * total_SA;
		for(int i=0;i<6;++i){
			if(i==5 || rv_choose < SA_faces[i]){
				point = fs[i]->RandomPoint();
				break;
			}
			rv_choose -= SA_faces[i];
		}
		x++;
	}while(((point - p).Length() > radius1 || (point - p).Length() < radius2) && x < MAX_RANDOM_TRIES);

	if(x >= MAX_RANDOM_TRIES){
		return getRandomPoint(p,radius1);
	}

	return point;
}
