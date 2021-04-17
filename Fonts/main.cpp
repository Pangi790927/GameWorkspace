#include "OpenglWindow.h"
#include "ShaderProgram.h"
#include "TextureLoader.h"

#include "GlFonts.h"
#include "Util.h"

void drawLine (
		const Math::Point2f& a,
		const Math::Point2f& b,
		const Math::Point4f& color = Math::Point4f(0, 1, 1, 1))
{
	glBegin(GL_LINES);
		glColor4fv(color.getPtr());
		glVertex2fv(a.getPtr());
		glVertex2fv(b.getPtr());
	glEnd();
}

int main(int argc, char const *argv[])
{
	using namespace Math;
	/// Window
	OpenglWindow newWindow(960, 540, "Texture Example");

	/// Shader
	ShaderProgram exampleProg = ShaderProgram({
		{GL_VERTEX_SHADER, "textShader.vert"},
		{GL_FRAGMENT_SHADER, "textShader.frag"}
	});

	exampleProg.setMatrix("projectionMatrix", Math::identity<4, float>());
	exampleProg.setMatrix("viewMatrix", Math::identity<4, float>());
	exampleProg.setMatrix("worldMatrix", Math::identity<4, float>());

	glClearColor(1, 0, 1, 1);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  
	glEnable(GL_TEXTURE_2D);

	float scale = 0.035;
	int text_sz = 18;
	GlFont font("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
			{12, text_sz, 128, 256});

	// Texture testText = TextureLoader::load("testImage.jpg");
	font.glfont.bind();

	while (newWindow.active) {
		/// Input handling
		if (newWindow.handleInput())
			if (newWindow.keyboard.getKeyState(newWindow.keyboard.ESC))
				newWindow.requestClose();
		
		/// Drawing
		newWindow.focus();

		exampleProg.setFloat("alpha", 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		float n = 1;

		char msg[] = "Aenean et viverra lorem, sit amet consectetur nunc. "
		"Maecenas fermentum lacus et dictum sodales. Sed fermentum iaculis "
		"odio id posuere. Fusce nec turpis ut dolor vulputate mollis nec "
		"consectetur massa. Sed vestibulum sapien quis turpis aliquet "
		"convallis. Etiam eget sem ipsum. Praesent faucibus eros iaculis "
		"sem gravida viverra. Curabitur maximus vel lorem vel dapibus. "
		"Mauris non libero ac libero egestas eleifend. Integer tempus odio "
		"dolor, pharetra sollicitudin ipsum sagittis at. Ut hendrerit posuere"
		"nisl, at tincidunt quam bibendum nec. Aliquam ornare diam id ex "
		"pellentesque cursus. Donec placerat justo elementum neque suscipit"
		"maximus. Aliquam ligula ipsum, ornare ac varius in, suscipit in mi."
		"Cras a felis tempus, tristique mauris id, volutpat est.";

		write_text(font, {-0.9, 0.9}, scale, text_sz, msg, {0.8, -0.8});

		exampleProg.setFloat("alpha", 1);
		drawLine({-0.9, 0.9}, {1, 0.9});
		drawLine({-0.9, 0.9}, {-0.9, scale + 0.9});

		// for (int i = 0; i < n; i++) {
		// 	float scale = 1 - (float(i) / 10);
		// 	glBegin(GL_QUADS);
		// 		glColor3f(1, 0, 0);
		// 		glTexCoord2f(0, 0);
		// 		glVertex2f(-scale, scale);
					
		// 		glTexCoord2f(1, 0);
		// 		glVertex2f(scale, scale);

		// 		glTexCoord2f(1, 1);
		// 		glVertex2f(scale, -scale);
				
		// 		glTexCoord2f(0, 1);
		// 		glVertex2f(-scale, -scale);
		// 	glEnd();
		// }
		
		newWindow.swapBuffers();
	}
	return 0;
}