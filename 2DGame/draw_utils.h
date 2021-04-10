#ifndef DRAW_UTILS_H
#define DRAW_UTILS_H

#define SIDE (0.05)
#define DEFAULT_COLOR	(Math::Point4f(0, 0, 0, 1))
#define CYAN			(Math::Point4f(0, 1, 1, 1))
#define RED				(Math::Point4f(1, 0, 0, 1))
#define GREEN			(Math::Point4f(0, 1, 0, 1))
#define BLUE			(Math::Point4f(0, 0, 1, 1))
#define MAGENTA			(Math::Point4f(1, 0, 1, 1))

void draw_grid(int x = 0, int y = 0, int w = 128, int h = 128,
		float side = SIDE)
{
	using namespace Math;
	using namespace Util;
	
	glLineWidth(1);

	Point2f d = Point2f(x*side, y*side);
	for (int i = 1; i < w; i++)
		drawLine(Point2f(i*side-1, 1) + d, Point2f(i*side-1, 1-h*side) + d,
				DEFAULT_COLOR);

	for (int i = 1; i < h; i++)
		drawLine(Point2f(-1, 1-i*side) + d, Point2f(w*side-1, 1-i*side) + d,
				DEFAULT_COLOR);

	glLineWidth(4);

	drawLine(Point2f(-1, 1) + d, Point2f(-1, 1-h*side) + d, RED);
	drawLine(Point2f(w*side-1, 1) + d, Point2f(w*side-1, 1-h*side) + d, RED);
	drawLine(Point2f(-1, 1) + d, Point2f(w*side-1, 1) + d, RED);
	drawLine(Point2f(-1, 1-h*side) + d, Point2f(w*side-1, 1-h*side) + d, RED);

	glLineWidth(1);
}

Math::Point2f point_at(int y, int x, float side = SIDE) {
	return Math::Point2f(x*side+side/2.-1, -y*side-side/2.+1);
}

void draw_circle(int y, int x, float side = SIDE,
		Math::Point4f color = DEFAULT_COLOR)
{
	using namespace Math;
	using namespace Util;

	Point2f center = point_at(y, x, side);
	float radius = side / 2.;
	float pi = 3.141592653589;
	float inc = pi / 10.;
	for (float i = 0; i <= 2 * pi; i += inc)
		drawLine(center + Point2f(sin(i), cos(i)) * radius,
				center + Point2f(sin(i + inc), cos(i + inc)) * radius, color);
}

void draw_cross(int y, int x, float side = SIDE,
		Math::Point4f color = DEFAULT_COLOR)
{
	using namespace Math;
	using namespace Util;

	Point2f center = point_at(y, x, side);
	Point2f corners[] = {
		{-1, -1},
		{-1,  1},
		{ 1,  1},
		{ 1, -1},
	};

	for (auto &&corner : corners)
		drawLine(center, center + corner * side / 2., color);
}

void draw_wall(int y, int x, float side = SIDE,
		Math::Point4f color = DEFAULT_COLOR)
{
	using namespace Math;
	using namespace Util;

	Point2f center = point_at(y, x, side);
	Point2f left_up = center + Point2f(-1, 1) * side / 2;

	int line_cnt = 5;
	float s = side / float(line_cnt);
	for (int i = 0; i <= line_cnt; i++)
		drawLine(Point2f(i*s, 0) + left_up, Point2f(i*s, -side) + left_up,
				color);

	for (int i = 0; i <= line_cnt; i++)
		drawLine(Point2f(0, -i*s) + left_up, Point2f(side, -i*s) + left_up,
				color);
}


#endif