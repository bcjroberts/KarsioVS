#pragma once
#include "../../Engine/Component.h"
#include "DriveComponent.h"
#include "../../Main/initOpenGL/OpenGL_Deps.h"


struct upgradeInput {
    bool upgradeChassis = false;
    bool upgradeArmor = false;
    bool upgradeRam = false;
    bool upgradeGun = false;
};

class ControllableComponent : public Component
{
public:
	bool player;	// true = player controlled, false = ai controlled
	vehicleInput input;
    upgradeInput upInput;
	ControllableComponent(bool nplayer);
	void getInput();
	~ControllableComponent();
};

