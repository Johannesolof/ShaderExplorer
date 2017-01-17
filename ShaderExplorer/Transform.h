#pragma once
#include <glm/glm.hpp>

class Transform
{
public:
	Transform() : matrix_(glm::mat4()) {}
	Transform(glm::mat4 matrix) : matrix_(matrix) {}
	~Transform();
	
	glm::mat4 Matrix() const;
	glm::vec3 Right() const;
	glm::vec3 Up() const;
	glm::vec3 Forward() const;
	glm::vec3 Position() const;
	glm::mat3 Rotation() const;

	static glm::vec3 const WorldUp;

	void Translate(glm::vec3 translation);
	void Rotate(float angle, glm::vec3 axis);
	void Scale(glm::vec3 scale);

private:
	glm::mat4 matrix_;
};

