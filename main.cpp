#include <iostream>
#include <fstream>

#include <vector>

#include "render.h"

int main(int argc, char const *argv[])
{
	std::cout.sync_with_stdio(false);
	// Można ustawić rozdzielczość z linii poleceń
	std::uint32_t res {256};
	if(argc > 1)
	{
		res = std::atoi(argv[1]);
	}

	// Ustawienie sceny
	Object cube1;
	cube1.loadFromFile("scene/cube1.ply");
	Object cube2;
	cube2.loadFromFile("scene/cube2.ply");
	Object back;
	back.loadFromFile("scene/back.ply");
	Object top;
	top.loadFromFile("scene/top.ply");
	Object bottom;
	bottom.loadFromFile("scene/bottom.ply");
	Object left;
	left.loadFromFile("scene/left.ply");
	Object right;
	right.loadFromFile("scene/right.ply");
	Object light;
	light.loadFromFile("scene/light.ply");

	cube1.material.type = Material::Diffuse_t;
	cube1.material.diffuse.color = glm::vec3(0.8f, 0.5f, 0.5f);
	
	cube2.material.type = Material::Diffuse_t;
	cube2.material.diffuse.color = glm::vec3(0.3f, 1.f, 0.8f);

	left.material.type = Material::Diffuse_t;
	left.material.diffuse.color = glm::vec3(1.f, 0.f, 0.f);

	right.material.type = Material::Diffuse_t;
	right.material.diffuse.color = glm::vec3(0.f, 1.f, 0.f);

	back.material.type = Material::Diffuse_t;
	back.material.diffuse.color = glm::vec3(0.9f);

	top.material.type = Material::Diffuse_t;
	top.material.diffuse.color = glm::vec3(0.9f);

	bottom.material.type = Material::Diffuse_t;
	bottom.material.diffuse.color = glm::vec3(0.9f);

	light.material.type = Material::Emission_t;
	light.material.emission.color = glm::vec3(1.f);
	light.material.emission.power = 15.f;

	std::vector<Object> scene;
	scene.push_back(light);
	scene.push_back(cube1);
	scene.push_back(cube2);
	scene.push_back(back);
	scene.push_back(left);
	scene.push_back(right);
	scene.push_back(top);
	scene.push_back(bottom);

	// Rendering

	// Wygenerowanie losowych kierunków (można ich użyć zamiast dynamicznie generować kierunki promieni odbitych)
	LightRay::directions.resize(LightRay::raysCount * 1000);
	//#pragma opm parallel for
	for(std::size_t i = 0; i < LightRay::directions.size(); ++i)
	{
		LightRay::directions[i] = Ray::randomDirection();
	}

	//Właściwy rendering sceny
	Render render;
	Camera camera(glm::vec3(0.f, -3.f, 0.f), 55.f);
	render(scene, camera, res);

	return 0;
}

