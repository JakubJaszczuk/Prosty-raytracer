#ifndef RAY_H_
#define RAY_H_

#include <glm/glm.hpp>
#include "hit_data.h"
#include "object.h"

// Struktura promienia
struct Ray
{
	glm::vec3 origin;
	glm::vec3 dir;

	Ray() = default;
	Ray(glm::vec3 origin, glm::vec3 direction);
	virtual ~Ray() = default;

	bool slabs(const glm::vec3& p0, const glm::vec3& p1) const noexcept;
	HitData closestIntersection(const std::vector<Object>& sceneData) const noexcept;
	static glm::vec3 randomDirection() noexcept;
};

#endif // RAY_H_
