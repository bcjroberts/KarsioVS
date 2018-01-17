#pragma once

#include <glm/gtc/quaternion.hpp>
#include "../../Engine/Component.h"

class PhysicsComponent: public Component
{
private:
	int eventBitmask;
public:
	PhysicsComponent();
	~PhysicsComponent();
	int getEventBitmask() override;
	void processFrameUpdate() override;
};

