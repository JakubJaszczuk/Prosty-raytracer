#include "render.h"

///////////
/// Ray ///
///////////
HitData::HitData(glm::vec2 uv, float dist, std::size_t object, std::size_t triangle) :
	uv {uv}, objectIndex {object}, triangleIndex {triangle}, t {dist} {}


///////////
/// Ray ///
///////////
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

////////////////
/// LightRay ///
////////////////
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
		for(std::size_t i = 0; i < LightRay::directions.size() && counter < Render::raysCount; ++i)
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


//////////////
/// Object ///
//////////////
void Object::createAABB() noexcept
{
	aabbP1 = vertices[0];
	aabbP2 = vertices[0];
	for(const auto& v : vertices)
	{
		if(v.x < aabbP1.x) aabbP1.x = v.x;
		if(v.y < aabbP1.y) aabbP1.y = v.y;
		if(v.z < aabbP1.z) aabbP1.z = v.z;
		if(v.x > aabbP2.x) aabbP2.x = v.x;
		if(v.y > aabbP2.y) aabbP2.y = v.y;
		if(v.z > aabbP2.z) aabbP2.z = v.z;
	}
}

void Object::loadFromFile(const char* path)
{
	// Ładowanie geometrii
	int iloscV = 0;
	int iloscT = 0;
	std::ifstream file(path, std::ios::in);
	if(!file)
	{
		std::cerr << "Nie ma takiego pliku: ";
		std::cerr << path << std::endl;
		return;
	}
	std::string line;
	while(std::getline(file, line))
	{
		if(line.substr(0, 15) == "element vertex ")
		{
			std::istringstream s(line.substr(15));
			s >> iloscV;
		}
		else if(line.substr(0, 13) == "element face ")
		{
			std::istringstream s(line.substr(13));
			s >> iloscT;
		}
		else if(line == "end_header")
		{
			break;
		}
	}
	// Czytaj dane wierzchołków
	for(auto i = 0; i < iloscV; ++i)
	{
		std::getline(file, line);
		std::istringstream s(line);

		glm::vec3 v;
		glm::vec3 vn;
		s >> v.x;
		s >> v.y;
		s >> v.z;
		s >> vn.x;
		s >> vn.y;
		s >> vn.z;
		vertices.push_back(v);
		normals.push_back(vn);
	}
	// Czytaj dane trójkątów
	for(auto i = 0; i < iloscT; ++i)
	{
		std::getline(file, line);
		std::istringstream s(line.substr(2));
		glm::vec3 v;
		s >> v.x;
		s >> v.y;
		s >> v.z;
		triangles.push_back(v);
	}
	createAABB();
}


//////////////
/// Camera ///
//////////////
Camera::Camera(glm::vec3 position) : position {position} {}

Camera::Camera(float fov) : fov {fov} {}

Camera::Camera(glm::vec3 position, float fov) : position {position}, fov {fov} {}


//////////////
/// Render ///
//////////////
glm::vec2 Render::pixelCoord(float fov, std::uint32_t resolution, std::uint32_t i, std::uint32_t j) const noexcept
{
	const float halfScreenLength = glm::tan(glm::radians(fov / 2));
	const float halfPixelLength = halfScreenLength / resolution;
	return -halfScreenLength + halfPixelLength + 2 * halfPixelLength * glm::vec2(i, j);
}

LightRay Render::createPrimaryRay(const Camera& camera, std::uint32_t resolution, std::uint32_t i, std::uint32_t j) const noexcept
{
	const glm::vec2 coord = pixelCoord(camera.fov, resolution, i, j);
	const glm::vec3 dir = glm::normalize(glm::vec3(coord.x, 1.f, coord.y));
	return LightRay(camera.position, dir, 3);
}

void Render::render(const std::vector<Object>& sceneData, const Camera& camera, std::uint32_t resolution, std::vector<glm::u8vec3>& buffer, std::uint32_t stride, std::uint32_t ord) const noexcept
{
	// Obliczenia
	//#pragma omp parallel for
	for(std::uint32_t j = ord; j < resolution; j += stride)
	{
		std::cout << "Przetwarzam linię: " << j << std::endl; 
		for(std::uint32_t i = 0; i < resolution; ++i)
		{
			auto lightRay = createPrimaryRay(camera, resolution, i, resolution - j);
			auto color = lightRay.trace(sceneData);
			buffer[i + j * resolution] = colorConversion(color);
		}
	}
}

void Render::operator()(const std::vector<Object>& sceneData, const Camera& camera, std::uint32_t resolution) const noexcept
{
	// Bufor pikseli
	std::vector<glm::u8vec3> buffer(resolution * resolution);
	
	// TEST -> użycie OpenMP jest dużo prostsze
	const unsigned int concurent = 1;//std::thread::hardware_concurrency();
	std::vector<std::thread> threads;
	for(std::uint32_t i = 0; i < concurent; ++i)
	{
		threads.emplace_back(std::thread(&Render::render, this, std::ref(sceneData), std::ref(camera), resolution, std::ref(buffer), concurent, i));
	}
	// Czekanie na wątki
	for(std::uint32_t i = 0; i < concurent; ++i)
	{
		threads[i].join();
	}
	
	/*
	// Obliczenia
	#pragma omp parallel for
	for(std::uint32_t j = 0; j < resolution; ++j)
	{
		//std::cout << "Przetwarzam linię: " << j << std::endl; 
		for(std::uint32_t i = 0; i < resolution; ++i)
		{
			auto lightRay = createPrimaryRay(camera, resolution, i, resolution - j);
			auto color = lightRay.trace(sceneData);
			buffer[i + j * resolution] = colorConversion(color);
		}		
	}
	*/
	
	// Zapisanie do pliku
	std::ofstream ofs("img");
	ofs << "P3\n" << resolution << " " << resolution << "\n255\n";
	for(const auto& color : buffer)
	{
		ofs << (int)color.r << " " << (int)color.g << " " << (int)color.b << " ";
	}
}

glm::u8vec3 Render::colorConversion(glm::vec3 color) const noexcept
{
	return glm::u8vec3(glm::clamp(color, 0.f, 1.f) * 255.f);
}

//////////////////////
/// Free functions ///
//////////////////////

/// Kolejność UV -> {1 - (u + v), u, v}
inline glm::vec3 barycentricConversion(const glm::vec2& uv, const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2)
{
	return p0 * (1 - (uv.x + uv.y)) + p1 * uv.x + p2 * uv.y;
}

