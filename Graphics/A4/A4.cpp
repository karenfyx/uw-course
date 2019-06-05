// Winter 2019

#include <glm/ext.hpp>

#include "A4.hpp"
#include <glm/glm.hpp>
#include <glm/gtx/io.hpp>
#include <glm/gtx/transform.hpp>
#include "cs488-framework/MathUtils.hpp"
#include "Ray.hpp"
#include "PhongMaterial.hpp"

using namespace glm;
using namespace std;

A4::A4(SceneNode * r, Image & i, const glm::vec3 & e,
       const glm::vec3 & v, const glm::vec3 & u,
       double f, const glm::vec3 & a, const std::list<Light *> & l
       ):  root(r), image(i), eye(e), view(v),
            up(u), fovy(f),ambient(a), lights(l) {
    
        A4_Render();
                
};

void A4::A4_Render() {

  // Fill in raytracing code here...  

#ifdef DEBUG
  std::cout << "Calling A4_Render(\n" <<
		  "\t" << *root <<
          "\t" << "Image(width:" << image.width() << ", height:" << image.height() << ")\n"
          "\t" << "eye:  " << glm::to_string(eye) << std::endl <<
		  "\t" << "view: " << glm::to_string(view) << std::endl <<
		  "\t" << "up:   " << glm::to_string(up) << std::endl <<
		  "\t" << "fovy: " << fovy << std::endl <<
          "\t" << "ambient: " << glm::to_string(ambient) << std::endl <<
		  "\t" << "lights{" << std::endl;

	for(const Light * light : lights) {
		std::cout << "\t\t" <<  *light << std::endl;
	}
	std::cout << "\t}" << std::endl;
	std:: cout <<")" << std::endl;
#endif

	size_t h = image.height();
	size_t w = image.width();

    setPixelToWorldTrans();
    
    double t = 0; //blend
    vec3 blue = vec3(0,0,1);
    vec3 white = vec3(1,1,1);
    
	for (uint y = 0; y < h; ++y) {
		for (uint x = 0; x < w; ++x) {
            {
#ifdef DEBUG
                cout << "\r" << "Process pixel (" << x << " ," << y << ")";
                cout.flush();
#endif
            }
            curPixel = vec2(x,y);
            Ray ray = castRay(x,y);
            ray.pixel = vec2(x,y);
            Intersection i = Intersection(false);
            traceRay(ray, root, &i);
            dvec3 color = dvec3(1,1,1);
            
            if(i.intersect) { color = getColorFromLight(i); }
            else { color = t*blue + (1-t)*white; }
            
            int r = 5;
            int minx = 88;
            int miny = 140;
            if(x > minx && x < minx+r && y > miny and y < miny+r ) {color = vec3(0,0,0);}
            
			// Red:
			image(x, y, 0) = (double)color.x;
			// Green: 
			image(x, y, 1) = (double)color.y;
			// Blue: 
			image(x, y, 2) = (double)color.z;
            
            
		}
        t += (double)1/(double)h;
	}
}

Ray A4::castRay(int x, int y) {
    dvec4 pixel = dvec4(x, y, 0, 1);
    pixel = pixelToWorldTrans * pixel;
    //assert(int(pixel.w) == 1);
    dvec3 pixel3d = dvec3(pixel.x, pixel.y, pixel.z);
    return Ray(eye, pixel3d);
}

void A4::setPixelToWorldTrans() {
    double near = 1.0;
    double h = 2 * near * tan(degreesToRadians(fovy)/2.0);
    vec3 w = (normalize(view));  //z
    vec3 u = (normalize(cross(up, w))); //x
    vec3 v = cross(w, u); //y
    
    mat4 step_1 = translate(dvec3(-(double)image.width()/2.0,
                                -(double)image.height()/2.0,
                                 near));
    mat4 step_2 = scale(dvec3(-h/(double)image.height(),
                            -h/(double)image.height(),
                             1.0));
    
    mat4 step_3 = mat4( dvec4(u.x, u.y, u.z, 0),
                       dvec4(v.x, v.y, v.z, 0),
                       dvec4(w.x, w.y, w.z, 0),
                       dvec4(0,   0,   0,  1));
    
    mat4 step_4 = mat4( dvec4(1,  0,  0,  0),
                       dvec4(0,  1,  0,  0),
                       dvec4(0,  0,  1,  0),
                       dvec4(eye.x, eye.y, eye.z, 1));
    
    pixelToWorldTrans = step_4 * step_3 * step_2 * step_1; //* step_0;
    
#ifdef DEBUG
        cout << "step 1 " << step_1 << endl;
        cout << "step 2 " << step_2 << endl;
        cout << "step 3 " << step_3 << endl;
        cout << "step 4 " << step_4 << endl;
        cout << "step 12" << step_2*step_1 << endl;
        cout << "step 123" << step_3*step_2*step_1 << endl;
        cout << "ptw " << pixelToWorldTrans << endl;
#endif
    
}

void A4::traceRay(Ray &r, SceneNode * node, Intersection * ret) {
    Intersection i = node->findIntersection(r);
    if(i.intersect && (!ret->intersect || i.position.z > ret->position.z )) {*ret = i;}
    //cout << "traceRay at node " << node->m_name << endl;
    //Ray savedRay = r;
    for(auto child : node->children) {
        traceRay(r, child, ret);
    }
}

dvec3 A4::getColorFromLight(Intersection &i) {
    PhongMaterial * material = static_cast<PhongMaterial *>(i.material);
    dvec3 color = ambient * material->m_kd;
    for(Light * source : lights) {
        if(!isLightBlocked(i, source)) {
            color += getDiffuseColor(source, i);
            color += getSpecularColor(source, i);
        }
    }
    return color;
}

dvec3 A4::getDiffuseColor(Light * source, Intersection & i) {
    dvec3 lightDirection = normalize((dvec3)source->position - i.position);
    double direct = dot(lightDirection, i.normal);
    double d = glm::distance((fvec3)source->position, (fvec3)i.position);
    double attenuation = source->falloff[0] +
                        source->falloff[1] * d +
                        source->falloff[2] * d * d;
    if( direct < 0) { direct = 0; }
    PhongMaterial * material = static_cast<PhongMaterial *>(i.material);
    
    if(curPixel.x == 88 && curPixel.y == 140 && source->id == 2) {
#ifdef DEBUG
        cout << "diffuse " << material->m_kd * direct/attenuation * source->colour << "\n";
#endif
    }
    
    return material->m_kd * direct/attenuation * source->colour;
    
}

dvec3 A4::getSpecularColor(Light * source, Intersection & i) {
    dvec3 lightDirection = normalize((dvec3)source->position - i.position);
    dvec3 viewDirection = normalize((dvec3)eye - i.position);
    dvec3 l = lightDirection;
    dvec3 reflectDirection = -l + 2 * dot(l, i.normal) * i.normal;
    double direct = dot(viewDirection, reflectDirection);
    if(direct < 0) { direct = 0;}
    PhongMaterial * material = static_cast<PhongMaterial *>(i.material);
    
    /*{
        glm::dvec3 Ri = normalize(dvec3(i.inDirection));
        glm::dvec3 N = i.normal;
        glm::dvec3 Rr = Ri - 2.0 * N * (dot(Ri, N));
        direct = std::max(glm::dot(Rr, glm::normalize(lightDirection)), 0.0);
    }*/
#ifdef DEBUG
    if(curPixel.x == 190 && curPixel.y == 110 && source->id == 1) {        
        cout << "white lightDircetion " << lightDirection << endl;
        cout << "view dir " << viewDirection << endl;
        cout << "reflect by lib " << reflect(-lightDirection, i.normal) << endl;
        cout << "reflect by me " << reflectDirection << endl;
        cout << "direct " << direct << endl;
        cout << "final results " << material->m_ks * pow(direct, material->m_shininess) * source->colour << endl;
    }
#endif
    
    return material->m_ks * pow(direct, material->m_shininess) * source->colour;
}


bool A4::isLightBlocked(Intersection &i, Light * source) {
    Ray intersectToLight = Ray(i.position, source->position);
    Intersection result = Intersection(false);
    traceRay(intersectToLight, root, &result);
    return result.intersect;
}
