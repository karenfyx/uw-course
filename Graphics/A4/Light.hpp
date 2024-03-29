// Winter 2019

#pragma once

#include <iosfwd>

#include <glm/glm.hpp>

// Represents a simple point light.
struct Light {
  Light();
  static int lightCount;
  int id;
  glm::vec3 colour;
  glm::vec3 position;
  double falloff[3];
};

std::ostream& operator<<(std::ostream& out, const Light& l);
