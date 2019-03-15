#include "camera.h"

Camera::Camera(glm::vec3 position) : position {position} {}

Camera::Camera(float fov) : fov {fov} {}

Camera::Camera(glm::vec3 position, float fov) : position {position}, fov {fov} {}
