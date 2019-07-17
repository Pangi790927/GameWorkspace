#ifndef PHYSICS_TEST_H
#define PHYSICS_TEST_H

#include "World.h"
#include "TestCamera.h"
#include "TestUtils.h"

class PhysicsTest {
public:
	constexpr const static float SELECT_THRESHOLD = 0.01;

	TestCamera camera;
	ShaderProgram shader;

	PhysicsTest () {
		camera -= World::up * 100;
		camera.horizRot += 120;

		shader = ShaderProgram({
			{GL_VERTEX_SHADER, "Shaders/textureShader.vert"},
			{GL_FRAGMENT_SHADER, "Shaders/textureShader.frag"}
		});
	}

	void update() {

	}

	void getInput (OpenglWindow& window, DrawContext& drawContext) {
		camera.getInput(window);
	}

	void render (DrawContext& drawContext) {
		DrawContext newContext = drawContext;
		newContext.view = camera.getTransform();

		shader.useProgram();
		shader.setMatrix("projectionMatrix", newContext.proj);
		shader.setMatrix("viewMatrix", newContext.view);
		shader.setMatrix("worldMatrix", newContext.world);

		for (int i = -100; i < 100; i += 2)
			for (int j = -100; j < 100; j += 2)
				Util::drawLine(Math::Point3f(i, 0, j),
						Math::Point3f(i, 0, j) + World::up);
	}
};

#endif