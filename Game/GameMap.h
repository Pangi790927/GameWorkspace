#ifndef GAME_MAP_H
#define GAME_MAP_H

#include "MapTile.h"

class GameMap {
public:
	using MapVertexType = Vertex<
		Math::Point3f,	VertexPosition,
		Math::Point3f,	VertexNormal,
		Math::Point4f,	VertexColor,
		Math::Point2f,	VertexTexCoord
	>;

	int width;
	int height;
	float scale;
	std::vector<std::vector<MapTile>> tiles;

	Mesh<MapVertexType> mMap;
	DeprecatedVBOMeshDraw gMap;
	ShaderProgram shader;

	GameMap (int width, int height, float scale = 50)
	: width(width), height(height), scale(scale),
	tiles(height, std::vector<MapTile>(width)) {
		aquire(0, 0);
	}

	auto& operator () (int x, int y) {
		return tiles[x][y];
	}

	const auto& operator () (int x, int y) const {
		return tiles[x][y];
	}

	auto& operator () (const Math::Point2i& pos) {
		return tiles[pos.x][pos.y];
	}

	const auto& operator () (const Math::Point2i& pos) const {
		return tiles[pos.x][pos.y];
	}

	auto& operator [] (int i) {
		return tiles[i];
	}

	const auto& operator [] (int i) const {
		return tiles[i];
	}

	bool inside (int x, int y) const {
		return x >= 0 && y >= 0 && x < tiles.size() && y < tiles[0].size();
	}

	bool inside (const Math::Point2i& pos) const {
		return inside(pos.x, pos.y);
	}

	Math::Point2i getTilePos (const Math::Point2f& pos) const {
		int x = pos.x / scale + 0.5;
		int y = pos.y / scale + 0.5;
		if (!inside(x, y))
			return Math::Point2i(0, 0);
		return Math::Point2i(x, y);
	}

	Math::Point2i getTilePos (const Math::Point3f& pos) const {
		return getTilePos(Math::Point2f(pos.x, pos.z));
	}

	Math::Point3f toWorld (const Math::Point2i& pos) const {
		return Math::Point3f(pos.x * scale, 0, pos.y * scale);
	}

	auto& operator () (const Math::Point2f& pos) {
		return (*this)(getTilePos(pos));
	}

	const auto& operator () (const Math::Point2f& pos) const {
		return (*this)(getTilePos(pos));
	}

	void initRender() {
		using namespace Math;
		shader = ShaderProgram({
			{GL_VERTEX_SHADER, "Shaders/mapShader.vert"},
			{GL_FRAGMENT_SHADER, "Shaders/mapShader.frag"}
		});
		for (int i = 0; i < height; i++)
			for (int j = 0; j < width; j++)
				Util::addSquareW(mMap, 1.5,
						Vec4f((i + j) % 2, 0, 1, 1),
						translation<float>(i, 0, j) *
						rot4<float>(-90, World::west) *
						scale);

		gMap = DeprecatedVBOMeshDraw(mMap);
	}

	void render (DrawContext& drawContext) {
		shader.useProgram();
		shader.setMatrix("projectionMatrix", drawContext.proj);
		shader.setMatrix("viewMatrix", drawContext.view);
		shader.setMatrix("worldMatrix", drawContext.world);
		gMap.draw(shader);
	}

	bool aquire (int x, int y) {
		return inside(x, y) && tiles[x][y].aquire();
	}

	bool canAquire (int x, int y) const {
		return inside(x, y) && tiles[x][y].canAquire();
	}

	void release(int x, int y) {
		if (inside(x, y)) {
			tiles[x][y].release();
		}
	}

	bool aquire (const Math::Point2i& pos) {
		return aquire(pos.x, pos.y);
	}

	bool canAquire (const Math::Point2i& pos) const {
		return canAquire(pos.x, pos.y);
	}

	void release (const Math::Point2i& pos) {
		release(pos.x, pos.y);
	}
};

#endif