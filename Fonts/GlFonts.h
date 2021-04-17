#ifndef GLFONTS_H
#define GLFONTS_H

#include <ft2build.h>
#include FT_FREETYPE_H
#undef FTERRORS_H_

const char* FT_Error_String(FT_Error err)
{
    #undef FTERRORS_H_
    #define FT_ERRORDEF( e, v, s )  case e: return s;
    #define FT_ERROR_START_LIST     switch (err) {
    #define FT_ERROR_END_LIST       }
    #include FT_ERRORS_H
    return "(Unknown error)";
}

#include <mutex>
#include <tuple>
#include <set>

#include "Util.h"
#include "Texture.h"

struct GlFont {
	struct font_meta_t {
		int ascent;
		int descent;

		int total_gap;
		int max_advance;
	};

	struct draw_meta_t {
		float max_advance;
		float max_up;
		float max_down;
		float gap;
	};

	struct char_draw_t {
		char c;
		Math::Point2f top_left;
		Math::Point2f bot_right;
		Math::Point2f tex_top_left;
		Math::Point2f tex_bot_right;
		float advance;
	};

	struct char_data_t {
		char c;
		int selected_sz;
		int w;
		int h;
		int x;
		int y;
		int bearing_x;
		int bearing_y;
		int advance;
	};

	int tex_w;
	int tex_h;

	Texture glfont;
	std::unordered_map<int, char_data_t> char_data;
	std::unordered_map<int, font_meta_t> meta;

	GlFont(const std::string &fontname, std::vector<int> fontsizes = {48}) {
		auto [ftptr, muptr] = init_lib();
		if (!ftptr)
			EXCEPTION("Can't get lib");

		struct remember_t {
			int id;
			int selected_sz;
			int c;
			int w;
			int h;
			int bearing_x;
			int bearing_y;
			int advance;
			std::vector<uint8_t> d;
		};
		Util::BinPack<remember_t *> bin_pack;

		std::map<std::tuple<int, int>, remember_t> to_remember;
		std::lock_guard guard(*muptr);
		{
			FT_Face face;
			int ret = 0;
			if ((ret = FT_New_Face(*ftptr, fontname.c_str(), 0, &face)))
				EXCEPTION("Failed to load font: %s", FT_Error_String(ret));


			for (auto sz : fontsizes) {
				int ymax = 0;
				int ymin = 10000000;
				int max_advance = 0;
				if (FT_Set_Pixel_Sizes(face, 0, sz) != 0)
					EXCEPTION("Failed to set sz: %d error: %s",
							sz, FT_Error_String(ret));

				// 32 is the first printable character
				for (int c = 32; c < 128; c++) {
					if (FT_Load_Char(face, c, FT_LOAD_RENDER))
						EXCEPTION("Failed to load character: %d[%c] err: %s",
								c, (char)c, FT_Error_String(ret));

					if (sz > 0xffff || sz < 0)
						EXCEPTION("Size: 0x%x is too large, max is: 0x%x",
								sz, 0xffff);
					int id = (((uint8_t)sz & 0xffff) << 8) | (uint8_t)c;
					int w = face->glyph->bitmap.width;
					int h = face->glyph->bitmap.rows;

					remember_t *rem = new remember_t;
					rem->d.resize((w + 2) * (h + 2));

					for (int i = 1; i < h + 1; i++)
						for (int j = 1; j < w + 1; j++)
							rem->d[i * (w + 2) + j] =
									face->glyph->bitmap.buffer[
									(i - 1) * w + j - 1];
					rem->id = id;
					rem->selected_sz = sz;
					rem->c = c;
					rem->w = w;
					rem->h = h;
					rem->bearing_x = face->glyph->bitmap_left;
					rem->bearing_y = face->glyph->bitmap_top;
					rem->advance = face->glyph->advance.x >> 6;

					max_advance = std::max(max_advance, rem->advance);
					ymax = std::max(ymax, rem->bearing_y);
					ymin = std::min(ymin, rem->bearing_y - h);
					bin_pack.insert(w + 2, h + 2, rem);
				}

				meta[sz].ascent = ymax;
				meta[sz].descent = ymin;
				meta[sz].total_gap = meta[sz].ascent - meta[sz].descent;
				meta[sz].max_advance = max_advance;
			}
			/* take data from face and store it for later use */
			FT_Done_Face(face);
		}
		glfont.alocateSpace(bin_pack.w, bin_pack.h, 1);
		tex_w = bin_pack.w;
		tex_h = bin_pack.h;
		for (auto &&rect : bin_pack) {
			for (int i = 0; i < rect.h; i++)
				for (int j = 0; j < rect.w; j++)
					glfont.get_r(i + rect.y, j + rect.x) =
							rect.data->d[i * rect.w + j];
			/* copy data to newly created image */
			char_data[rect.data->id].c = rect.data->c;
			char_data[rect.data->id].selected_sz = rect.data->selected_sz;
			char_data[rect.data->id].w = rect.data->w;
			char_data[rect.data->id].h = rect.data->h;
			char_data[rect.data->id].x = rect.x + 1;
			char_data[rect.data->id].y = rect.y + 1;
			char_data[rect.data->id].bearing_x = rect.data->bearing_x;
			char_data[rect.data->id].bearing_y = rect.data->bearing_y;
			char_data[rect.data->id].advance = rect.data->advance;
			delete rect.data;
		}
		glfont.generateOpenGLTexture(Texture::TEX_LINEAR);
	}

	draw_meta_t get_meta(int selected_sz, float height) {
		if (meta.find(selected_sz) == meta.end())
			EXCEPTION("Size not in font");
		auto &m = meta[selected_sz];
		return draw_meta_t{
			.max_advance = float(m.max_advance) / float(m.total_gap) * height,
			.max_up = float(m.ascent) / float(m.total_gap) * height,
			.max_down =	float(m.descent) / float(m.total_gap) * height,
			.gap = height, 
		};
	}

	char_draw_t get_char_draw(int selected_sz, char c, float height) {
		if (meta.find(selected_sz) == meta.end())
			EXCEPTION("Size not in font");
		if (int(c) < 32 || int(c) > 128)
			c = '?';

		int id = (((uint8_t)selected_sz & 0xffff) << 8) | (uint8_t)c;
		auto &chr = char_data[id];
		auto &m = meta[selected_sz];

		return char_draw_t {
			.c = chr.c,
			.top_left = {
				float(chr.bearing_x) / m.total_gap * height,
				float(chr.bearing_y) / m.total_gap * height
			},
			.bot_right = {
				float(chr.bearing_x + chr.w) / m.total_gap * height,
				float(chr.bearing_y - chr.h) / m.total_gap * height
			},
			.tex_top_left = {
				float(chr.x) / tex_w,
				float(chr.y) / tex_h
			},
			.tex_bot_right = {
				float(chr.x + chr.w) / tex_w,
				float(chr.y + chr.h) / tex_h
			},
			.advance = float(chr.advance) / m.total_gap * height,
		};
	}

	static std::tuple<FT_Library *, std::mutex *> init_lib(){
		struct UninitFt {
			FT_Library &ft;
			UninitFt(FT_Library &ft) : ft(ft) {}
			~UninitFt() { FT_Done_FreeType(ft); };
		};
		static int was_init = 1;
		static std::mutex mu;
		static FT_Library ft;
		static UninitFt uninit_ft(ft);

		std::lock_guard guard(mu);
		if (was_init == 0)
			return {&ft, &mu};
		if (was_init < 0)
			return {NULL, NULL};

		was_init = 0;
		int ret;
		if ((ret = FT_Init_FreeType(&ft)) != 0) {
			DBG("Can't init FreeType Lib: %s", FT_Error_String(ret));
			was_init = -1;
			return {NULL, NULL};
		}

		return {&ft, &mu};
	}
};

#endif