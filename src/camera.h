#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "vector.h"
#include "ray.h"
#include <iostream>

//Copied from RPI Graphics hw3

class Camera {
public:
	Camera(const Vec &c,const Vec &poi,const Vec &u);
	virtual ~Camera() {}

	virtual Ray generateRay(double x,double y) =0;

	virtual void glInit(int w,int h) = 0;
	void glPlaceCamera(void);
	void dollyCamera(double dist);
	virtual void zoomCamera(double dist) = 0;
	void truckCamera(double dx,double dy);
	void rotateCamera(double rx,double ry);
protected:
	//Helpers to get camera coordinate axes
	Vec getHorizontal() const {
		Vec ans;
		Vec::Cross(ans,getDirection(),up);
		ans.Normalize();
		return ans;
	}
	Vec getScreenUp() const{
		Vec ans;
		Vec::Cross(ans,getHorizontal(),getDirection());
		return ans;
	}
	Vec getDirection() const {
		Vec ans=point_of_interest - camera_position;
		ans.Normalize();
		return ans;
	}

	Vec point_of_interest;
	Vec camera_position;
	Vec up;
	int width;
	int height;
};

class PerspectiveCamera : public Camera {
public:
	PerspectiveCamera(const Vec &c= Vec(0,0,1),const Vec &poi = Vec(0,0,0),const Vec &u = Vec(0,1,0),double a = 45);

	Ray generateRay(double x, double y);

	void glInit(int w,int h);
	void zoomCamera(double dist);

	friend std::istream& operator>>(std::istream &istr,PerspectiveCamera &c);

private:
	double angle;
};

#endif
