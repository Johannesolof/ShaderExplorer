#include "Camera.h"


Camera::Camera(glm::vec3 position, glm::vec3 direction, 
	float vertical_fov, int window_width, int window_height, float near_plane, float far_plane)
	: vertical_fov_(vertical_fov), window_width_(window_width), window_height_(window_height),
	  near_plane_(near_plane), far_plane_(far_plane)
{
	transform = Transform(
		glm::lookAt(position, position + direction, Transform::WorldUp)
	);
	projectionMatrix_ = glm::perspective(vertical_fov, float(window_width) / float(window_height), near_plane, far_plane);
}


Camera::~Camera()
{
	
}

void Camera::Update(int window_width, int window_height)
{
	window_width_ = window_width;
	window_height_ = window_height;
	projectionMatrix_ = glm::perspective(vertical_fov_, float(window_width_) / float(window_height_), near_plane_, far_plane_);
	projectionMatrix_ = lookAt(transform.Position(), transform.Forward(), Transform::WorldUp);
}

void Camera::Update()
{
	
}
