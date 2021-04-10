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
#include "pathfinding.h"
#include "camera.h"

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
		{GL_VERTEX_SHADER, "Shaders/mapShader.vert"},
		{GL_FRAGMENT_SHADER, "Shaders/mapShader.frag"}
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

	bool wasLmb = false;
	GameMap map("map.json");
	// auto grid = load_grid("map.txt");
	Camera camera;

	while (window.active) {
		/// EXIT KEY:
		if (window.handleInput()) {
			if (window.keyboard.getKeyState(window.keyboard.ESC))
				window.requestClose();
		}
		// newGame.getInput(window, drawContext);
		camera.getInput(window);
		window.mouse.update();
		
		window.focus();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		glLineWidth(1);

		shader.setMatrix("projectionMatrix", identity<4, float>());
		shader.setMatrix("viewMatrix", identity<4, float>());
		shader.setMatrix("worldMatrix",
				translation<float>(-camera.pos.x, -camera.pos.y, 0));

		map.draw(camera.pos);

		static int sleep_reset = 0;
		static std::vector<Math::Point2i> to_animate;
		static std::vector<Math::Point2i> path;
		static std::vector<std::vector<Math::Point2i>> update_order;
		static int animate_inc = 0;
		static int path_inc = 0;
		static int sleep_timer = sleep_reset;

		sleep_timer--;
		if (sleep_timer <= 0) {
			sleep_timer = sleep_reset;
			animate_inc++;
			if (animate_inc >= to_animate.size())
				path_inc++;
		}
		if (animate_inc > to_animate.size())
			animate_inc = to_animate.size();
		if (path_inc > path.size())
			path_inc = path.size();

		for (int i = 0; i < animate_inc; i++) {
			draw_wall(to_animate[i].x, to_animate[i].y, SIDE, CYAN);
		}
		glLineWidth(2);
		for (int i = 0; i < path_inc; i++) {
			draw_wall(path[i].x, path[i].y, SIDE, GREEN);
		}
		glLineWidth(1);
		if (animate_inc != 0)
			draw_cross(to_animate[animate_inc - 1].x,
					to_animate[animate_inc - 1].y, SIDE, RED);
		if (animate_inc != 0)
			for (auto elem : update_order[animate_inc - 1])
				draw_circle(elem.x, elem.y, SIDE, RED);

		// still needs to go:
		Point2f mousePos = Util::getMousePos(window.mouse,
				window.width, window.height);
		static Point2f mouseStart = 0;
		if (window.mouse.getOnceLmb()) {
			mouseStart = mousePos;
		}

		shader.setMatrix("projectionMatrix", identity<4, float>());
		shader.setMatrix("viewMatrix", identity<4, float>());		

		if (window.mouse.getLmb()) {
			wasLmb = true;
			auto end = mousePos;

			auto red = RED;

			shader.setMatrix("projectionMatrix", identity<4, float>());
			shader.setMatrix("viewMatrix", identity<4, float>());
			shader.setMatrix("worldMatrix", identity<4, float>());
			
			Util::drawLine(mouseStart, Point2f(mouseStart.x, mousePos.y, 0), red);
			Util::drawLine(Point2f(mouseStart.x, mousePos.y, 0), mousePos, red);
			Util::drawLine(mousePos, Point2f(mousePos.x, mouseStart.y, 0), red);
			Util::drawLine(Point2f(mousePos.x, mouseStart.y, 0), mouseStart, red);
		}

		if (!window.mouse.getLmb() && wasLmb) {
			wasLmb = false;
			if ((mousePos - mouseStart).norm2() < 0.01) {
				printf("clicked\n");
				Math::Point2i a = map.get_index(mousePos + camera.pos -
						Math::Point2f(-1, 1));
				to_animate = animated_fill(map, a.x, a.y);
				// auto [visited, upd] = animated_dijkstra(map, a.y, -a.x, 0, 0);
				auto [visited, upd, p] = animated_A_star(map, a.x, a.y, 0, 0);
				to_animate = visited;
				update_order = upd;
				path = p;
				animate_inc = 0;
				path_inc = 0;
				sleep_timer = 10;
			}
			else {
				printf("selection\n");
			}
		}
		window.swapBuffers();
	}
	return 0;
}

/*
	Tasks:
		+ Make a chunk-based grid system
		+ enable movement with wasd
		- make buttons
		- maybe implement faster 2D graphics?
		- enable target place with rmb
		- make some props: walls, buildings etc.
		- try to implement D*Lite
		- try to implement FibHeap
		- test D*Lite, FibHeap, A*, BinaryHeap
		- D* should be able to recompute path after finding an obstacle,
			check how well it works
		- find out how to share paths between troops
		- optimize pathing for chunks, maybe check what pre-processing can be
			done to see if chunk is traversable?
		- implement troops movement
		- start building the actual game
*/