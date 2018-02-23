#include "ControllableComponent.h"
#include "../../Engine/Core.h"

#define GAMEPAD_A		0
#define GAMEPAD_B		1
#define GAMEPAD_X		2
#define GAMEPAD_Y		3
#define GAMEPAD_LB		4
#define GAMEPAD_RB		5
#define GAMEPAD_BACK	6
#define GAMEPAD_START	7

#define GAMEPAD_LEFT_JOYSTICK_X		0
#define GAMEPAD_LEFT_JOYSTICK_Y		1
#define GAMEPAD_RIGHT_JOYSTICK_X	2
#define GAMEPAD_RIGHT_JOYSTICK_Y	3
#define GAMEPAD_LEFT_TRIGGER		4
#define GAMEPAD_RIGHT_TRIGGER		5

ControllableComponent::ControllableComponent(bool nplayer) : Component(CONTROLLABLE) {
	player = nplayer;
}

// joystick controller stuff
void ControllableComponent::getInput() {
	// why is everything set to true in default? Did I break something? O:
	input.accel = input.brake = input.handbrake = input.steering = 0.0f;

	int present = glfwJoystickPresent(GLFW_JOYSTICK_1);
	if (present) {
		int axesCount, buttonCount;
		const float *axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &axesCount);
		const unsigned char *buttons = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &buttonCount);
		
		input.brake = axes[GAMEPAD_LEFT_TRIGGER];
		input.accel = axes[GAMEPAD_RIGHT_TRIGGER];
        input.steering = axes[GAMEPAD_LEFT_JOYSTICK_X];
	} else { //Use keyboard inputs instead if no controller is present.
        input.brake = glfwGetKey(Core::globalWindow, GLFW_KEY_DOWN) ? 1.0f : 0.0f;
        input.accel = glfwGetKey(Core::globalWindow, GLFW_KEY_UP) ? 1.0f : 0.0f;
        input.steering = glfwGetKey(Core::globalWindow, GLFW_KEY_RIGHT) ? -1.0f : 0.0f;
        input.steering = glfwGetKey(Core::globalWindow, GLFW_KEY_LEFT) ? 1.0f : input.steering;
	}
}


ControllableComponent::~ControllableComponent() {
}
