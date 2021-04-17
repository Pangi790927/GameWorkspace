#ifndef GAME_CAMERA_H
#define GAME_CAMERA_H

#include "Camera.h"
#include "OpenglWindow.h"

class GameCamera : public Camera {
public:
	GameCamera (float speed = DEFAULT_MOV_SPEED, float rotSpeed = DEFAULT_ROT_SPEED)
	:  Camera(speed, rotSpeed) {}

	void getInput (OpenglWindow& window) {
		float speed = 1.0f;
		if (window.keyboard.getKeyState(window.keyboard.L_SHIFT))
			speed = 10;
		if (window.keyboard.getStateNoCase('W'))
			moveForward(speed);
		if (window.keyboard.getStateNoCase('S'))
			moveBackward(speed);
		if (window.keyboard.getStateNoCase('A'))
			moveLeft(speed);
		if (window.keyboard.getStateNoCase('D'))
			moveRight(speed);
		if (window.keyboard.getStateNoCase('Q'))
			moveDown(speed);
		if (window.keyboard.getStateNoCase('E'))
			moveUp(speed);

		if (window.mouse.lmb) {
			rotate(
				window.mouse.x - window.mouse.lastX,
				window.mouse.y - window.mouse.lastY,
				window.width,
				window.height
			);
		}
	}
};

#endif