#include "OpenglWindow.h"
#include "ShaderProgram.h"

// names for the vertex
struct VertexTexID		{ constexpr const static char name[] = "texId"; };
struct VertexTexCoord	{ constexpr const static char name[] = "texCoord"; };
struct VertexNormal		{ constexpr const static char name[] = "normal"; };
struct VertexPosition	{ constexpr const static char name[] = "pos"; };
struct VertexColor		{ constexpr const static char name[] = "color"; };

#include "Util.h"
#include "Mesh.h"
#include "MeshTools.h"
#include "DeprecatedVBOMeshDraw.h"

#include "DrawContext.h"
#include "GameUtil.h"
#include "GameMap.h"

using NormalVertexType = Vertex<
	Math::Point3f,	VertexPosition,
	Math::Point3f,	VertexNormal,
	Math::Point4f,	VertexColor,
	Math::Point2f,	VertexTexCoord
>;

int main (int argc, char const *argv[])
{
	using namespace Math;
	/// Window
	OpenglWindow window(800, 800, "Shaders Example");

	/// Shader
	ShaderProgram shader = ShaderProgram({
		{GL_VERTEX_SHADER, "Shaders/textureShader.vert"},
		{GL_FRAGMENT_SHADER, "Shaders/textureShader.frag"}
	});

	/// Draw Context
	DrawContext drawContext;

	window.setResize([&] (int x, int y, int w, int h){
		window.focus();
		glViewport(0, 0, w, h);
		drawContext.aspect = window.width / (float)window.height;
		drawContext.proj = scale<float>(drawContext.aspect, 0);
		shader.setMatrix("projectionMatrix", drawContext.proj);
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
	// glEnable(GL_BLEND);
	// glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Mesh<NormalVertexType> square1;
	Mesh<NormalVertexType> square2;
	Mesh<NormalVertexType> circle;

	Util::addSquare(square1, 0.3, Vec4f(0.34, 0.56, 0.1, 1));
	Util::addSquare(square2, 0.3, Vec4f(0.74, 0.23, 0.56, 1));
	Util::addCircle(circle, 0.01, 30, Vec4f(0.12, 0.23, 0.42, 1));

	DeprecatedVBOMeshDraw gSquare1(square1);
	DeprecatedVBOMeshDraw gSquare2(square2);
	DeprecatedVBOMeshDraw gCircle(circle);

	GameMap map(10, 10);

	while (window.active) {
		/// EXIT KEY:
		if (window.handleInput()) {
			if (window.keyboard.getKeyState(window.keyboard.ESC))
				window.requestClose();
		}
		// newGame.getInput(window, drawContext);
		window.mouse.update();
		
		window.focus();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		glLineWidth(1);
		glLineWidth(4);

		shader.setMatrix("projectionMatrix", identity<4, float>());
		shader.setMatrix("viewMatrix", identity<4, float>());
		shader.setMatrix("worldMatrix", identity<4, float>());

		// still needs to go:
		Point2f mousePos = Util::getMousePos(window.mouse,
				window.width, window.height);
		static Point2f mouseStart = 0;
		if (window.mouse.getOnceRmb()) {
			mouseStart = mousePos;
		}

		gCircle.draw(shader);
		gSquare1.draw(shader);
		gSquare2.draw(shader);

		map.draw(drawContext);

		shader.setMatrix("projectionMatrix", identity<4, float>());
		shader.setMatrix("viewMatrix", identity<4, float>());		

		if (window.mouse.getRmb()) {
			auto end = mousePos;

			auto red = Point4f(1, 0, 0, 1);

			shader.setMatrix("projectionMatrix", identity<4, float>());
			shader.setMatrix("viewMatrix", identity<4, float>());
			shader.setMatrix("worldMatrix", identity<4, float>());
			
			Util::drawLine(mouseStart, Point2f(mouseStart.x, mousePos.y, 0), red);
			Util::drawLine(Point2f(mouseStart.x, mousePos.y, 0), mousePos, red);
			Util::drawLine(mousePos, Point2f(mousePos.x, mouseStart.y, 0), red);
			Util::drawLine(Point2f(mousePos.x, mouseStart.y, 0), mouseStart, red);
		}
		window.swapBuffers();
	}
	return 0;
}