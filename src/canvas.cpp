#include "canvas.h"
#include "inputparser.h"
#include "raytracer.h"
#include "mesh.h"
#include "camera.h"
#include "utils.h"
#include "hit.h"
#include "raytree.h"

//Adapted from RPI Graphics hw3

//initialize static variables
InputParser *Canvas::input=NULL;
Mesh *Canvas::mesh=NULL;
RayTracer* Canvas::raytracer=NULL;

int Canvas::mouseButton = 0;
int Canvas::mouseX = 0;
int Canvas::mouseY = 0;
bool Canvas::controlPressed = false;
bool Canvas::altPressed = false;
bool Canvas::shiftPressed = false;

int Canvas::raytracing_skip;
int Canvas::raytracing_x;
int Canvas::raytracing_y;


//Helper to deal with any errors with the OpenGL functions
int HandleGLError(const std::string &message);

//Intializes the previewer
//Copied from RPI Graphics hw3
void Canvas::initialize(InputParser *_input,Mesh *_mesh,RayTracer *_raytracer){
	input=_input;
	mesh=_mesh;
	raytracer=_raytracer;

	raytracing_skip = input->pixel_size;	

	//Initializes window
	glutInitWindowSize(input->width, input->height);
	glutInitWindowPosition(100,100);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGB);
	glutCreateWindow("OpenGL Viewer");
	HandleGLError("in glcanvas initialize");

	// basic rendering 
	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);
	glShadeModel(GL_SMOOTH);
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
	GLfloat ambient[] = { 0.2, 0.2, 0.2, 1.0 };
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);
	glCullFace(GL_BACK);
	glDisable(GL_CULL_FACE);

	//Callback functions
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutKeyboardFunc(keyboard);
	glutIdleFunc(idle);

	RayTree::initializeVBOs();
	raytracer->initializeVBOs();

	RayTree::setupVBOs();
	raytracer->setupVBOs();


	glutMainLoop();
}

//Initializes the lighting for the previewer only
//Copied from RPI Graphics hw3
void Canvas::initializeLighting(){
	GLfloat position[4] = {30,30,100,1};
	GLfloat diffuse[4] = {.75,.75,.75,1};
	GLfloat specular[4] = {0,0,0,1};
	GLfloat ambient[4] = {.2,.2,.2,1.0};

	GLfloat zero[4] = {0,0,0,0};
	glLightfv(GL_LIGHT1, GL_POSITION, position);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT1, GL_SPECULAR, specular);
	glLightfv(GL_LIGHT1, GL_AMBIENT, zero);
	glEnable(GL_LIGHT1);
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,GL_TRUE);
	glEnable(GL_COLOR_MATERIAL);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT,ambient);

	GLfloat spec_mat[4] = {1,1,1,1};
	float glexponent = 30;
	glMaterialfv(GL_FRONT,GL_SHININESS, &glexponent);
	glMaterialfv(GL_FRONT,GL_SPECULAR, spec_mat);

	glColorMaterial(GL_FRONT,GL_AMBIENT_AND_DIFFUSE);
	float back_color[] = {.2,.8,.8,1};
	glMaterialfv(GL_BACK,GL_AMBIENT_AND_DIFFUSE,back_color);
	glEnable(GL_LIGHT1);
}

//Draws the rasterized version of the scene
//Copied from RPI Graphics hw3
void Canvas::display(void){
	glDrawBuffer(GL_BACK);

	Vec bg=mesh->background_color;

	glClearColor(bg.r(),bg.g(),bg.b(),0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	mesh->camera->glInit(input->width,input->height);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	mesh->camera->glPlaceCamera();
	initializeLighting();

	glEnable(GL_CULL_FACE);

	HandleGLError();

	raytracer->drawVBOs();
	RayTree::drawVBOs();

	glDisable(GL_LIGHTING);

	glutSwapBuffers();
}

//Changes the size of the window
//Copied from RPI Graphics hw3
void Canvas::reshape(int w,int h){
	input->width=w;
	input->height=h;

	glViewport(0,0,(GLsizei)input->width,(GLsizei)input->height);

	mesh->camera->glInit(input->width,input->height);
}

//Saves the location of the mouse press
//Copied from RPI Graphics hw3
void Canvas::mouse(int button,int /*state*/,int x,int y){
	input->raytracing=false;

	mouseButton=button;
	mouseX = x;
	mouseY = y;

	shiftPressed = (glutGetModifiers() & GLUT_ACTIVE_SHIFT) != 0;
	controlPressed = (glutGetModifiers() & GLUT_ACTIVE_CTRL) != 0;
	altPressed = (glutGetModifiers() & GLUT_ACTIVE_ALT) != 0;
}

//Mouse button drags
//Copied from RPI Graphics hw3
void Canvas::motion(int x,int y){
	//Left button = rotation
	//Left button+ctrl = translation
	if(mouseButton==GLUT_LEFT_BUTTON){
		
		if(controlPressed){
			mesh->camera->truckCamera((mouseX-x)*.5,(y-mouseY)*.5);
		}else{
			mesh->camera->rotateCamera(.005*(mouseX-x),.005*(mouseY-y));
		}
		mouseX=x;
		mouseY=y;
	}
	//Right button + ctrl = zoom
	//Right button = dolly
	else if(mouseButton == GLUT_RIGHT_BUTTON){
		if(controlPressed){
			mesh->camera->zoomCamera(mouseY-y);
		} else {
			mesh->camera->dollyCamera(mouseY-y);
		}
		mouseX=x;
		mouseY=y;
	}
	glutPostRedisplay();
}

//Any button on the keyboard pressed
void Canvas::keyboard(unsigned char key,int x,int y){
	switch(key){
		case '1':{
			//Draws with highest resolution
			input->raytracing=true;
			raytracing_skip = 1;
			raytracing_x = raytracing_skip / 2;
			raytracing_y = raytracing_skip / 2;
			display();
			break;
		}
		case 'r': case 'R': {
			//Does next level of resolution
			input->raytracing=true;
			raytracing_skip = raytracing_skip /2;
			if(raytracing_skip % 2 ==0){
				raytracing_skip++;
			}
			raytracing_x = raytracing_skip / 2;
			raytracing_y = raytracing_skip / 2;
			display();
			break;
		}
		case ' ': {
			//Resets resolution
			input->raytracing=!input->raytracing;
			if(input->raytracing){
				raytracing_skip=input->pixel_size;
				raytracing_x = raytracing_skip/2;
				raytracing_y = raytracing_skip/2;
				display();
			}
			break;
		}
		case 't': case 'T': {
			//Traces ray for one pixel and displays raytree
			int i = x;
			int j = glutGet(GLUT_WINDOW_HEIGHT) - y;
			RayTree::Activate();
			int temp=raytracing_skip;
			raytracing_skip=1;
			TraceRay(i,j);
			RayTree::Deactivate();
			RayTree::setupVBOs();
			raytracer->setupVBOs();

			glutPostRedisplay();
			raytracing_skip = temp;
			break;
		}
		case 'q': case 'Q': {
			//Quits
			delete Canvas::mesh;
			delete Canvas::raytracer;
			exit(0);
			break;
		}
		case 'm': case 'M': {
			//Change intersection mode
			if(input->rp_mode + 1 == NUM_RANDOM_POINT_MODES){
				input->rp_mode = RANDOM_POINT_MODE(0);
			}else{
				input->rp_mode = RANDOM_POINT_MODE(input->rp_mode + 1);
			}
			if(input->rp_mode == RANDOM_POINT){
				std::cout<<"Intersection using RANDOM_POINT"<<std::endl;
			}
			if(input->rp_mode == INSIDE_RADIUS){
				std::cout<<"Intersection using INSIDE_RADIUS"<<std::endl;
			}
			if(input->rp_mode == BETWEEN_RADIUS){
				std::cout<<"Intersection using BETWEEN_RADIUS"<<std::endl;
			}
			break;
		}
		case 's': case 'S': {
			//Changes the use of the singularity fix for multiple scattering
			input->singularity_fix = !input->singularity_fix;
			if(input->singularity_fix){
				std::cout<<"Now using singularity_fix"<<std::endl;
			}else{
				std::cout<<"Now NOT using singularity_fix"<<std::endl;
			}
			break;
		}
	}
}

//Called when nothing is going on
//Copied from RPI Graphics hw3
void Canvas::idle(){
	//Draws 100 pixels at a time
	if(input->raytracing){
		glDisable(GL_LIGHTING);
		glDrawBuffer(GL_FRONT);
		glDisable(GL_DEPTH_TEST);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glPointSize(raytracing_skip);
		glBegin(GL_POINTS);
		for(int i=0; i < 100; ++i){
			if(!DrawNextPixel()){
				input->raytracing=false;
				break;
			}
		}
		glEnd();
		glFlush();
	}
}

//Draws one pixel
int Canvas::DrawNextPixel(){
	//Goes to the next line
	if(raytracing_x > input->width){
		raytracing_x=raytracing_skip/2;
		raytracing_y+=raytracing_skip;
	}
	//Once all lines are done
	if(raytracing_y > input->height){
		//Refines the resolution unless already at the finest resolution or set not to refine
		if(raytracing_skip == 1 || !input->automatic_refine) {
			return 0;
		}
		//Doubles the resolution
		raytracing_skip = raytracing_skip /2;
		if(raytracing_skip % 2 ==0){
			raytracing_skip++;
		}
		raytracing_x = raytracing_skip / 2;
		raytracing_y = raytracing_skip / 2;
		glEnd();
		glPointSize(raytracing_skip);
		glBegin(GL_POINTS);
	}
	//Traces a ray and converts the color and draws a small box
	Vec color = TraceRay(raytracing_x,raytracing_y);
	double r = linear_to_srgb(color.r());
	double g = linear_to_srgb(color.g());
	double b = linear_to_srgb(color.b());
	glColor3f(r,g,b);
	double x = 2*(raytracing_x/double(input->width)) - 1;
	double y = 2*(raytracing_y/double(input->height)) - 1;
	glVertex3f(x,y,-1);
	raytracing_x +=raytracing_skip;
	return 1;
}

//Traces one ray at pixel coordinates (i,j)
Vec Canvas::TraceRay(double i,double j){
	Vec color;
	int md=input->width;
	if(input->height > md){
		md=input->height;
	}
	if(input->num_antialias_samples > 1) {
		//Used jittered sampling to trace multiple samples
		int nd = (int)floor(sqrt(input->num_antialias_samples));
		double space = 1.0/(double)nd;
		for(int s = 0; s < input->num_antialias_samples; ++s){
			double xoff = double(int((s % (nd * nd)))/double(nd));
			double yoff = (s % (nd *nd)) - xoff * nd;
			double xd = space * xoff + space * GLOBAL_mtrand.rand();
			double yd = space * yoff + space * GLOBAL_mtrand.rand();
			double x = (i + xd - input->width / 2.0)/double(md) + .5;
			double y = (j + yd - input->height / 2.0)/double(md) + .5;
			//std::cout<<x<<" "<<y<<std::endl;
			Ray r = mesh->camera->generateRay(x,y);
			Hit hit;
			color += (raytracer->TraceRay(r,hit,0))/double(input->num_antialias_samples);

			RayTree::AddMainSegment(r,0,hit.getT());
		}
	} else {
		//If only one anti-alias samples the ray is traced through the center
		double x = (i - input->width / 2.0)/double(md) + .5;
		double y = (j - input->height / 2.0)/double(md) + .5;
		Ray r = mesh->camera->generateRay(x,y);
		Hit hit;
		color += (raytracer->TraceRay(r,hit,0));
		RayTree::AddMainSegment(r,0,hit.getT());
	}
	return color;
}

//Helps deal with errors from OpenGL function calls
int HandleGLError(const std::string &message) {
  GLenum error;
  int i = 0;
  while ((error = glGetError()) != GL_NO_ERROR) {
    if (message != "") {
      std::cout << "[" << message << "] ";
    }
    std::cout << "GL ERROR(" << i << ") " << gluErrorString(error) << std::endl;
    i++;
  }
  if (i == 0) return 1;
  return 0;
}
