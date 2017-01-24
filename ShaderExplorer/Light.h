#pragma once
#include "Entity.h"

class Light : public Entity
{
public:
	Light();
	~Light();

	void Update() override {};
};

