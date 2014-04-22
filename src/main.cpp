

#include <iostream>

#include "canvas.h"

#include "MersenneTwister.h"
#include "inputparser.h"
#include "mesh.h"
#include "raytracer.h"

#include "matrix.h"
#include "vector.h"

//Copied from RPI Graphics hw3

MTRand GLOBAL_mtrand;

int main(int argc,char *argv[]){
	//Initializes random number generator
	GLOBAL_mtrand = MTRand(37);

	//Reads in command line input
	InputParser *input=new InputParser(argc,argv);
	glutInit(&argc, argv);
	
	//Loads mesh and intializes raytracer
	Mesh *mesh=new Mesh();
	mesh->LoadInput(input->input_file,input);
	RayTracer *raytracer= new RayTracer(mesh,input);

	//Starts previwer and opens window
	Canvas::initialize(input,mesh,raytracer);

	//Never reachs here
	return 0;
}