#include "OpenglWindow.h"
#include "ShaderProgram.h"

// names for the vertex
struct VertexTexID {};
struct VertexTexCoord {};
struct VertexNormal {};
struct VertexPosition {};
struct VertexColor {};

#include "Mesh.h"
#include "OBJLoader.h"
#include "FixedFunctionMeshDraw.h"
#include "DeprecatedVBOMeshDraw.h"
#include "DynamicVBOMeshDraw.h"

using VertexType = Vertex<
	Math::Point2f,	VertexTexCoord,
	Math::Point3f,	VertexNormal,
	Math::Point3f,	VertexPosition,
	int,			VertexTexID
>;

int main (int argc, char const *argv[])
{
	/// Window
	OpenglWindow newWindow(500, 500, "Shaders Example");

	/// Shader
	ShaderProgram shader = ShaderProgram({
		{GL_VERTEX_SHADER, "textureShader.vert"},
		{GL_FRAGMENT_SHADER, "textureShader.frag"}
	});

	newWindow.setResize([&] (int x, int y, int w, int h){
		newWindow.focus();
		glViewport(0, 0, w, h);
		shader.setMatrix("projectionMatrix", Math::projection<float>(
				55.0, newWindow.width / (float)newWindow.height, 0.1, 1000));
	});

	shader.setMatrix("viewMatrix", Math::identity<4, float>());
	shader.setMatrix("worldMatrix", Math::translation<float>(0, -10, -50));

	/// Mesh
	Mesh mesh = OBJLoader<VertexType>().loadMesh("Obj/", "Lowpoly_tree_sample.obj");
	DeprecatedVBOMeshDraw gMesh(mesh);
	DynamicVBOMeshDraw gdMesh(mesh);

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	while (newWindow.active) {
		if (newWindow.handleInput())
			if (newWindow.keyboard.getKeyState(newWindow.keyboard.ESC))
				newWindow.requestClose();
		
		newWindow.focus();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		FixedFunctionMeshDraw().draw(mesh, shader);
		// gMesh.draw(shader);
		// gdMesh.draw(shader);
		newWindow.swapBuffers();
	}
	return 0;
}