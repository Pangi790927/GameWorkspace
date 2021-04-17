#ifndef GAME_H
#define GAME_H

#include "Player.h"
#include "GameMap.h"
#include "Unit.h"
#include "TankUnit.h"
#include "GameCamera.h"
#include "GameUtil.h"

class Game {
public:
	constexpr const static float SELECT_THRESHOLD = 0.01;

	Player player;
	GameMap map;
	std::vector<std::shared_ptr<Unit>> units;
	std::vector<std::shared_ptr<Unit>> selectedUnits;
	ShaderProgram unitShader;
	GameCamera camera;

	Math::Point3f selection;
	Math::Point2f mouse_pos_screen;
	Math::Point3f mouse_pos_map;
	Math::Point2f lastPos;
	bool wasRmb = false;
	bool wallAdd = false;

	Game (int mapWidth, int mapHeight) : map(mapWidth, mapHeight) {
		spawnUnit(1, 1, 3, 3);
		spawnUnit(1, 1, 3, 4);
		spawnUnit(1, 1, 3, 5);
		spawnUnit(1, 1, 6, 3);
		spawnUnit(1, 1, 7, 3);
		camera -= World::up * 100;
		camera.horizRot += 120;
		camera.speed = Camera::DEFAULT_MOV_SPEED * 5;
	}

	void spawnUnit (int player, int type, int i, int j) {
		if (map.canAquire(i, j)) {
			map.aquire(i, j);
			units.push_back(std::shared_ptr<Unit>(new TankUnit(player, type)));
			units.back()->pos = Math::Point3f(i, 0, j) * map.scale;
			units.back()->dest.setFinish(Math::Point2i(i, j));
		}
	}

	void update() {
		static int timeout = 0;
		timeout++;
		if (timeout > 2) {
			timeout = 0;
			for (auto&& unit : units) {
				unit->move(map);
			}
		}
	}

	auto mouseToMap(Math::Point2f pos, DrawContext& drawContext) {
		using namespace Math;
		auto ray = Util::getMouseRay(
			drawContext.view,
			pos,
			drawContext.yFov,
			drawContext.zNear,
			drawContext.aspect
		);
		auto intersect = Util::planeIntersect(
			ray,
			Util::Plane(
				Point3f(),
				World::up
			)
		);
		Point3f res;
		if (intersect.first) {
			res = intersect.second;
			return std::tuple{true, res};
		}
		else {
			return std::tuple{false, res};
		}
	}

	void getInput (OpenglWindow& window, DrawContext& drawContext) {
		DrawContext newContext = drawContext;
		newContext.view = camera.getTransform();

		camera.getInput(window);
		
		mouse_pos_screen = Util::getMousePos(window.mouse,
				window.width, window.height);
		auto [intersect, map_pos] = mouseToMap(mouse_pos_screen, newContext);
		if (intersect)
			mouse_pos_map = map_pos;
		
		if (window.keyboard.getKeyState('t'))
			wallAdd = true;
		else
			wallAdd = false;
		if (window.mouse.getRmb() && !wasRmb) {
			lastPos = Util::getMousePos(window.mouse,
					window.width, window.height);
			wasRmb = true;
		}
		if (!window.mouse.getRmb()) {
			if (wasRmb) {
				Math::Point2f currentPos = Util::getMousePos(window.mouse,
						window.width, window.height); 
				if ((lastPos - currentPos).norm2() < SELECT_THRESHOLD)
					onRightClick(lastPos, newContext);
				else
					onMakeSelection(lastPos, currentPos, newContext);
			}
			wasRmb = false;
		}
	}

	void onMakeSelection (Math::Point2f a, Math::Point2f b,
			DrawContext& drawContext)
	{
		selectedUnits.clear();
		for (auto&& unit : units) {
			Math::Point2f result = Util::toScreen(
				unit->pos,
				drawContext.proj,
				drawContext.view
			);
			if (Util::inSquare(result, a, b))
				selectedUnits.push_back(unit);
		}
	}

	void onRightClick (Math::Point2f pos,
			DrawContext& drawContext)
	{
		using namespace Math;
		auto [was_intersect, map_pos] = mouseToMap(pos, drawContext);
		if (was_intersect) {
			selection = map_pos;
			auto tilePos = map.getTilePos(Point2f(selection.x, selection.z));
			if (wallAdd) {
				spawnUnit(1, 1,
					tilePos.x,
					tilePos.y
				);
			}
			else {
				for (auto&& unit : selectedUnits) {
					unit->dest.setFinish(tilePos);
				}
			}
		}
	}

	void initRender() {
		map.initRender();
		unitShader = ShaderProgram({
			{GL_VERTEX_SHADER, "Shaders/mapShader.vert"},
			{GL_FRAGMENT_SHADER, "Shaders/mapShader.frag"}
		});
	}

	void input (Keyboard<>& keyboard, Mouse& mouse) {

	} 

	void render (DrawContext& drawContext) {
		DrawContext newContext = drawContext;
		newContext.view = camera.getTransform();
		map.render(newContext);
		for (auto&& unitPtr : units) {
			unitPtr->render(newContext, unitShader);
		}
		unitShader.useProgram();
		unitShader.setMatrix("projectionMatrix", newContext.proj);
		unitShader.setMatrix("viewMatrix", newContext.view);
		unitShader.setMatrix("worldMatrix", newContext.world);
		for (auto&& selectUnit : selectedUnits) {
			Util::drawLine(selectUnit->pos, selectUnit->pos + World::up * 10);
			// for (auto&& pos : selectUnit->dest.path) {
			// 	Util::drawLine(map.toWorld(pos), map.toWorld(pos) + World::up * 7,
			// 			Math::Point4f(1, 0, 0, 1));
			// }
		}
		Util::drawLine(selection, selection + World::up * 10);
		Util::drawLine(mouse_pos_map, mouse_pos_map + World::up * 15);
	}

	void render2D(DrawContext& drawContext) {
		using namespace Math;
		if (wasRmb) {
			auto red = Point4f(1, 0, 0, 1);

			Point3f mouseStart = lastPos;
			Point3f mousePos = mouse_pos_screen;;

			Util::drawLine(mouseStart, Point3f(mouseStart.x, mousePos.y, 0), red);
			Util::drawLine(Point3f(mouseStart.x, mousePos.y, 0), mousePos, red);
			Util::drawLine(mousePos, Point3f(mousePos.x, mouseStart.y, 0), red);
			Util::drawLine(Point3f(mousePos.x, mouseStart.y, 0), mouseStart, red);
		}
	}
};

#endif