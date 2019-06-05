// Winter 2019

#include <iostream>
#include <fstream>

#include <glm/ext.hpp>
#include <glm/gtx/intersect.hpp>
#include <glm/gtx/io.hpp>

// #include "cs488-framework/ObjFileDecoder.hpp"
#include "Mesh.hpp"

Mesh::Mesh( const std::string& fname )
	: m_vertices()
	, m_faces()
{
	std::string code;
	double vx, vy, vz;
	size_t s1, s2, s3;

	std::ifstream ifs( fname.c_str() );
	while( ifs >> code ) {
		if( code == "v" ) {
			ifs >> vx >> vy >> vz;
			m_vertices.push_back( glm::vec3( vx, vy, vz ) );
		} else if( code == "f" ) {
			ifs >> s1 >> s2 >> s3;
			m_faces.push_back( Triangle( s1 - 1, s2 - 1, s3 - 1 ) );
		}
	}
}

std::ostream& operator<<(std::ostream& out, const Mesh& mesh)
{
  out << "mesh {";
  /*
  
  for( size_t idx = 0; idx < mesh.m_verts.size(); ++idx ) {
  	const MeshVertex& v = mesh.m_verts[idx];
  	out << glm::to_string( v.m_position );
	if( mesh.m_have_norm ) {
  	  out << " / " << glm::to_string( v.m_normal );
	}
	if( mesh.m_have_uv ) {
  	  out << " / " << glm::to_string( v.m_uv );
	}
  }

*/
  out << "}";
  return out;
}

Intersection rayTriangleIntersection(Ray r, dvec3 vertex[]) {
    //std::cout << "Mesh::hit" << std::endl;
    Intersection hitPoint = Intersection(false);
    dvec3 AminusB = vertex[0] - vertex[1];
    dvec3 AminusC = vertex[0] - vertex[2];
    dvec3 AminusS = vertex[0] - r.s;
    mat3 M = {r.d, AminusB, AminusC};
    if(abs(determinant(M)) <= 0.05) { return hitPoint; }
    //std::cout << "det " << determinant(M) << std::endl;
    vec3 res = inverse(M) * AminusS;
    if(res.y > 0 && res.z > 0 && res.y+res.z < 1) {
        hitPoint = Intersection(true, r.getPoint(res.x), r.d);
        hitPoint.t = res.x;
        hitPoint.normal = normalize(cross(-AminusB,-AminusC));
        
    }
    return hitPoint;
}

Intersection Mesh::hit(Ray r) {
    Intersection closest_hitPoint = Intersection(false);
    for(Triangle t: m_faces) {
        dvec3 vertex[3] = { m_vertices[t.v1], m_vertices[t.v2], m_vertices[t.v3] };
        Intersection hitPoint = rayTriangleIntersection(r, vertex);
        if(hitPoint.intersect && hitPoint.t > ESP) {
            if(!closest_hitPoint.intersect || hitPoint.t < closest_hitPoint.t)
                closest_hitPoint = hitPoint;
        }
    }
    return closest_hitPoint;
}


