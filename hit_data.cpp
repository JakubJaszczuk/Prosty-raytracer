#include "hit_data.h"

HitData::HitData(glm::vec2 uv, float dist, std::size_t object, std::size_t triangle) :
	uv {uv}, objectIndex {object}, triangleIndex {triangle}, t {dist} {}
