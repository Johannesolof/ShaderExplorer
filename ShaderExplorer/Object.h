#pragma once
#include "Entity.h"
#include "Model.h"
#include <memory>

class Object : public Entity
{
public:
	Object(std::shared_ptr<Model> model);
	~Object();

	std::shared_ptr<Model> GetModel() const;

	void Update() override;

private:
	std::shared_ptr<Model> model_;
};

