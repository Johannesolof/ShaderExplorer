#pragma once
#include "Entity.h"
#include "Model.h"

class Object : public Entity
{
public:
	Object(Model * model) : model(model) {}
	~Object();

	Model	* model;
};

