#ifndef OBJECT_H_
#define OBJECT_H_

#define GLM_ENABLE_EXPERIMENTAL

#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <random>
#include <glm/glm.hpp>
#include <glm/gtx/component_wise.hpp>
#include <glm/gtx/intersect.hpp>
#include "material.h"

struct Object
{
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	std::vector<glm::uvec3> triangles;
	Material material;
	glm::vec3 aabbP1;
	glm::vec3 aabbP2;

	void createAABB() noexcept;

	void loadFromFile(const char* path);
};

#endif // OBJECT_H_

