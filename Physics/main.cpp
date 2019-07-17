#include "OpenglWindow.h"
#include "ShaderProgram.h"

// names for the vertex
struct VertexTexID		{ constexpr const static char name[] = "texId"; };
struct VertexTexCoord	{ constexpr const static char name[] = "texCoord"; };
struct VertexNormal		{ constexpr const static char name[] = "normal"; };
struct VertexPosition	{ constexpr const static char name[] = "pos"; };
struct VertexColor		{ constexpr const static char name[] = "color"; };

#include "DrawContext.h"
#include "PhysicsTest.h"

int main (int argc, char const *argv[])
{
	using namespace Math;
	/// Window
	OpenglWindow newWindow(640, 480, "Shaders Example");

	/// Draw Context
	DrawContext drawContext;
	drawContext.yFov = 55.0;
	drawContext.zNear = 0.1;
	drawContext.zFar = 10000;

	newWindow.setResize([&] (int x, int y, int w, int h){
		newWindow.focus();
		glViewport(0, 0, w, h);
		drawContext.aspect = newWindow.width / (float)newWindow.height;
		drawContext.proj = projection<float>(
			drawContext.yFov,
			drawContext.aspect,
			drawContext.zNear,
			drawContext.zFar
		);
	});

	/// RENDER OPTIONS
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_CULL_FACE);
	glLineWidth(6);
	glEnable(GL_DEPTH_TEST);
	glCullFace(GL_BACK);
	glClearColor(1, 1, 1, 1);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_POLYGON_SMOOTH);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	PhysicsTest physicsTest;

	while (newWindow.active) {
		/// EXIT KEY:
		if (newWindow.handleInput()) {
			if (newWindow.keyboard.getKeyState(newWindow.keyboard.ESC))
				newWindow.requestClose();
		}
		physicsTest.getInput(newWindow, drawContext);
		newWindow.mouse.update();
		
		newWindow.focus();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		physicsTest.update();
		
		glLineWidth(1);
		physicsTest.render(drawContext);
		glLineWidth(4);

		newWindow.swapBuffers();
	}
	return 0;
}