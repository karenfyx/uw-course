// Winter 2019

#include "Primitive.hpp"
#include "polyroots.hpp"
#include <glm/gtx/io.hpp>
using namespace std;

Primitive::~Primitive()
{
}

Sphere::~Sphere()
{
}

Cube::~Cube()
{
}

NonhierSphere::~NonhierSphere()
{
}

NonhierBox::~NonhierBox()
{
}

Intersection NonhierSphere::hit(Ray r) {
    Intersection hitPoint = Intersection(false);
    double a = dot(r.d, r.d);
    double b = 2 * dot(r.d, r.s - (dvec3)m_pos);
    double c = dot(r.s - (dvec3)m_pos, r.s - (dvec3)m_pos) - m_radius * m_radius;
    
    double roots[2];
    quadraticRoots(a, b, c, roots);
    
    double det = b*b - 4*a*c;
    double t = 0;
    if( det == 0 && roots[0] > 0 ) {
        t = roots[0];
    }
    if( det > 0 ) {
        if(roots[0] > 0 && roots[1] > 0) {
            t = glm::min(roots[0], roots[1]);
        } else if(roots[0] > 0) {
            t = roots[0];
        } else if(roots[1] > 0) {
            t = roots[1];
        }
    }
    if(t < EPS) { return hitPoint; }
    hitPoint = Intersection(true, r.getPoint(t), r.d );
    //Calculate Normal
    if(hitPoint.intersect) {
        hitPoint.normal = normalize(hitPoint.position - (dvec3)m_pos);
    }
    
    return hitPoint;
}


Intersection rayFaceIntersection(Ray r, dvec3 vertex[]) {
    Intersection hitPoint = Intersection(false);
    dvec3 AminusB = vertex[0] - vertex[1];
    dvec3 AminusC = vertex[0] - vertex[2];
    dvec3 AminusS = vertex[0] - r.s;
    mat3 M = {r.d, AminusB, AminusC};
    if(abs(determinant(M)) <= 0.01) {
#ifdef DEBUG
        if(r.pixel.x == 70 && r.pixel.y == 140) {
            cout << "M" << M << endl;
            cout << "determinant(M) " << determinant(M) << endl;
        }
#endif
                return hitPoint;
    }
    vec3 res = inverse(M) * AminusS;

    if(res.y > 0 && res.z > 0 && res.y < 1 && res.z < 1) {
        
        hitPoint = Intersection(true, r.getPoint(res.x), r.d );
        hitPoint.t = res.x;
        hitPoint.normal = normalize(cross(-AminusB,-AminusC));
    }
    return hitPoint;
}

Intersection NonhierBox::hit(Ray r) {
    Intersection closest_hitPoint = Intersection(false);
    for(Face face: m_faces) {
        dvec3 vertex[3] = { face.v1, face.v2, face.v3 };
        Intersection hitPoint = rayFaceIntersection(r, vertex);
        if(hitPoint.intersect && hitPoint.t > ESP) {
            if(!closest_hitPoint.intersect || hitPoint.t < closest_hitPoint.t)
                //std::cout <<  "hit pos " << hitPoint.position << std::endl;
                closest_hitPoint = hitPoint;
        }
    }
    return closest_hitPoint;
}

NonhierBox::NonhierBox(const glm::vec3& pos, double size): m_pos(pos), m_size(size) {
    double r = size;
    vec3 vertex[8] = { pos, vec3(r+pos.x, pos.y, pos.z),
                     vec3(pos.x, r+pos.y, pos.z), vec3(r+pos.x, r+pos.y, pos.z),
                     vec3(pos.x, pos.y, r+pos.z), vec3(r+pos.x, pos.y, r+pos.z),
                     vec3(pos.x, r+pos.y, r+pos.z), vec3(r+pos.x, r+pos.y, r+pos.z) };
   // Face face = Face(vertex[], vertex[], vertex[]); //top
    m_faces.push_back(Face(vertex[6], vertex[2], vertex[7]));
      //bottom
    m_faces.push_back(Face(vertex[4], vertex[5], vertex[0]));
      //left
    m_faces.push_back(Face(vertex[2], vertex[6], vertex[0]));
      //right
    m_faces.push_back(Face(vertex[1], vertex[5], vertex[3]));
      //front
    m_faces.push_back(Face(vertex[2], vertex[0], vertex[3]));
      //back
    m_faces.push_back(Face(vertex[6], vertex[4], vertex[7]));
}
