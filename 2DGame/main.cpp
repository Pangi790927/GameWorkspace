#include "OpenglWindow.h"
#include "ShaderProgram.h"

// names for the vertex
struct VertexTexID		{ constexpr const static char name[] = "texId"; };
struct VertexTexCoord	{ constexpr const static char name[] = "texCoord"; };
struct VertexNormal		{ constexpr const static char name[] = "normal"; };
struct VertexPosition	{ constexpr const static char name[] = "pos"; };
struct VertexColor		{ constexpr const static char name[] = "color"; };

#include "GlFonts.h"
#include "Util.h"
#include "Mesh.h"
#include "MeshTools.h"
#include "DeprecatedVBOMeshDraw.h"

#include "DrawContext.h"
#include "GameUtil.h"
#include "pathfinding.h"
#include "camera.h"
#include "ImGui.h"

using NormalVertexType = Vertex<
	Math::Point3f,	VertexPosition,
	Math::Point3f,	VertexNormal,
	Math::Point4f,	VertexColor,
	Math::Point2f,	VertexTexCoord
>;

std::vector<std::string> dbg_strings(128);
#define WRITE_SCREEN(line, fmt, ...)\
do {\
	char buff[1024] = {0};\
	snprintf(buff, sizeof(buff), fmt, ##__VA_ARGS__);\
	if (line < dbg_strings.size())\
		dbg_strings[line] = buff;\
	else\
		EXCEPTION("line out of bounds");\
} while (0);
std::string fps_text;

struct Unit {
	struct Draw {
		int type;
		Math::Point2f a;
		Math::Point2f b;
		Math::Point3f color;
	};
	
	Math::Point2i origin_mark;
	std::set<Math::Point2i> marks;
	std::vector<Draw> d;

	void add_mark(Math::Point2f pos, float grid_step) {
		auto posi = Math::Point2i(
				roundl((pos.x - fmodf(pos.x, grid_step)) / grid_step) -
					(pos.x < 0),
				roundl((pos.y - fmodf(pos.y, grid_step)) / grid_step) -
					(pos.y < 0));
		std::cout << posi.tr() << std::endl;
		origin_mark = Math::Point2i(std::min(origin_mark.x, posi.x),
				std::min(origin_mark.y, posi.y));

		if (marks.find(posi) != marks.end())
			return ;
		d.push_back(Unit::Draw{
			.type = 4,
			.a = pos,
		});
		marks.insert(posi);

		bool first = true;
		for (auto &&mark : marks) {
			if (first) {
				first = false;
				origin_mark = mark;
			}
			origin_mark = Math::Point2i(std::min(origin_mark.x, mark.x),
					std::min(origin_mark.y, mark.y));
		}
	}

	void rm_mark(Math::Point2f pos, float grid_step) {
		auto posi = Math::Point2i(
				roundl((pos.x - fmodf(pos.x, grid_step)) / grid_step) -
					(pos.x < 0),
				roundl((pos.y - fmodf(pos.y, grid_step)) / grid_step) -
					(pos.y < 0));
		marks.erase(posi);

		bool first = true;
		for (auto &&mark : marks) {
			if (first) {
				first = false;
				origin_mark = mark;
			}
			origin_mark = Math::Point2i(std::min(origin_mark.x, mark.x),
					std::min(origin_mark.y, mark.y));
		}
	}

	void pop_last(float grid_step) {
		if (d.size() > 0) {
			if (d.back().type == 4)
				rm_mark(d.back().a, grid_step);

			d.resize(d.size() - 1);
		}
	}

	void clear() {
		marks.clear();
		d.clear();
	}
};

static void debug_info(bool* p_open)
{
	const float PAD = 10.0f;
	static int corner = 1;
	ImGuiIO& io = ImGui::GetIO();
	ImGuiWindowFlags window_flags =
			ImGuiWindowFlags_NoDecoration |
			ImGuiWindowFlags_AlwaysAutoResize |
			ImGuiWindowFlags_NoSavedSettings |
			ImGuiWindowFlags_NoFocusOnAppearing |
			ImGuiWindowFlags_NoNav;
	const ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImVec2 work_pos = viewport->WorkPos;
	ImVec2 work_size = viewport->WorkSize;
	ImVec2 window_pos, window_pos_pivot;
	window_pos.x = (corner & 1) ? (work_pos.x + work_size.x - PAD) :
		(work_pos.x + PAD);
	window_pos.y = (corner & 2) ? (work_pos.y + work_size.y - PAD) :
		(work_pos.y + PAD);
	window_pos_pivot.x = (corner & 1) ? 1.0f : 0.0f;
	window_pos_pivot.y = (corner & 2) ? 1.0f : 0.0f;
	ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
	window_flags |= ImGuiWindowFlags_NoMove;
	ImGui::SetNextWindowBgAlpha(0.80f); // Transparent background
	if (ImGui::Begin("Debug Window", p_open, window_flags)) {
		ImGui::Text("DEBUG                                   ");
		ImGui::Separator();
		if (ImGui::IsMousePosValid())
			ImGui::Text("Mouse Position: (%.1f,%.1f)", io.MousePos.x, io.MousePos.y);
		else
			ImGui::Text("Mouse Position: <invalid>");
		for (auto &&dbg_str : dbg_strings)
			if (dbg_str != "")
				ImGui::Text(dbg_str.c_str());
	}
	ImGui::End();
}

static bool main_menu(bool* p_open, bool* draw_active)
{
	const float PAD = 10.0f;
	static int corner = 1;
	ImGuiIO& io = ImGui::GetIO();
	ImGuiWindowFlags window_flags =
			ImGuiWindowFlags_NoDecoration |
			ImGuiWindowFlags_AlwaysAutoResize |
			ImGuiWindowFlags_NoSavedSettings |
			ImGuiWindowFlags_NoFocusOnAppearing |
			ImGuiWindowFlags_NoNav;
	const ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImVec2 work_pos = viewport->WorkPos;
	ImVec2 work_size = viewport->WorkSize;
	ImVec2 window_pos, window_pos_pivot;
	window_pos.x = work_pos.x + work_size.x - PAD;
	window_pos.y = work_pos.y + PAD + 200;
	window_pos_pivot.x = 1;
	window_pos_pivot.y = 0;
	ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
	window_flags |= ImGuiWindowFlags_NoMove;
	ImGui::SetNextWindowBgAlpha(1.0f); // Transparent background
	if (ImGui::Begin("Menu", p_open, window_flags)) {
		ImGui::Text("Menu                                    ");
		if (ImGui::Button("Create unit"))
			*draw_active = true;
		if (ImGui::Button("Quit")) {
			ImGui::End();
			return false;
		}
		ImGui::Separator();
	}
	ImGui::End();
	return true;
}

static void unit_draw_popup(bool* p_open)
{
	using namespace Math;
	ImGuiIO& io = ImGui::GetIO();
	if (ImGui::IsKeyDown('w') && io.KeyCtrl) {
		*p_open = false;
		return;
	}
	if (!ImGui::Begin("Canvas", p_open))
	{
		ImGui::End();
		return;
	}

	static Unit unit;
	static Point2f scrolling(0.0f, 0.0f);
	static bool adding = false;
	static int to_draw = 1;
	if (ImGui::Button("Done draw")) {
		*p_open = false;
	}
	if (ImGui::Button("Line"))
		to_draw = 1;
	ImGui::SameLine();
	if (ImGui::Button("Circle"))
		to_draw = 2;
	ImGui::SameLine();
	if (ImGui::Button("Square"))
		to_draw = 3;
	ImGui::SameLine();
	if (ImGui::Button("Mark"))
		to_draw = 4;
	ImGui::SameLine();
	ImGui::Text(to_draw == 1 ? "Line" : (
			to_draw == 2 ? "Circle" : (
			to_draw == 3 ? "Square" : "Mark")));

	ImGui::Text("Mouse Left: drag to add lines");
	ImGui::Text("Mouse Right: drag to scroll, click for context menu.");

	static Point3f draw_color = Point3f(1, 1, 0);
	ImGui::ColorEdit3("color 1", draw_color.getPtr());

	Point2f canvas_p0 = ImGui::GetCursorScreenPos();      // ImDrawList API uses screen coordinates!
	Point2f canvas_sz = ImGui::GetContentRegionAvail();   // Resize canvas to what's available
	if (canvas_sz.x < 50.0f) canvas_sz.x = 50.0f;
	if (canvas_sz.y < 50.0f) canvas_sz.y = 50.0f;
	Point2f canvas_p1 = canvas_p0 + canvas_sz;

	// Draw border and background color
	ImDrawList* draw_list = ImGui::GetWindowDrawList();
	draw_list->AddRectFilled(canvas_p0, canvas_p1, IM_COL32(50, 50, 50, 255));
	draw_list->AddRect(canvas_p0, canvas_p1, IM_COL32(255, 255, 255, 255));

	// This will catch our interactions
	ImGui::InvisibleButton("canvas", canvas_sz,
			ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight);
	const bool is_hovered = ImGui::IsItemHovered(); // Hovered
	const bool is_active = ImGui::IsItemActive();   // Held
	const Point2f origin = canvas_p0 + scrolling;
	const Point2f mouse_pos_in_canvas = Point2f(io.MousePos.x, io.MousePos.y) -
			origin;
	printf("hovered: %d active: %d\n", is_hovered, is_active);

	// Add first and second point
	const float GRID_STEP = 64.0f;
	if (to_draw == 4 && is_hovered && !adding &&
			ImGui::IsMouseClicked(ImGuiMouseButton_Left))
	{
		unit.add_mark(mouse_pos_in_canvas, GRID_STEP);
	}
	else if (to_draw == 4 && adding) {
		if (!ImGui::IsMouseDown(ImGuiMouseButton_Left))
			adding = false;
	}
	else if (is_hovered && !adding &&
			ImGui::IsMouseClicked(ImGuiMouseButton_Left))
	{
		unit.d.push_back(Unit::Draw{
			.type = to_draw,
			.a = mouse_pos_in_canvas,
			.b = mouse_pos_in_canvas,
			.color = draw_color * 255
		});
		adding = true;
	}
	else if (adding)
	{
		unit.d.back().b = mouse_pos_in_canvas;
		if (!ImGui::IsMouseDown(ImGuiMouseButton_Left))
			adding = false;
	}

	// Pan (we use a zero mouse threshold when there's no context menu)
	// You may decide to make that threshold dynamic based on whether the mouse is hovering something etc.
	const float mouse_threshold_for_pan = -1.0f;
	if (is_active && ImGui::IsMouseDragging(ImGuiMouseButton_Right, mouse_threshold_for_pan))
	{
		scrolling.x += io.MouseDelta.x;
		scrolling.y += io.MouseDelta.y;
	}

	// Context menu (under default mouse threshold)
	ImVec2 drag_delta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Right);
	if (ImGui::IsMouseReleased(ImGuiMouseButton_Right) && drag_delta.x == 0.0f && drag_delta.y == 0.0f)
		ImGui::OpenPopupOnItemClick("context");
	if (ImGui::BeginPopup("context"))
	{
		if (adding) {
			unit.pop_last(GRID_STEP);
		}
		adding = false;
		if (ImGui::MenuItem("Remove one", NULL, false, unit.d.size() > 0)) {
			unit.pop_last(GRID_STEP);
		}
		if (ImGui::MenuItem("Remove all", NULL, false, unit.d.size() > 0)) {
			unit.clear();
		}
		ImGui::EndPopup();
	}

	// Draw grid + all lines in the canvas
	draw_list->PushClipRect(canvas_p0, canvas_p1, true);
	for (float x = fmodf(scrolling.x, GRID_STEP); x < canvas_sz.x; x += GRID_STEP)
		draw_list->AddLine(ImVec2(canvas_p0.x + x, canvas_p0.y),
				ImVec2(canvas_p0.x + x, canvas_p1.y), IM_COL32(200, 200, 200, 40));
	for (float y = fmodf(scrolling.y, GRID_STEP); y < canvas_sz.y; y += GRID_STEP)
		draw_list->AddLine(ImVec2(canvas_p0.x, canvas_p0.y + y),
				ImVec2(canvas_p1.x, canvas_p0.y + y), IM_COL32(200, 200, 200, 40));

	float thickness = 4.0;
	for (auto &&mark : unit.marks) {
		draw_list->AddRectFilled(origin + Point2f(mark) * GRID_STEP,
				origin + (mark + Point2f(1, 1)) * GRID_STEP,
				IM_COL32(50, 250, 50, 40));
	}
	if (unit.marks.size()) {
		draw_list->AddRectFilled(origin + unit.origin_mark * GRID_STEP,
				origin + (unit.origin_mark + Point2f(1, 1)) * GRID_STEP,
				IM_COL32(250, 50, 250, 40));
	}
	for (auto &&d : unit.d) {
		if (d.type == 1) {
			draw_list->AddLine(origin + d.a, origin + d.b,
					IM_COL32(d.color.r, d.color.g, d.color.b, 255), thickness);
		}
		else if (d.type == 2) {
			draw_list->AddCircle(origin + d.a, (d.a - d.b).norm2(),
					IM_COL32(d.color.r, d.color.g, d.color.b, 255),
					0, thickness);			
		}
		else if (d.type == 3) {
			draw_list->AddRect(origin + d.a, origin + d.b,
					IM_COL32(d.color.r, d.color.g, d.color.b, 255),
					0, 0, thickness);
		}
	}
	draw_list->PopClipRect();

	ImGui::End();
}

static bool ui_logic() {
	static bool dbg_active = true;
	static bool menu_active = true;
	static bool draw_active = false;

	if (dbg_active)
		debug_info(&dbg_active);
	if (menu_active)
		if (!main_menu(&menu_active, &draw_active))
			return false;
	if (draw_active)
		unit_draw_popup(&draw_active);
	return true;
}

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

	ShaderProgram text_shader = ShaderProgram({
		{GL_VERTEX_SHADER, "Shaders/textShader.vert"},
		{GL_FRAGMENT_SHADER, "Shaders/textShader.frag"}
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
	// glEnable(GL_DEPTH_TEST);
	glCullFace(GL_BACK);
	glClearColor(1, 1, 1, 1);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_POLYGON_SMOOTH);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	bool wasLmb = false;
	GameMap map("map.json");
	// auto grid = load_grid("map.txt");
	GlFont font("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", {32});
	Camera camera;

	init_imgui(&window);

	float fps = 0;
	while (window.active) {
		/// EXIT KEY:
		ImGuiIO& io = ImGui::GetIO();
		bool enable_mouse = true;
		bool enable_keyboard = true;
		if (window.handleInput()) {
			if (window.keyboard.getKeyState(window.keyboard.ESC))
				window.requestClose();
			process_events_imgui(&window);
		}
		if (io.WantCaptureMouse) {
			enable_mouse = false;
		}
		if (io.WantCaptureKeyboard)
			enable_keyboard = false;
		if (enable_keyboard)
			camera.getInput(window);
		window.mouse.update();
		
		window.focus();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		glLineWidth(1);

		shader.useProgram();

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
		// draw_circlef(1, 1, SIDE * 2, RED);
		draw_wallf(0, 0, SIDE, MAGENTA);
		draw_wallf(0, 1, SIDE, MAGENTA);
		draw_wallf(1, 0, SIDE, MAGENTA);
		draw_wallf(1, 1, SIDE, MAGENTA);

		// still needs to go:
		Point2f mousePos = Util::getMousePos(window.mouse,
				window.width, window.height);
		static Point2f mouseStart = 0;
		if (enable_mouse && window.mouse.getOnceLmb()) {
			mouseStart = mousePos;
		}

		shader.setMatrix("projectionMatrix", identity<4, float>());
		shader.setMatrix("viewMatrix", identity<4, float>());		
		shader.setMatrix("worldMatrix", identity<4, float>());

		if (enable_mouse && window.mouse.getLmb()) {
			wasLmb = true;
			auto end = mousePos;

			auto red = RED;
			
			Util::drawLine(mouseStart, Point2f(mouseStart.x, mousePos.y, 0), red);
			Util::drawLine(Point2f(mouseStart.x, mousePos.y, 0), mousePos, red);
			Util::drawLine(mousePos, Point2f(mousePos.x, mouseStart.y, 0), red);
			Util::drawLine(Point2f(mousePos.x, mouseStart.y, 0), mouseStart, red);
		}

		if (enable_mouse && !window.mouse.getLmb() && wasLmb) {
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

		text_shader.useProgram();
		text_shader.setMatrix("projectionMatrix", Math::identity<4, float>());
		text_shader.setMatrix("viewMatrix", Math::identity<4, float>());
		text_shader.setMatrix("worldMatrix", Math::identity<4, float>());
		text_shader.setFloat("alpha", 0);
		font.glfont.bind();

		WRITE_SCREEN(0, "fps: %f", fps);
		WRITE_SCREEN(1, "This is another text");

		static uint64_t start_fps_time = 0;
		uint64_t curr_fps_time;
		static float curr_fps = 0;
		curr_fps++;
		if ((curr_fps_time = Util::get_time_ms()) - start_fps_time > 1000) {
			fps = curr_fps / float(curr_fps_time - start_fps_time) * 1000.;
			start_fps_time = curr_fps_time;
			curr_fps = 0;
		}

		predraw_imgui(&window);
		if (!ui_logic())
			window.requestClose();
		render_imgui();

		window.swapBuffers();
	}
	uninit_imgui();
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