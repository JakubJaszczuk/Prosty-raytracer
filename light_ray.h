#ifndef LIGHT_RAY_H_
#define LIGHT_RAY_H_

#include <glm/glm.hpp>
#include "ray.h"

// Struktura wyspecjalizowanego promienia
struct LightRay : Ray
{
	static std::uint32_t raysCount;
	static std::uint32_t rayDepth;
	static std::vector<glm::vec3> directions;

	const std::uint8_t ttl {4};

	LightRay() = default;
	LightRay(glm::vec3 origin, glm::vec3 direction);
	LightRay(glm::vec3 origin, glm::vec3 direction, std::uint8_t maxDepth);
	~LightRay() = default;

	glm::vec3 trace(const std::vector<Object>& sceneData);
};

/// Kolejność UV -> {1 - (u + v), u, v}
inline glm::vec3 barycentricConversion(const glm::vec2& uv, const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2);

#endif // LIGHT_RAY_H_
