#include "Transform.h"
#include <glm/gtx/transform.hpp>
#include <glm/detail/type_mat.hpp>


Transform::~Transform()
{
}

glm::mat4 Transform::Matrix() const
{
	return matrix_;
}

glm::vec3 Transform::Right() const
{
	return glm::vec3(matrix_[0]);
}

glm::vec3 Transform::Up() const
{
	return glm::vec3(matrix_[1]);
}

glm::vec3 Transform::Forward() const
{
	return glm::vec3(matrix_[2]);
}

glm::vec3 Transform::Position() const
{
	return glm::vec3(matrix_[3]);
}

glm::mat3 Transform::Rotation() const
{
	return glm::mat3(matrix_);
}

glm::vec3 const Transform::WorldUp = glm::vec3(0, 1, 0);

void Transform::Translate(glm::vec3 translation)
{
	matrix_ = glm::translate(matrix_, translation);
}

void Transform::Rotate(float angle, glm::vec3 axis)
{
	matrix_ = glm::rotate(matrix_, angle, axis);
}

void Transform::Scale(glm::vec3 scale)
{
	matrix_ = glm::scale(matrix_, scale);
}


