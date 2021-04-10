#ifndef GAME_MAP_H
#define GAME_MAP_H

#include "json.h"

#define CHK_SZ 32

struct GameMap {
	struct chunk_t {
		char m[CHK_SZ][CHK_SZ] = {0};

		char *operator [] (int i) {
			if (i < 0 || i >= CHK_SZ)
				return NULL;
			return m[i];
		}

		const char *operator [] (int i) const {
			if (i < 0 || i >= CHK_SZ)
				return NULL;
			return m[i];
		}
	};

	using key_t = Math::Point2i;
	std::function<int(const key_t&)> key_hash = [](const key_t& k) {
		return hash<int>()(k.x) ^ hash<int>()(k.y);
	};
	using key_hash_t = decltype(key_hash);
	using map_data_t = std::unordered_map<const key_t, chunk_t, key_hash_t,
			std::equal_to<const key_t>>;

	map_data_t data;

	GameMap(std::string map_file)
	: data(std::unordered_map<int, int>{}.bucket_count(), key_hash)
	{
		using namespace nlohmann;
		std::ifstream file(map_file);
		json jMap;
		file >> jMap;

		for (auto &&chunk_desc : jMap["chunks"]) {
			int row = chunk_desc["row"];
			int col = chunk_desc["col"];
			std::string chunk_file = chunk_desc["filename"];
			printf("loading: chunk [%d, %d] at [%s]\n",
					row, col, chunk_file.c_str());
			data[{row, col}] = load_chunk(chunk_file);
		}
	}

	char &operator () (int i, int j) {
		int row = div(i, CHK_SZ);
		int col = div(j, CHK_SZ);
		return data[{row, col}][abs(i - row * CHK_SZ)][abs(j - col * CHK_SZ)];
	}

	void draw(Math::Point2f cam_pos) {
		Math::Point2i pos = div(cam_pos + Math::Point2f(1, -1), SIDE);
		pos.y *= -1;
		Math::Point2i dirs[] = {
			{-1,  1}, { 0,  1}, { 1,  1},
			{-1,  0}, { 0,  0}, { 1,  0},
			{-1, -1}, { 0, -1}, { 1, -1},
		};

		// drawLine();
		draw_circle(pos.y, pos.x, SIDE, MAGENTA);
		auto p = div(pos, CHK_SZ) * CHK_SZ;
		draw_circle(p.y, p.x, SIDE, BLUE);

		for (auto &&dir : dirs) {
			auto p = div(pos, CHK_SZ) * CHK_SZ + dir * CHK_SZ;
			draw_grid(p.x, -p.y, CHK_SZ, CHK_SZ);

			for (int i = p.y; i < CHK_SZ + p.y; i++) {
				for (int j = p.x; j < CHK_SZ + p.x; j++) {
					if ((*this)(i, j) == '#')
						draw_wall(i, j);
					else if ((*this)(i, j) != '.')
						draw_cross(i, j);
				}
			}
		}
	}

	Math::Point2i get_index(Math::Point2f cursor) {
		Math::Point2i pos = div(cursor, SIDE);
		return {-pos.y - 1, pos.x};
	}

	int div(int a, float b) {
		return std::floor(a / b);
	}

	Math::Point2i div(Math::Point2i a, float b) {
		return Math::Point2i(std::floor(a.x / b), std::floor(a.y / b));
	}

	Math::Point2i div(Math::Point2f a, float b) {
		return Math::Point2i(std::floor(a.x / b), std::floor(a.y / b));
	}

	static chunk_t load_chunk(std::string filename) {
		std::ifstream file(filename);
		std::string line;
		bool done = false;
		chunk_t ret;
		int i = 0;
		while (!done && std::getline(file, line)) {
			for (int j = 0; j < line.size(); j++) {
				if (i < CHK_SZ && j < CHK_SZ) {
					ret.m[i][j] = line[j];
				}
				else if (i >= CHK_SZ) {
					done = true;
				}
			}
			i++;
		}
		return ret;
	}	

};

#endif