#include "canvas.h"

#include <cassert>
#include "camera.h"
#include "matrix.h"

//Copied from RPI Graphics hw3

Camera::Camera(const Vec &c,const Vec &poi,const Vec &u){
	camera_position = c;
	point_of_interest = poi;
	up = u;
	up.Normalize();
}

//Uses cameras member variables to put place it in the scene
void Camera::glPlaceCamera(void){
	gluLookAt(camera_position.x(),camera_position.y(),camera_position.z(),point_of_interest.x(),point_of_interest.y(),point_of_interest.z(),up.x(),up.y(),up.z());
}

//Moves the camera a specified distance backwards or forwards
void Camera::dollyCamera(double dist){
	Vec dif= camera_position - point_of_interest ;
	double d=dif.Length();
	dif.Normalize();
	d *= pow(1.003,dist);
	camera_position = point_of_interest + dif *d;
}

//Moves the camera parallel to the image plane
void Camera::truckCamera(double dx,double dy) {
	Vec dif= camera_position - point_of_interest ;
	double d=dif.Length();
	Vec trans = (d*.0007)*(getHorizontal()*dx + getScreenUp()*dy);
	camera_position+=trans;
	point_of_interest+=trans;
}

//Rotates about image center
void Camera::rotateCamera(double rx,double ry){
	double tiltAngle = acos(up.Dot(getDirection()));
	//Avoid having up and direction aligned
	if (tiltAngle > 3.13){
		ry=tiltAngle-3.13;
	}else if( tiltAngle-ry < .01){
		ry= tiltAngle - .01;
	}
	Matrix rotMat;
	rotMat.setToIdentity();
	rotMat *= Matrix::MakeTranslation(point_of_interest);
	rotMat *= Matrix::MakeAxisRotation(up,rx);
	rotMat *= Matrix::MakeAxisRotation(getHorizontal(),ry);
	rotMat *= Matrix::MakeTranslation( -1*point_of_interest);
	rotMat.Transform(camera_position);
}

PerspectiveCamera::PerspectiveCamera(const Vec &c,const Vec &poi,const Vec &u,double a) : Camera(c,poi,u) {
	angle = a;
}

//Makes the previewer use the perspective camera and changes the width and height of the window
void PerspectiveCamera::glInit(int w,int h){
	width = w;
	height = h;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	double aspect = double(width)/double(height);
	double asp_angle = angle * 180/M_PI;
	if(aspect > 1){
		asp_angle /= aspect;
	}
	double dist_to_poi = (point_of_interest-camera_position).Length();
	gluPerspective(asp_angle,aspect,dist_to_poi*.1,dist_to_poi*100.0);
}

//Zooms in on the interest point
void PerspectiveCamera::zoomCamera(double dist) {
	angle *= pow(1.003,dist);
	glInit(width,height);
}

//Generates a ray for a given pixel coordinates
Ray PerspectiveCamera::generateRay(double x,double y) {
	Vec screenCenter = camera_position + getDirection();
	double screenHeight = 2 * tan(angle/2.0);
	Vec xAxis = getHorizontal() * screenHeight;
	Vec yAxis = getScreenUp() * screenHeight;
	Vec lowerleft = screenCenter - .5*xAxis - .5*yAxis;
	Vec screenPoint = lowerleft + x*xAxis + y*yAxis;
	Vec dir = screenPoint - camera_position;
	dir.Normalize();
	return Ray(camera_position,dir);
}

//Reads in a camera
std::istream& operator>>(std::istream &istr,PerspectiveCamera &c){
	std::string token;

	istr >> token; assert(token == "{");
	istr >> token; assert(token == "camera_position");
	istr >> c.camera_position;
	istr >> token; assert(token == "point_of_interest");
	istr >> c.point_of_interest;
	istr >> token; assert(token == "up");
	istr >> c.up;
	istr >> token; assert(token == "angle");
	istr >> c.angle;
	istr >> token; assert(token == "}");

	return istr;
}
