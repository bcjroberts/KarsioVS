#pragma once
#include "../../Engine/Component.h"
#include "DriveComponent.h"
#include "../../Main/initOpenGL/OpenGL_Deps.h"



class ControllableComponent : public Component
{
public:
	bool player;	// true = player controlled, false = ai controlled
	vehicleInput input;
	ControllableComponent(bool nplayer);
	void getInput();
	~ControllableComponent();
};

