#include "Object.h"


Object::Object(std::shared_ptr<Model> model)
{
	this->model_ = model;
}

Object::~Object()
{
}

std::shared_ptr<Model> Object::GetModel() const
{
	return model_;
}

void Object::Update()
{
}
