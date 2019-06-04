#ifndef GAME_UTIL_H
#define GAME_UTIL_H

namespace Util
{
	void drawLine (
			const Math::Point2f& a,
			const Math::Point2f& b,
			const Math::Point4f& color = Math::Point4f(0, 1, 1, 1))
	{
		glBegin(GL_LINES);
			glColor4fv(color.getPtr());
			glVertex2fv(a.getPtr());
			glVertex2fv(b.getPtr());
		glEnd();
	}

	Math::Point2f getMousePos (const Mouse& mouse, float width, float height) {
		return Math::Point2f (mouse.x / width * 2.0f - 1, 1.0f - mouse.y / height * 2.0f);
	}
}

#endif