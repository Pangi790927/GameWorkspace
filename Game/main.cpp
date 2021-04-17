#include "OpenglWindow.h"
#include "ShaderProgram.h"

// names for the vertex
struct VertexTexID		{ constexpr const static char name[] = "texId"; };
struct VertexTexCoord	{ constexpr const static char name[] = "texCoord"; };
struct VertexNormal		{ constexpr const static char name[] = "normal"; };
struct VertexPosition	{ constexpr const static char name[] = "pos"; };
struct VertexColor		{ constexpr const static char name[] = "color"; };

#include "Mesh.h"
#include "MeshTools.h"
#include "OBJLoader.h"
#include "FixedFunctionMeshDraw.h"
#include "DeprecatedVBOMeshDraw.h"
#include "DynamicVBOMeshDraw.h"

#include "GameCamera.h"
#include "GameUtil.h"
#include "DrawContext.h"
#include "Game.h"

using VertexType = Vertex<
	Math::Point2f,	VertexTexCoord,
	Math::Point3f,	VertexNormal,
	Math::Point3f,	VertexPosition,
	int,			VertexTexID
>;

using NormalVertexType = Vertex<
	Math::Point3f,	VertexPosition,
	Math::Point3f,	VertexNormal,
	Math::Point4f,	VertexColor,
	Math::Point2f,	VertexTexCoord
>;

void debugMessage(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
                  const GLchar *message, const void *userParam)
{
	printf("%s\n", message);
}

int main (int argc, char const *argv[])
{
	using namespace Math;
	/// Window
	OpenglWindow newWindow(800, 800, "QRev");

	/// Shader
	ShaderProgram shader = ShaderProgram({
		{GL_VERTEX_SHADER, "Shaders/textureShader.vert"},
		{GL_FRAGMENT_SHADER, "Shaders/textureShader.frag"}
	});

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
		// shader.setMatrix("projectionMatrix", drawContext.proj);
	});

	GLint flags;
	glGetIntegerv(GL_CONTEXT_FLAGS, &flags);

	if (flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(debugMessage, NULL);
		
		// enable all
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0,
				NULL, GL_TRUE);
		printf("Debug context !!!\n");
	}
	else
		printf("Not debug context !!!\n");


	/// RENDER OPTIONS
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_CULL_FACE);
	glLineWidth(6);
	glEnable(GL_DEPTH_TEST);
	glCullFace(GL_BACK);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_POLYGON_SMOOTH);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
	glDisable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Game newGame(128, 128);
	newGame.initRender();

	while (newWindow.active) {
		// EXIT KEY:
		if (newWindow.handleInput()) {
			if (newWindow.keyboard.getKeyState(newWindow.keyboard.ESC))
				newWindow.requestClose();
		}
		newGame.getInput(newWindow, drawContext);
		newWindow.mouse.update();
		
		newWindow.focus();
		glClearColor(1, 1, 1, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		newGame.update();
		
		glLineWidth(1);
		glEnable(GL_DEPTH_TEST);
		// glDisable(GL_BLEND);
		newGame.render(drawContext);
		
		glLineWidth(4);
		// glEnable(GL_BLEND);
		glDisable(GL_DEPTH_TEST);
		shader.setMatrix("projectionMatrix", identity<4, float>());
		shader.setMatrix("viewMatrix", identity<4, float>());
		shader.setMatrix("worldMatrix", identity<4, float>());
		newGame.render2D(drawContext);

		static int fps = 0;
		static int last_time = 0;
		fps += 1;

		if (last_time != time(0)) {
			last_time = time(0);
			printf("time %d fps: %d\n", last_time, fps);
			fps = 0;
		}

		newWindow.swapBuffers();
	}
	return 0;
}