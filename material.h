#ifndef MATERIAL_H_
#define MATERIAL_H_

#include <glm/glm.hpp>

struct Material
{
	struct Emission
	{
		glm::vec3 color;
		float power;
	};

	struct Diffuse
	{
		glm::vec3 color;
	};

	struct Glossy
	{
		glm::vec3 color;
		float roughtness;
	};

	enum MaterialType {Emission_t, Diffuse_t, Glossy_t, Count};

	MaterialType type;
	union
	{
		Emission emission;
		Diffuse diffuse;
		Glossy glossy;
	};
};

#endif // MATERIAL_H_
