// Fall 2018
#ifndef SceneNode_hpp
#define SceneNode_hpp
#pragma once

#include "Material.hpp"

#include <glm/glm.hpp>

#include <list>
#include <string>
#include <iostream>
#include "Ray.hpp"



enum class NodeType {
	SceneNode,
	GeometryNode,
	JointNode
};

struct Intersection {
    bool intersect;
    glm::dvec3 position;
    glm::dvec3 normal;
    glm::dvec3 color;
    glm::dvec3 inDirection;
    double t = 0;
    Material * material;

    Intersection(bool i, glm::dvec3 p, dvec3 d):
    intersect(i), position(p), inDirection(d) {}
    Intersection(bool i):intersect(i) {};
    void setMaterial(Material * m);
};

class SceneNode {
public:
    SceneNode(const std::string & name);

	SceneNode(const SceneNode & other);

    virtual ~SceneNode();
    
	int totalSceneNodes() const;
    
    const glm::mat4& get_transform() const;
    const glm::mat4& get_inverse() const;
    
    void set_transform(const glm::mat4& m);
    
    void add_child(SceneNode* child);
    
    void remove_child(SceneNode* child);

	//-- Transformations:
    void rotate(char axis, float angle);
    void scale(const glm::vec3& amount);
    void translate(const glm::vec3& amount);


	friend std::ostream & operator << (std::ostream & os, const SceneNode & node);

    // Transformations
    glm::mat4 trans;
    glm::mat4 invtrans;
    
    std::list<SceneNode*> children;

	NodeType m_nodeType;
	std::string m_name;
	unsigned int m_nodeId;
    
    //Add
    virtual Intersection findIntersection(Ray r);

private:
	// The number of SceneNode instances.
	static unsigned int nodeInstanceCount;
};


#endif
