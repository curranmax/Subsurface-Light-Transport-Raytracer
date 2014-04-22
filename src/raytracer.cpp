#include "raytracer.h"
#include "mesh.h"
#include "vector.h"
#include "face.h"
#include "edge.h"
#include "material.h"
#include "utils.h"
#include "primitive.h"
#include "hit.h"
#include "raytree.h"

#include <cassert>
//VBO functions and outline of raytracer Adapted from RPI Graphics hw3

//Finds first object that ray intersects
bool RayTracer::CastRay(const Ray &ray,Hit &h,bool use_rasterized_patches) const{
	bool answer = false;
	//Checks all quads for intersections
	for (int i=0;i<mesh->numOriginalFaces();++i){
		Face *f=mesh->getOriginalFace(i);
		answer = f->intersect(ray,h,input->intersect_backfacing) || answer;
	}
	//Checks all primitives for intersections
	for(int i = 0;i<mesh->numPrimitives(); ++i){
		answer = mesh->getPrimitive(i)->intersect(ray,h) || answer;
	}
	return answer;
}

//Recursively Traces a ray in the scene
Vec RayTracer::TraceRay(Ray &ray,Hit &hit,int bounce=0) const{

	hit = Hit();
	//Finds the intersection of the ray in the scene
	bool intersect = CastRay(ray,hit,false);

	//Didn't intersect so returns background color
	if(!intersect){
		return Vec(srgb_to_linear(mesh->background_color.r()),
					srgb_to_linear(mesh->background_color.g()),
					srgb_to_linear(mesh->background_color.b()));
	}

	Material *m = hit.getMaterial();

	//If its a light, just makes it white
	if(m->getEmittedColor().Length() > 0.001) {
		return Vec(1,1,1);
	}

	Vec normal = hit.getNormal();
	Vec point = ray.pointAtT(hit.getT());
	Vec diffuse_color = m->getDiffuseColor();
	Vec ans = diffuse_color * input->ambient_light;

	//shadow
	for(unsigned int i=0;i<mesh->getLights().size();++i){
		Face *l=mesh->getLights()[i];

		//Finds color of light on material, which is inversly proportional to the squared distance between it and the light
		Vec lc=l->getMaterial()->getEmittedColor() * l->getArea();
		Vec lightCenter = l->computeCenter();
		Vec dirToLight = lightCenter - point;
		double distToLight = dirToLight.Length();
		dirToLight.Normalize();
		Vec obslcol = lc / (M_PI *distToLight * distToLight);

		Vec total;
		Vec c0=(*l)[0]->get(),
			c1=(*l)[1]->get(),
			c2=(*l)[2]->get(),
			c3=(*l)[3]->get();
		if(input->num_shadow_samples > 1){
			//Jittered sampling over the face of the light
			int nd=(int)floor(sqrt(input->num_shadow_samples));
			double space=1.0/double(nd);
			for(int lsamp=0;lsamp<input->num_shadow_samples;++lsamp){
				//Compute point on the light to cast to
				double xoff = double(int((lsamp % (nd * nd)) / double(nd)));
				double yoff = (lsamp % (nd * nd)) - xoff * nd;
				double xd = space * xoff + space * GLOBAL_mtrand.rand();
				double yd = space * yoff + space * GLOBAL_mtrand.rand();
				Vec c01 = c0 + (c1 - c0) * yd;
				Vec c32 = c3 + (c2 - c3) * yd;
				Vec lightpoint = c01 + (c32 - c01)*xd;
				Vec dirToLP = lightpoint - point;
				dirToLP.Normalize();

				Ray lightray(point,dirToLP);
				Hit lighth;

				//Casts the ray towards the light
				bool lightintersect=CastRay(lightray,lighth,false);
				RayTree::AddShadowSegment(lightray,0,lighth.getT());

				//Checks if the ray intersected the light
				if(lightintersect && lighth.getMaterial()->getEmittedColor().Length() > .001){
					total+=m->Shade(ray,hit,dirToLP,obslcol,input)/double(input->num_shadow_samples);
				}
			}
			ans+=total;
		} else {
			//If one sample then cast to the center of the light
			Ray lightray(point,dirToLight);
			Hit lighth;

			bool lightintersect=CastRay(lightray,lighth,false);
			RayTree::AddShadowSegment(lightray,0,lighth.getT());

			if(lightintersect && lighth.getMaterial()->getEmittedColor().Length() > .001){
				ans+=m->Shade(ray,hit,dirToLight,obslcol,input)/double(input->num_shadow_samples);
			}
		}
	}

	//reflection
	//Recusively traces ray
	Vec reflectiveColor=m->getReflectiveColor();
	if(bounce < input->num_bounces && reflectiveColor.Length() > 0.001){
		Vec reflectiveDir = ray.getDirection() - 2 * ((ray.getDirection().Dot(normal))*normal);
		Ray reflectray(point,reflectiveDir);
		Hit reflecth;

		Vec reflectans = TraceRay(reflectray,reflecth,bounce + 1);

		RayTree::AddReflectedSegment(reflectray,0,reflecth.getT());

		ans += reflectans * reflectiveColor;
	}

	double index_of_refraction = hit.getMaterial()->getIndexOfRefraction();

	//Single Scattering term
	if(m->isTranslucent() && hit.hasObject() && input->num_single_scatter_samples > 0){
		//Formula of single scattering term:
		//sigma_s(x_0) * F(index,w_o) * F(index,w_i) * 1/(4pi) / sigma_tc * exp(-s_i' * sigma_t(x_i)) * exp(-s_o' * sigma_t(x_0)) * L_i(x_i,w_i)
		Vec single_scatter_contribution;
		const Primitive *obj = hit.getObject();

		//Parameters of the material that is being scattering in
		Vec sigma_s_x0 = hit.getMaterial()->getSigmaS();
		Vec sigma_t_x0 = hit.getMaterial()->getSigmaT();
		Vec normal_0 = hit.getNormal();
		Vec dir_in_0 = ray.getDirection();
		Vec x_0 = point;
		double cos_0 = normal_0.Dot(-1 * dir_in_0);
		double F_0 = FresnelTransmission(1/index_of_refraction,acos(cos_0));

		Vec dir_out_0 = dir_in_0;

		//Finds the further distance tha tone can go into the object
		Ray objDepthTestRay(x_0,dir_out_0);
		Hit objDepthTestHit;
		obj->intersect(objDepthTestRay,objDepthTestHit);
		double max_t = objDepthTestHit.getT();
		Vec max_x_c = x_0 + max_t * dir_out_0;		

		//Each sample
		for(int i = 0; i < input->num_single_scatter_samples ; ++i){
			double rv;
			double t_0 = 0;
			//ensures point is not too close to surface and will still be in the object
			while(t_0 < .0001 && max_t > .0001 ){
				rv = GLOBAL_mtrand.rand() * (1 - exp(-max_t * sigma_t_x0.Average())) + exp(-max_t * sigma_t_x0.Average());
				t_0 =-log(rv)/(sigma_t_x0.Average());
			}
			double s_0;
			Vec x_c;
			if(t_0 < max_t ){
				s_0 = t_0 * input->scale;
				x_c = dir_out_0 * t_0 +x_0;
				RayTree::AddSingleScatterSegment(objDepthTestRay,0,t_0);
			} else {
				s_0 = max_t * input->scale;
				x_c = max_x_c;
				RayTree::AddSingleScatterSegment(objDepthTestRay,0,max_t);
			}
			//Each light
			for(unsigned int j=0;j<mesh->getLights().size();++j){
				Face *l=mesh->getLights()[j];
				Vec lc=l->getMaterial()->getEmittedColor() * l->getArea();
				Vec lightpoint = l->RandomPoint();
				Vec dir_in_i = lightpoint - x_c;
				dir_in_i.Normalize();

				//Casts from point inside object towards light to find intersection with object
				Ray find_x_i_ray(x_c,dir_in_i);
				Hit find_x_i_hit;
				Vec x_i;
				double s_i;
				Vec sigma_t_xi;
				Vec normal_i ;
				//If no intersection, assume that t_0 >= max_t, this is true for all but extreme edge cases
				if(!obj->intersect(find_x_i_ray,find_x_i_hit)){
					x_i = x_c;
					s_i = 0;
					sigma_t_xi = objDepthTestHit.getMaterial()->getSigmaT();
					normal_i = objDepthTestHit.getNormal();
				} else {
					x_i = x_c + dir_in_i * find_x_i_hit.getT();
					s_i = find_x_i_hit.getT() * input->scale;
					sigma_t_xi = find_x_i_hit.getMaterial()->getSigmaT();
					normal_i = find_x_i_hit.getNormal();
					RayTree::AddSingleScatterSegment(find_x_i_ray,0,find_x_i_hit.getT());
				}
				Vec dir_out_i = dir_in_i;

				//Casts ray from second point of intersection to light
				Ray light_Ray(x_i,dir_out_i);
				Hit light_Hit;
				bool lightintersect = CastRay(light_Ray,light_Hit,false);
				RayTree::AddSingleScatterSegment(light_Ray,0,light_Hit.getT());
				
				//Adds contribution of single scattering if ray intersects light
				if(lightintersect && light_Hit.getMaterial()->getEmittedColor().Length()>.001){
					//Calculate s_i_prime
					Vec obslcol = lc / (M_PI * light_Hit.getT() * light_Hit.getT());
					double s_i_prime;
					Vec w_i = dir_out_i;
					
					double w_dot_normal_i = fabs(w_i.Dot(normal_i));
					double denomenator = sqrt(1 - (1/index_of_refraction*index_of_refraction) * (1 - w_dot_normal_i * w_dot_normal_i));
					if(denomenator == 0){
						continue;
					}
					s_i_prime = s_i * w_dot_normal_i / denomenator;	
					double F_i = FresnelTransmission(index_of_refraction,acos(w_dot_normal_i));

					Vec sigma_tc = sigma_t_x0 + fabs(normal_i.Dot(dir_in_0))/w_dot_normal_i * sigma_t_xi;
					sigma_tc.Invert();

					//adds contribution of this sample
					single_scatter_contribution += sigma_tc * F_i * (-s_i_prime * sigma_t_xi).Exp() * (-s_0 * sigma_t_x0).Exp() * obslcol;
				}	
			}
		}
		//Averages total and adds to final answer
		ans += single_scatter_contribution * sigma_s_x0 * F_0 / input->num_single_scatter_samples;
	}

	//Multiple Scattering
	if(m->isTranslucent() && hit.hasObject() && input->num_multiple_scatter_samples > 0){
		//Gets som constants
		const Primitive *obj = hit.getObject();
		Vec x_0 = point;
		Vec sigma_t_x0 = hit.getMaterial()->getSigmaT();
		Vec sigma_tr_x0 = hit.getMaterial()->getSigmaTR();
		double avg_sigma_tr_x0 = sigma_tr_x0.Average();
		Vec inv_stx0 = sigma_t_x0;
		inv_stx0.Invert();

		//Dipole Source placement
		//Places a real source below the surface and a virtual source below the surface to satisfy boundary positions
		Vec real_source = x_0 - normal * inv_stx0;
		double z_r = (x_0 - real_source).Length();
		//Approximation
		double F_dr = 1.440 / index_of_refraction / index_of_refraction + .710 / index_of_refraction + .668 + .0636 * index_of_refraction;
		Vec virtual_source = x_0 + normal * (inv_stx0 + inv_stx0 * 2 / 3 *(1 + F_dr) / (1 - F_dr));
		double z_v = (x_0 - virtual_source).Length();

		Vec multiple_scatter_contribution;
		double weighted_contribution = 0;

		//For each sample find random point on object and cast ray to light
		for(int i=0; i < input->num_multiple_scatter_samples; ++i){
			Vec x_i;
			double factor = 1.0;

			//Gets a random point using selected mode
			if(input->rp_mode == RANDOM_POINT){
				x_i = obj->getRandomPoint();
				factor = 1-exp(-(x_i-x_0).Length() / avg_sigma_tr_x0);
			}else if(input->rp_mode == INSIDE_RADIUS){
				double rv = GLOBAL_mtrand.rand();
				double radius = -avg_sigma_tr_x0 * log(1-rv) * input->scale;
				x_i = obj->getRandomPoint(x_0,radius);
			}else if(input->rp_mode == BETWEEN_RADIUS){
				double rv1 = GLOBAL_mtrand.rand();
				double rv2 = GLOBAL_mtrand.rand();

				double radius1 = -avg_sigma_tr_x0 * log(1-rv1) * input->scale;
				double radius2 = -avg_sigma_tr_x0 * log(1-rv2) * input->scale;

				x_i = obj->getRandomPoint(x_0,radius1,radius2);

			}

			//Finds distance between x_i and the dipoles
			double d_r = (x_i - real_source).Length();
			double d_v = (x_i - virtual_source).Length();

			//Clamps d_r and d_v to z_r and z_v respectively
			if(input->singularity_fix){
				if(d_r < z_r){
					d_r = z_r;
				}
				if(d_v < z_v){
					d_v = z_v;
				}
			}

			//Casts a ray to each light source and adds contribution
			for(unsigned int j=0;j<mesh->getLights().size();++j){

				Face *l=mesh->getLights()[j];
				Vec lightpoint = l->RandomPoint();
				Vec w_i = lightpoint - x_i;
				double distToLight = w_i.Length();
				w_i.Normalize();

				Ray light_Ray(x_i,w_i);
				Hit light_Hit;
				bool lightintersect = CastRay(light_Ray,light_Hit,false);
				RayTree::AddMultipleScatterSegment(light_Ray,0,light_Hit.getT());

				if(lightintersect && light_Hit.getMaterial()->getEmittedColor().Length()>.001){
					Vec obslcol = l->getMaterial()->getEmittedColor() * l->getArea() / (M_PI * distToLight * distToLight);

					multiple_scatter_contribution += factor * obslcol / 4 / M_PI *((sigma_tr_x0 * d_r + Vec(1,1,1)) * (-1 * sigma_tr_x0 * d_r).Exp() * inv_stx0 / pow(d_r,3) + z_v * (sigma_tr_x0 * d_v + Vec(1,1,1)) * (-1 * sigma_tr_x0 * d_v).Exp() * inv_stx0 / pow(d_v,3));
				}
				weighted_contribution += factor;

			}
		}
		ans += multiple_scatter_contribution / weighted_contribution;
	}

	return ans;
}

//Prepares scene for previewer
void RayTracer::initializeVBOs(){
	glGenBuffers(1, &mesh_quad_verts_VBO);
	glGenBuffers(1, &mesh_quad_indices_VBO);
	glGenBuffers(1, &mesh_textured_quad_indices_VBO);
	glGenBuffers(1, &mesh_interior_edge_indices_VBO);
	glGenBuffers(1, &mesh_border_edge_indices_VBO);
}

void RayTracer::setupVBOs(){
	mesh_quad_verts.clear();
	mesh_quad_indices.clear();
	mesh_textured_quad_indices.clear();
	mesh_interior_edge_indices.clear();
	mesh_border_edge_indices.clear();
	int nf=mesh->numFaces();
	for(int i=0;i<nf;++i){
		Face *f=mesh->getFace(i);
		Edge *e=f->getEdge();
		for(int j=0;j<4;++j){
			Vec p=((*f)[j])->get();
			Vec norm=f->computeNormal();
			Vec color=f->getMaterial()->getDiffuseColor();
			color=Vec(linear_to_srgb(color.r()),linear_to_srgb(color.g()),linear_to_srgb(color.b()));
			mesh_quad_verts.push_back(VBOPosNormalColorTexture(p,norm,color,0,0)); //0s are placeholders for the texture
			if(e->getOpposite()==NULL){
				mesh_border_edge_indices.push_back(VBOIndexedEdge(i*4+j,i*4+(j+1)%4));
			}
			e=e->getNext();
		}
		mesh_quad_indices.push_back(VBOIndexedQuad(i*4,i*4+1,i*4+2,i*4+3));
	}
	assert((int)mesh_quad_verts.size() == nf * 4);
	assert((int)mesh_quad_indices.size() + (int)mesh_textured_quad_indices.size() == nf);

	cleanupVBOs();

	glBindBuffer(GL_ARRAY_BUFFER,mesh_quad_verts_VBO);
	glBufferData(GL_ARRAY_BUFFER,sizeof(VBOPosNormalColorTexture)*mesh_quad_verts.size(),&mesh_quad_verts[0],GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,mesh_quad_indices_VBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(VBOIndexedQuad)*mesh_quad_indices.size(),&mesh_quad_indices[0],GL_STATIC_DRAW);
	if(mesh_textured_quad_indices.size()>0){
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,mesh_textured_quad_indices_VBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(VBOIndexedQuad)*mesh_textured_quad_indices.size(),&mesh_textured_quad_indices[0],GL_STATIC_DRAW);
	}
	if(mesh_interior_edge_indices.size()>0){
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,mesh_interior_edge_indices_VBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(VBOIndexedEdge)*mesh_interior_edge_indices.size(),&mesh_interior_edge_indices[0],GL_STATIC_DRAW);
	}
	if(mesh_border_edge_indices.size()>0){
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,mesh_border_edge_indices_VBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(VBOIndexedEdge)*mesh_border_edge_indices.size(),&mesh_border_edge_indices[0],GL_STATIC_DRAW);
	}
}

void RayTracer::drawVBOs(){
	glEnable(GL_LIGHTING);
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(1.1,4.0);
	
	glBindBuffer(GL_ARRAY_BUFFER,mesh_quad_verts_VBO);

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3,GL_FLOAT,sizeof(VBOPosNormalColorTexture),BUFFER_OFFSET(0));

	glEnableClientState(GL_NORMAL_ARRAY);
	glNormalPointer(GL_FLOAT,sizeof(VBOPosNormalColorTexture),BUFFER_OFFSET(12));

	glEnableClientState(GL_COLOR_ARRAY);
	glColorPointer(3,GL_FLOAT,sizeof(VBOPosNormalColorTexture),BUFFER_OFFSET(24));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,mesh_quad_indices_VBO);
	glDrawElements(GL_QUADS,mesh_quad_indices.size()*4,GL_UNSIGNED_INT,BUFFER_OFFSET(0));
	
	glEnable(GL_TEXTURE_2D);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2,GL_FLOAT,sizeof(VBOPosNormalColorTexture),BUFFER_OFFSET(36));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,mesh_textured_quad_indices_VBO);
	glDrawElements(GL_QUADS,mesh_textured_quad_indices.size()*4,GL_UNSIGNED_INT,BUFFER_OFFSET(0));
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisable(GL_TEXTURE_2D);

	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);

	glDisable(GL_POLYGON_OFFSET_FILL);

	//Draw wireframe if needed here

	HandleGLError();
}

void RayTracer::cleanupVBOs(){
	glDeleteBuffers(1, &mesh_quad_verts_VBO);
	glDeleteBuffers(1, &mesh_quad_indices_VBO);
	glDeleteBuffers(1, &mesh_textured_quad_indices_VBO);
	glDeleteBuffers(1, &mesh_interior_edge_indices_VBO);
	glDeleteBuffers(1, &mesh_border_edge_indices_VBO);
}
