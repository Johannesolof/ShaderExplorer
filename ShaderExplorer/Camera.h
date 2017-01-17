#pragma once
#include <glm/glm.hpp>
#include "Entity.h"

class Camera: public Entity
{
public:
	Camera(glm::vec3 position, glm::vec3 direction);
	~Camera();



};

