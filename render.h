#ifndef RENDER_H
#define RENDER_H

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/intersect.hpp>
#include <glm/gtx/component_wise.hpp>

#include <vector>
#include <fstream>
#include <random>
#include <iostream>
#include <sstream>
#include <thread>

//////////////////
/// Structures ///
//////////////////
// Materiał
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

// Obiekt sceny
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

// Struktura wyspecjalizowanego promienia
struct LightRay : Ray
{
	static std::vector<glm::vec3> directions;

	const std::uint8_t ttl {4};

	LightRay() = default;
	LightRay(glm::vec3 origin, glm::vec3 direction);
	LightRay(glm::vec3 origin, glm::vec3 direction, std::uint8_t maxDepth);
	~LightRay() = default;

	glm::vec3 trace(const std::vector<Object>& sceneData);
};

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

// Renderer
struct Render
{
	static constexpr std::uint32_t raysCount {10};

	glm::vec2 pixelCoord(float fov, std::uint32_t resolution, std::uint32_t i, std::uint32_t j) const noexcept;
	LightRay createPrimaryRay(const Camera& camera, std::uint32_t resolution, std::uint32_t i, std::uint32_t j) const noexcept;
	void operator()(const std::vector<Object>& sceneData, const Camera& camera, std::uint32_t resolution) const noexcept;
	glm::u8vec3 colorConversion(glm::vec3 color) const noexcept;
	void render(const std::vector<Object>& sceneData, const Camera& camera, std::uint32_t resolution, std::vector<glm::u8vec3>& buffer, std::uint32_t stride, std::uint32_t ord) const noexcept;
};

// Free functions

/// Kolejność UV -> {1 - (u + v), u, v}
inline glm::vec3 barycentricConversion(const glm::vec2& uv, const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2);

//std::size_t RAYS_COUNT = 0;

#endif // RENDER_H
