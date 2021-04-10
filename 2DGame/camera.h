#ifndef CAMERA_H
#define CAMERA_H

#include "MathLib.h"

#define DEFAULT_SPEED (0.02)

struct Camera {
	Math::Point2f pos;

	void getInput (OpenglWindow& window) {
		float speed = DEFAULT_SPEED;

		if (window.keyboard.getKeyState(window.keyboard.L_SHIFT))
			speed = DEFAULT_SPEED * 10.0f;
		if (window.keyboard.getStateNoCase('W'))
			pos.y += speed;
		if (window.keyboard.getStateNoCase('S'))
			pos.y -= speed;
		if (window.keyboard.getStateNoCase('A'))
			pos.x -= speed;
		if (window.keyboard.getStateNoCase('D'))
			pos.x += speed;
	}
};

#endif