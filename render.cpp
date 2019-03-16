#include "render.h"

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
	return LightRay(camera.position, dir, LightRay::rayDepth);
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
	
	// TEST -> użycie OpenMP jest dużo prostsze, niż samemu robienie wątków
	const unsigned int concurent = std::thread::hardware_concurrency();
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
		
	// Zapisanie do pliku PPM
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
