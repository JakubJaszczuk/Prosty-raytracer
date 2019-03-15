#include "light_ray.h"

std::vector<glm::vec3> LightRay::directions;

LightRay::LightRay(glm::vec3 origin, glm::vec3 direction) : Ray::Ray(origin, direction) {}

LightRay::LightRay(glm::vec3 origin, glm::vec3 direction, std::uint8_t maxDepth) : Ray::Ray(origin, direction), ttl {maxDepth} {}

glm::vec3 LightRay::trace(const std::vector<Object>& sceneData)
{
	// Najbliższe trafienie
	HitData hitData = closestIntersection(sceneData);
	if(hitData.objectIndex == std::numeric_limits<std::size_t>::max())
	{
		return glm::vec3(0.f, 0.f, 0.f);
	}
	// Obliczenia związane z materiałem
	const Object& obj = sceneData[hitData.objectIndex];
	const Material& mat = obj.material;
	if(mat.type == Material::Emission_t)
	{
		return mat.emission.color * mat.emission.power;
	}
	else if(ttl == 0)
	{
		return glm::vec3(0.f, 0.0f, 0.f);
	}
	else if(mat.type == Material::Diffuse_t)
	{
		const glm::uvec3& tri = obj.triangles[hitData.triangleIndex];
		glm::vec3 normal = glm::normalize(barycentricConversion(hitData.uv, obj.normals[tri[0]], obj.normals[tri[1]], obj.normals[tri[2]]));
		
		// Punkt trafienie należy "wyciągnąć" ponad powierzchnię obiektu -> dodanie t * normal
		glm::vec3 orig = barycentricConversion(hitData.uv, obj.vertices[tri[0]], obj.vertices[tri[1]], obj.vertices[tri[2]]) + normal * 0.00001f;
		std::uint32_t counter {0};
		glm::vec3 color = glm::vec3(0.f, 0.f, 0.f);
		for(std::size_t i = 0; i < LightRay::directions.size() && counter < LightRay::raysCount; ++i)
		{
			//glm::vec3 direction = Ray::randomDirection();
			glm::vec3 direction = LightRay::directions[i];
			if(glm::dot(normal, direction) > 0)
			{
				color += LightRay(orig, direction, ttl - 1).trace(sceneData);
				++counter;
			}
		}
		return mat.diffuse.color * (color / static_cast<float>(counter));
	}
	else if(mat.type == Material::Glossy_t)
	{
		return glm::vec3(0.f, 0.f, 0.f);
	}
	else
	{
		return glm::vec3(0.f, 0.f, 0.f);
	}	
}

/// Kolejność UV -> {1 - (u + v), u, v}
inline glm::vec3 barycentricConversion(const glm::vec2& uv, const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2)
{
	return p0 * (1 - (uv.x + uv.y)) + p1 * uv.x + p2 * uv.y;
}
