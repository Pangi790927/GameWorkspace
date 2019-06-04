#ifndef GAME_MAP_H
#define GAME_MAP_H

#include <vector>

struct MapCell {
	
};

struct GameMap {
	using MapVertexType = Vertex<
		Math::Point3f,	VertexPosition,
		Math::Point3f,	VertexNormal,
		Math::Point4f,	VertexColor,
		Math::Point2f,	VertexTexCoord
	>;

	int width;
	int height;

	std::vector<std::vector<MapCell>> mapCells;

	ShaderProgram shader;
	Mesh<MapVertexType> mTerrain;
	DeprecatedVBOMeshDraw gTerrain;

	GameMap() {}
	GameMap (int width, int height) {}

	void initRender() {
		using namespace Math;
		shader = ShaderProgram({
			{GL_VERTEX_SHADER, "Shaders/mapShader.vert"},
			{GL_FRAGMENT_SHADER, "Shaders/mapShader.frag"}
		});
		for (int i = 0; i < height; i++)
			for (int j = 0; j < width; j++)
				Util::addSquare(
					mTerrain, 
					0.5,
					Vec4f((i + j) % 2, 0, 1, 1),
					translation<float>(i, 0, j)
				);

		gTerrain = DeprecatedVBOMeshDraw(mTerrain);
	}

	void draw (DrawContext& drawContext) {
		shader.useProgram();
		shader.setMatrix("projectionMatrix", drawContext.proj);
		shader.setMatrix("viewMatrix", drawContext.view);
		shader.setMatrix("worldMatrix", drawContext.world);
		gTerrain.draw(shader);	
	}
};

#endif