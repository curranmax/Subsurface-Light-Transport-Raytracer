#include "material.h"
#include "ray.h"
#include "hit.h"
#include "inputparser.h"

Vec Material::Shade(const Ray &ray,const Hit &hit,const Vec &dirToLight, const Vec &lcolor,InputParser *input) const{
	//Vec p = ray.pointAtT(hit.getT());
	Vec n = hit.getNormal();
	Vec e = ray.getDirection()*-1.0f;
	Vec l = dirToLight;

	Vec ans=Vec(0,0,0);

	//Emitted
	ans += getEmittedColor();

	//Diffuse
	double dot_nl = n.Dot(l);
	if(dot_nl < 0){
		dot_nl = 0;
	}
	ans += lcolor * getDiffuseColor() * dot_nl;

	//Specular
	Vec specColor = refColor;
	double exponent = 100;

	Vec r =(l*-1.0f) + n * (2 * dot_nl);
	r.Normalize();
	double dot_er= e.Dot(r);
	if (dot_er < 0){
		dot_er = 0;
	}
	ans+= lcolor * specColor * pow(dot_er,exponent) * dot_nl;

	return ans;
}
