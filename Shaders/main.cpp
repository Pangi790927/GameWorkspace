#include <iostream>
#include "OpenglWindow.h"
#include "ShaderProgram.h"

int main (int argc, char const *argv[])
{
	OpenglWindow newWindow(500, 500, "Shaders Example");

	ShaderProgram exampleProg = ShaderProgram({
		{GL_VERTEX_SHADER, "Examples/defaultShader.vert"},
		{GL_FRAGMENT_SHADER, "Examples/defaultShader.frag"}
	});

	exampleProg.setMatrix("projectionMatrix", Math::identity<4, float>());
	exampleProg.setMatrix("viewMatrix", Math::identity<4, float>());
	exampleProg.setMatrix("worldMatrix", Math::identity<4, float>());

	exampleProg.setVector("uColor", Math::Vec4f(1, 1, 1, 1));

	while (newWindow.active) {
		if (newWindow.handleInput())
			if (newWindow.keyboard.getKeyState(newWindow.keyboard.ESC))
				newWindow.requestClose();
		newWindow.focus();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glBegin(GL_LINES);
			glColor3f(1, 0, 0);
			glVertex2f(0, 0);
			glVertex2f(1, 1);
		glEnd();
		
		newWindow.swapBuffers();
	}
	return 0;
}