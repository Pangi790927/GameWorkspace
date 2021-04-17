#include "OpenglWindow.h"
#include "ShaderProgram.h"
#include "TextureLoader.h"

int main(int argc, char const *argv[])
{
	/// Window
	OpenglWindow newWindow(960, 540, "Texture Example");

	/// Shader
	ShaderProgram exampleProg = ShaderProgram({
		{GL_VERTEX_SHADER, "textureShader.vert"},
		{GL_FRAGMENT_SHADER, "textureShader.frag"}
	});

	exampleProg.setMatrix("projectionMatrix", Math::identity<4, float>());
	exampleProg.setMatrix("viewMatrix", Math::identity<4, float>());
	exampleProg.setMatrix("worldMatrix", Math::identity<4, float>());

	exampleProg.setVector("uColor", Math::Vec4f(1, 1, 1, 1));

	/// Texture
	glEnable(GL_TEXTURE_2D);
	Texture testText = TextureLoader::load("testImage.jpg");
	testText.bind();

	while (newWindow.active) {
		/// Input handling
		if (newWindow.handleInput())
			if (newWindow.keyboard.getKeyState(newWindow.keyboard.ESC))
				newWindow.requestClose();
		
		/// Drawing
		newWindow.focus();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glBegin(GL_QUADS);
			glTexCoord2f(0, 0);
			glVertex2f(-1, 1);
				
			glTexCoord2f(1, 0);
			glVertex2f(1, 1);

			glTexCoord2f(1, 1);
			glVertex2f(1, -1);
			
			glTexCoord2f(0, 1);
			glVertex2f(-1, -1);
		glEnd();
		
		newWindow.swapBuffers();
	}
	return 0;
}