#ifndef TEST_UTILS_H
#define TEST_UTILS_H

namespace Util
{
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
}

#endif