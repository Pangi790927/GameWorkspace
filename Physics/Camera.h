#ifndef CAMERA_H
#define CAMERA_H

#include "MathLib.h"
#include "World.h"

struct Camera : Math::Point3f {
	using Parent = Math::Point3f;

	float vertRot = 0.0f;
	float horizRot = 0.0f;

	constexpr const static float DEFAULT_MOV_SPEED = 1.0f;
	constexpr const static float DEFAULT_ROT_SPEED = 250.0f;
	float speed = DEFAULT_MOV_SPEED;
	float rotSpeed = DEFAULT_ROT_SPEED;

	Camera (float speed = DEFAULT_MOV_SPEED, float rotSpeed = DEFAULT_ROT_SPEED)
	: speed(speed), rotSpeed(rotSpeed) {}

	// initialy the players looks to the north
	Math::Mat4f getTransform() {
		return getRot() * getTransl();
	}

	Math::Mat4f getRot() {
		return Math::rot4<float>(-vertRot, World::west) * Math::rot4<float>(-horizRot, World::up);
	}

	Math::Mat4f getTransl() {
		return Math::translation<float>(x, y, z);
	}

	void rotate (float dx, float dy, float width, float height) {
		horizRot += dx / width * rotSpeed;
		vertRot -= dy / height * rotSpeed;
	}

	void moveForward (float ampl = 1.0f) {
		(*this) -= Math::rot3<float>(horizRot, World::up) * World::north * ampl * speed;	
	}

	void moveBackward (float ampl = 1.0f) {
		(*this) -= Math::rot3<float>(horizRot, World::up) * World::south * ampl * speed;
	}

	void moveLeft (float ampl = 1.0f) {
		(*this) -= Math::rot3<float>(horizRot, World::up) * World::west * ampl * speed;
	}

	void moveRight (float ampl = 1.0f) {
		(*this) -= Math::rot3<float>(horizRot, World::up) * World::east * ampl * speed;
	}

	void moveUp (float ampl = 1.0f) {
		(*this) -= World::up * ampl * speed;
	}

	void moveDown (float ampl = 1.0f) {
		(*this) -= World::down * ampl * speed;
	}
};

#endif