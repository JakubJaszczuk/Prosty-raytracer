#include "object.h"

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

