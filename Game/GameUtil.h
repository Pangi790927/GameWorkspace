#ifndef GAME_UTIL_H
#define GAME_UTIL_H

namespace Util
{
	struct Ray {
		Math::Point3f base;
		Math::Vec3f dir;

		Ray (Math::Point3f base = Math::Point3f(), Math::Vec3f dir = Math::Vec3f())
		: base(base), dir(dir) {}

		void draw (const Math::Point4f& color = Math::Point4f(0, 1, 1, 1)) {
			glBegin(GL_LINES);
				glColor4fv(color.getPtr());
				glVertex3fv(base.getPtr());
				glVertex3fv((base + dir).getPtr());
			glEnd();
		}
	};

	struct Plane {
		Math::Point3f base;
		Math::Vec3f norm;

		Plane (Math::Point3f base = Math::Point3f(), Math::Vec3f norm = Math::Vec3f())
		: base(base), norm(norm) {}
	};

	void drawLine (
			const Math::Point3f& a,
			const Math::Point3f& b,
			const Math::Point4f& color = Math::Point4f(0, 1, 1, 1))
	{
		glBegin(GL_LINES);
			glColor4fv(color.getPtr());
			glVertex3fv(a.getPtr());
			glVertex3fv(b.getPtr());
		glEnd();
	}

	Ray getMouseRay (const Math::Mat4f& view, const Math::Point2f& mouse2d,
			float fov, float zNear, float aspect)
	{
		using namespace Math;
		static const float pi = 3.141592653589; 

		Mat4f transform =
			rigidTransformInverse(view) * 
			translation<float>(
				tan(fov * pi / 180 / 2.0) * zNear * aspect * mouse2d.x,
				tan(fov * pi / 180 / 2.0) * zNear * mouse2d.y,
				-0.1
			);

		Point3f mouse3d = trunc<Point3f>(transform *
				Point4f(0, 0, 0, 1));
		Point3f center = trunc<Point3f>(rigidTransformInverse(view) *
				Point4f(0, 0, 0, 1));
		return Ray(center, mouse3d - center);
	}

	std::pair<bool, Math::Point3f> planeIntersect (const Ray& ray, const Plane& plane) {
		float denom = plane.norm.dot(ray.dir);
		if (std::abs(denom) > 0.00001f) {
			float t = (plane.base - ray.base).dot(plane.norm) / denom;
			if (t >= 0) {
				return std::make_pair(true, ray.base + ray.dir * t);
			}
			else
				return std::make_pair(false, Math::Point3f());
		}
		else
			return std::make_pair(false, Math::Point3f());
	}

	Math::Point2f getMousePos (const Mouse& mouse, float width, float height) {
		return Math::Point2f (mouse.x / width * 2.0f - 1, 1.0f - mouse.y / height * 2.0f);
	}

	Math::Point2f toScreen (const Math::Point3f& point,
			const Math::Mat4f& proj,
			const Math::Mat4f& view = Math::identity<4, float>(),
			const Math::Mat4f& world = Math::identity<4, float>())
	{
		Math::Point4f result = proj * view * world * Math::Point4f(point, 1.0);
		if (result.w > 0) {
			result /= result.w;
			return Math::Point2f(result.x, result.y);
		}
		else
			return Math::Point2f(-2, -2);
	}

	bool inSquare (Math::Point2f point, Math::Point2f a, Math::Point2f b) {
		if (a.x > b.x)
			std::swap(a.x, b.x);
		if (a.y > b.y)
			std::swap(a.y, b.y);
		if (a.x <= point.x && point.x <= b.x)
			if (a.y <= point.y && point.y <= b.y)
				return true;
		return false;
	}
}

#endif