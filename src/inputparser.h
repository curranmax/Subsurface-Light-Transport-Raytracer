#ifndef __INPUT_PARSER_H__
#define __INPUT_PARSER_H__

#include <cstring>
#include <cstdlib>
#include <iostream>
#include "vector.h"

//Adapted from RPI Graphics hw3

//Different modes to find random poitns on an object
#define NUM_RANDOM_POINT_MODES 3
enum RANDOM_POINT_MODE { RANDOM_POINT, INSIDE_RADIUS, BETWEEN_RADIUS};

//Hold different paramters for ray tracing and other things
class InputParser{
public:
	InputParser(){DefaultValues();}

	InputParser(int argc,char *argv[]){
		DefaultValues();

		//parses input from the command line and sets appropriate variables
		for(int i=1;i<argc;++i){
			if(!strcmp(argv[i],"-input")){
				increment(i,argc);
				input_file=argv[i];
			} else if(!strcmp(argv[i],"-num_shadow_samples")){
				increment(i,argc);
				num_shadow_samples = atoi(argv[i]);
			} else if(!strcmp(argv[i],"-num_antialias_samples")){
				increment(i,argc);
				num_antialias_samples = atoi(argv[i]);
			} else if(!strcmp(argv[i],"-num_bounces")){
				increment(i,argc);
				num_bounces = atoi(argv[i]);
			} else if(!strcmp(argv[i],"-num_single_scatter_samples")){
				increment(i,argc);
				num_single_scatter_samples = atoi(argv[i]);
			} else if(!strcmp(argv[i],"-num_multiple_scatter_samples")){
				increment(i,argc);
				num_multiple_scatter_samples = atoi(argv[i]);
			} else if(!strcmp(argv[i],"-scale")){
				increment(i,argc);
				scale = atoi(argv[i]);
			}
			else{
				UsageError();
			}
		}
		if(input_file==NULL){
			UsageError();
		}
	}

	void increment(int &i,int argc){
		++i;
		if(i>=argc){
			UsageError();
		}
	}

	//Sets default values for paramters
	void DefaultValues(){
		input_file=NULL;
		height=250;
		width=250;

		sphere_horizontal=8;
		sphere_vertical=6;
		cylinder_points=32;
		cone_points = 32;

		raytracing=false;
		intersect_backfacing = true;
		ambient_light=Vec(.2,.2,.2);
		pixel_size = 32;
		automatic_refine = false;

		num_antialias_samples = 1;
		num_shadow_samples = 1;
		num_bounces = 1;
		num_single_scatter_samples = 0;
		num_multiple_scatter_samples = 0;
		scale = 1.0;
		rp_mode = RANDOM_POINT;
		singularity_fix = false;
	}

	//When user gives wrong input
	void UsageError(){
		std::cerr<<"Usage: render -input <input_file_path> [options]"<<std::endl;
		std::cerr<<"\toptions:"<<std::endl;
		std::cerr<<"\t\t-num_shadow_samples <num_samples>"<<std::endl;
		std::cerr<<"\t\t-num_antialias_samples <num_samples>"<<std::endl;
		std::cerr<<"\t\t-num_single_scatter_samples <num_samples>"<<std::endl;
		std::cerr<<"\t\t-num_multiple_scatter_samples <num_samples>"<<std::endl;
		std::cerr<<"\t\t-num_bounces <num_bounces>"<<std::endl;
		std::cerr<<"\t\t-scale <scale>"<<std::endl;
		exit(1);
	}

	//All data public

	//BASIC
	char *input_file;
	int height;
	int width;

	//Rasterization
	int sphere_horizontal;
	int sphere_vertical;
	int cylinder_points;
	int cone_points;

	//Raytracing
	bool raytracing;
	bool intersect_backfacing;
	Vec ambient_light;
	double pixel_size;
	bool automatic_refine;

	//Raytracing variables
	int num_antialias_samples;
	int num_shadow_samples;
	int num_bounces;
	int num_single_scatter_samples;
	int num_multiple_scatter_samples;
	double scale;

	//Different methods of multiple scattering
	RANDOM_POINT_MODE rp_mode;
	bool singularity_fix;
};


#endif
