#ifndef _MATERIAL_H_
#define _MATERIAL_H_

#include "canvas.h"
#include <string>
#include <assert.h>
#include "vector.h"

class Ray;
class Hit;
class InputParser;

//Copied from RPI Graphics hw3

class Material {
public:
	Material(const Vec &d_color,const Vec &r_color,const Vec &e_color, const Vec &sig_s,const Vec &sig_a,double index){
		difColor = d_color;
		refColor = r_color;
		emitColor = e_color;
		translucent = false;
		sigma_s = Vec(0,0,0);
		sigma_a = Vec(0,0,0);
		sigma_t = Vec(0,0,0);
		if(sig_s.Length() + sig_a.Length() > .0001){
			sigma_s = sig_s;
			sigma_a = sig_a;
			sigma_t = sig_a + sig_s;
			sigma_tr = (3 * sigma_a * sigma_t).Sqrt();
			translucent = true;
		}
		index_of_refraction = index;
	}
	~Material() {}

	const Vec& getDiffuseColor() const { return difColor; }
	const Vec getDiffuseColor(double s,double t) const;
	const Vec& getReflectiveColor() const { return refColor; }
	const Vec& getEmittedColor() const { return emitColor; }
	bool isTranslucent() const { return translucent; }
	const Vec& getSigmaS() const { return sigma_s; }
	const Vec& getSigmaA() const { return sigma_a; }
	const Vec& getSigmaT() const { return sigma_t; }
	const Vec& getSigmaTR() const { return sigma_tr; }
	double getIndexOfRefraction() const { return index_of_refraction; }

	Vec Shade(const Ray &ray,const Hit &hit,const Vec &dirToLight, const Vec &lcolor,InputParser *input) const;

protected:
	Material() { exit(0); }
	Material(const Material&) { exit(0);}
	const Material& operator=(const Material&) { exit(0); }

	void ComputeAverageTextureColor();

	Vec difColor;
	Vec refColor;
	Vec emitColor;
	Vec sigma_a;
	Vec sigma_s;
	Vec sigma_t;
	Vec sigma_tr;
	bool translucent;
	double index_of_refraction;
};

#endif
