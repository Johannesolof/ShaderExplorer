#pragma once
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include "Entity.h"

class Camera: public Entity
{
public:
	Camera::Camera(glm::vec3 position, glm::vec3 direction, 
		float vertical_fov, int window_width, int window_height, float near_plane, float far_plane);
	~Camera();
	void Update(int w, int h);
	void Update() override;

	glm::mat4 ProjectionMatrix() const { return projectionMatrix_; }

private:
	glm::mat4 projectionMatrix_;
	float vertical_fov_;
	int window_height_;
	int window_width_;
	float near_plane_;
	float far_plane_;
};

