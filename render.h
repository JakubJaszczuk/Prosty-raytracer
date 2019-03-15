#ifndef RENDER_H
#define RENDER_H


//#include <glm/glm.hpp>
//#include <glm/gtx/string_cast.hpp>

#include <vector>
#include <fstream>
#include <random>
#include <iostream>
#include <sstream>
#include <thread>

#include "object.h"
#include "camera.h"
#include "light_ray.h"

// Renderer
struct Render
{
	glm::vec2 pixelCoord(float fov, std::uint32_t resolution, std::uint32_t i, std::uint32_t j) const noexcept;
	LightRay createPrimaryRay(const Camera& camera, std::uint32_t resolution, std::uint32_t i, std::uint32_t j) const noexcept;
	void operator()(const std::vector<Object>& sceneData, const Camera& camera, std::uint32_t resolution) const noexcept;
	glm::u8vec3 colorConversion(glm::vec3 color) const noexcept;
	void render(const std::vector<Object>& sceneData, const Camera& camera, std::uint32_t resolution, std::vector<glm::u8vec3>& buffer, std::uint32_t stride, std::uint32_t ord) const noexcept;
};

#endif // RENDER_H
