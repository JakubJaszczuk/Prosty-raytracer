#ifndef CAMERA_H_
#define CAMERA_H_

#include <glm/glm.hpp>

// Struktura kamery
struct Camera
{
	const glm::vec3 position {0.f, 0.f, 0.f};
	const float fov {60.f};

	Camera() = default;
	Camera(glm::vec3 position);
	Camera(float fov);
	Camera(glm::vec3 position, float fov);
	~Camera() = default;
};

#endif // CAMERA_H_
