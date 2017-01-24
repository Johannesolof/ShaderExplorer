#pragma once
#include "Transform.h"

class Entity
{
public:
	virtual ~Entity() = default;

	virtual void Update() = 0;
	
	Transform transform;
};

