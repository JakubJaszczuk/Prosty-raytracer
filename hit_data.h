#ifndef HIT_DATA_H_
#define HIT_DATA_H_

#include <glm/glm.hpp>

// Struktura na dane o trafieniu
struct HitData
{
	glm::vec2 uv;
	std::size_t objectIndex;
	std::size_t triangleIndex;
	float t;

	HitData() = default;
	HitData(glm::vec2 uv, float dist, std::size_t object, std::size_t triangle);
	~HitData() = default;
};

#endif // HIT_DATA_H_

