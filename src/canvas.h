#ifndef _CANVAS_H_
#define _CANVAS_H_

#define GL_GLEXT_PROTOTYPES
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <string>

//Adapted from RPI Graphics hw3

class InputParser;
class Mesh;
class RayTracer;
class Vec;

class Canvas{
public:
	static void initialize(InputParser *_input,Mesh *_mesh,RayTracer *_raytracer);
private:
	static void initializeLighting();
	static void display(void);
	static void reshape(int w,int h);
	static void mouse(int button,int state, int x, int y);
	static void motion(int x,int y);
	static void keyboard(unsigned char key, int x, int y);
	static void idle();

	static int DrawNextPixel();
	static Vec TraceRay(double i,double j);

	//State of mouse and keyboard input
	static int mouseButton;
	static int mouseX;
	static int mouseY;
	static bool shiftPressed;
	static bool controlPressed;
	static bool altPressed;

	//State of raytracing pixel and resolution
	static int raytracing_x;
	static int raytracing_y;
	static int raytracing_skip;
	
	//Data structures to hold information about scene and other paramters
	static InputParser *input;
	static Mesh *mesh;
	static RayTracer *raytracer;
};

int HandleGLError(const std::string &message = "");

#endif
