// Winter 2019

#pragma once

#include <glm/glm.hpp>
#include "SceneNode.hpp"
#include "Light.hpp"
#include "Image.hpp"

class Ray;

class A4 {
    // What to render
    SceneNode * root;
    
    // Image to write to, set to a given width and height
    Image & image;
    
    // Viewing parameters
    const glm::vec3 & eye;
    const glm::vec3 & view;
    const glm::vec3 & up;
    double fovy;
    
    // Lighting parameters
    const glm::vec3 & ambient;
    const std::list<Light *> & lights;
    
    // Add more
    glm::mat4 pixelToWorldTrans;
    
public:
    A4(SceneNode * r, Image & i, const glm::vec3 & e,
       const glm::vec3 & v, const glm::vec3 & u,
       double f, const glm::vec3 & ambient,
       const std::list<Light *> & lights);
    vec2 curPixel;
    
    void A4_Render();
    //glm::vec3 pixelToWorld();
    Ray castRay(int x, int y);
    void traceRay(Ray &r, SceneNode * node, Intersection * ret);
    void setPixelToWorldTrans();
    dvec3 getColorFromLight(Intersection & i);
    dvec3 getDiffuseColor(Light * source, Intersection & i);
    dvec3 getSpecularColor(Light * source, Intersection & i);
    bool isLightBlocked(Intersection &i, Light * source);
};




