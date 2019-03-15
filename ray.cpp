#include "ray.h"

Ray::Ray(glm::vec3 origin, glm::vec3 direction) : origin {origin}, dir {direction} {}

// Algorytm przecięcia promienia z AABB na podstawie:
// http://www.jcgt.org/published/0007/03/04/paper-lowres.pdf
bool Ray::slabs(const glm::vec3& p0, const glm::vec3& p1) const noexcept
{
	const auto invRaydir = glm::vec3(1.f) / dir;
	const auto t0 = (p0 - origin) * invRaydir;
	const auto t1 = (p1 - origin) * invRaydir;
	const auto tmin = glm::min(t0, t1);
	const auto tmax = glm::max(t0, t1);
	return glm::compMax(tmin) <= glm::compMin(tmax);
}

HitData Ray::closestIntersection(const std::vector<Object>& sceneData) const noexcept
{
	// Zapamiętywanie najbliższego trafienia
	glm::vec2 closestUV {0.f, 0.f};
	float closestT {std::numeric_limits<float>::infinity()};
	std::size_t tri {std::numeric_limits<std::size_t>::max()};
	std::size_t obj {std::numeric_limits<std::size_t>::max()};
	
	// Obliczenia
	glm::vec2 uv;
	float t {-std::numeric_limits<float>::infinity()};
	for(std::size_t i = 0; i < sceneData.size(); ++i)
	{
		if(slabs(sceneData[i].aabbP1, sceneData[i].aabbP2))
		{
			const Object& object = sceneData[i];
			for(std::size_t j = 0; j < object.triangles.size(); ++j)
			{
				const glm::uvec3& triangle = object.triangles[j];
				auto b = glm::intersectRayTriangle(origin, dir, object.vertices[triangle[0]], object.vertices[triangle[1]], object.vertices[triangle[2]], uv, t);

				// Warunek aktualizacji trafionego trójkąta
				if(b && t > 0 && t < closestT)
				{
					closestT = t;
					closestUV = uv;
					tri = j;
					obj = i;
				}
			}
		}
	}
	return HitData(closestUV, closestT, obj, tri);
}

glm::vec3 Ray::randomDirection() noexcept
{
	static std::random_device rd;
	static std::mt19937 engine(rd());
	static std::uniform_real_distribution<float> distribution {-1.f, 1.f};
	return glm::normalize(glm::vec3(distribution(engine), distribution(engine), distribution(engine)));
}
