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

	void drawTriangle (
			const Math::Point2f& a,
			const Math::Point2f& b,
			const Math::Point2f& c,
			const Math::Point4f& color = Math::Point4f(0, 1, 1, 1))
	{
		glBegin(GL_TRIANGLES);
			glColor4fv(color.getPtr());
			glVertex2fv(a.getPtr());
			glVertex2fv(b.getPtr());
			glVertex2fv(c.getPtr());
		glEnd();
	}

	void drawCircle (
			const Math::Point2f& a,
			float r,
			const Math::Point4f& color = Math::Point4f(0, 1, 1, 1))
	{
		glBegin(GL_TRIANGLES);
			float pi = 3.141592653589;
			float s = pi * 2. / 30.0;
			glColor4fv(color.getPtr());
			for (float x = 0; x < 2 * pi; x += s) {
				Math::Point2f b = a + Math::Point2f(sin(x), cos(x)) * r;
				Math::Point2f c = a + Math::Point2f(sin(x + s), cos(x + s)) * r;
				glVertex2fv(a.getPtr());
				glVertex2fv(c.getPtr());
				glVertex2fv(b.getPtr());
			}
		glEnd();
	}

	void writeText(
			GlFont &font,
			Math::Point2f pos,
			float scale,
			int text_sz,
			const char *text,
			Math::Point2f limit,
			const Math::Point4f& color = Math::Point4f(0, 0, 0, 1))
	{
		auto m = font.get_meta(text_sz, scale);
		float offx = pos.x;
		float offy = pos.y - m.max_up;
		glBegin(GL_QUADS);
		while (*text) {
			auto c = font.get_char_draw(text_sz, *text, scale);
			glColor4f(color.r, color.g, color.b, color.a);
			glTexCoord2f(c.tex_top_left.x, c.tex_top_left.y);
			glVertex2f(offx + c.top_left.x, c.top_left.y + offy);

			glTexCoord2f(c.tex_top_left.x, c.tex_bot_right.y);
			glVertex2f(offx + c.top_left.x, c.bot_right.y + offy);

			glTexCoord2f(c.tex_bot_right.x, c.tex_bot_right.y);
			glVertex2f(offx + c.bot_right.x, c.bot_right.y + offy);
			
			glTexCoord2f(c.tex_bot_right.x, c.tex_top_left.y);
			glVertex2f(offx + c.bot_right.x, c.top_left.y + offy);
			offx += c.advance;
			if (offx + m.max_advance > limit.x) {
				offx = pos.x;
				offy -= scale;
			}
			if (offy < limit.y)
				break;
			text++;
		}
		glEnd();
	}

	Math::Point2f getMousePos (const Mouse& mouse, float width, float height) {
		return Math::Point2f (mouse.x / width * 2.0f - 1, 1.0f - mouse.y / height * 2.0f);
	}
}

#endif