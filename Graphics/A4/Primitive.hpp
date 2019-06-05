// Fall 2018

#pragma once

#include <glm/glm.hpp>
#include "Ray.hpp"
#include "SceneNode.hpp"
#include <vector>

struct Face
{
    vec3 v1;
    vec3 v2;
    vec3 v3;
    
    Face( vec3 pv1, vec3 pv2, vec3 pv3 )
    : v1( pv1 )
    , v2( pv2 )
    , v3( pv3 )
    {}
};

class Primitive {
public:
    virtual ~Primitive();
    virtual Intersection hit(Ray r) {return Intersection(false);}
};



class Sphere : public Primitive {
public:
    virtual ~Sphere();
    Intersection hit(Ray r) {return Intersection(false);}
};



class Cube : public Primitive {
public:
    virtual ~Cube();
    Intersection hit(Ray r) {return Intersection(false);}
};



class NonhierSphere : public Primitive {
public:
    NonhierSphere(const glm::vec3& pos, double radius)
    : m_pos(pos), m_radius(radius) {}
    
    virtual ~NonhierSphere();
    Intersection hit(Ray r);

private:
    glm::vec3 m_pos;
    double m_radius;
    double EPS = 0.15;
};



class NonhierBox : public Primitive {
public:
    NonhierBox(const glm::vec3& pos, double size);

    virtual ~NonhierBox();
    Intersection hit(Ray r);
private:
    double ESP = 0.01;
    glm::vec3 m_pos;
    double m_size;
    std::vector<Face> m_faces;
};
