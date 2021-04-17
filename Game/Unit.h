#ifndef UNIT_H
#define UNIT_H

#include <set>
#include <map>
#include <queue>
#include <utility>

#include "ShaderProgram.h"
#include "Destination.h"
#include "GameMap.h"

class Unit {
public:
	const static int DEFAULT_MAX_ITER = 128;

	int player = 0;
	int type = 0;
	int maxIter = DEFAULT_MAX_ITER;

	Math::Point3f pos;
	Math::Point3f dir;

	Destination dest;

	Unit (int player = 0, int type = 0)
	: player(player), type(type) {}

	virtual void move (GameMap& map) {
		if (map.canAquire(dest.getNext()) && !dest.finishedPath()) {
			map.release(map.getTilePos(pos));
			pos = map.toWorld(dest.getNext());
			map.aquire(dest.advance());
		}
		else {
			if (dest.path.size() < dest.next)
				dest.clearPath();
			else if (dest.path.size() > 0)
				dest.path.pop_back();
			else if (dest.tries > 0) {
				dest.tries--;
				path(map);	// maybe add a thread pool here?
							// and join threads in another function?
			}
		}
	}

	void path (GameMap& map) {
		dest.clearPath();
		int iterLeft = maxIter;

		using namespace Math;
		float inf = map.height * map.width * 100;
		
		auto posHash = [&] (const auto& pos) -> int {
			return map.height * pos.x + pos.y;
		};

		auto posEq = [&] (const auto& left, const auto& right) {
			return posHash(left) == posHash(right);
		};

		std::unordered_map<Point2i, Point2i, decltype(posHash), decltype(posEq)>
		prev(size_t(180), posHash, posEq);
		
		std::unordered_map<Point2i, float, decltype(posHash), decltype(posEq)>
		tDist(size_t(180), posHash, posEq);
		
		std::unordered_map<Point2i, float, decltype(posHash), decltype(posEq)>
		hDist(size_t(180), posHash, posEq);

		auto getTDist = [&] (auto pos) {
			if (tDist.find(pos) == tDist.end())
				tDist[pos] = inf;
			return tDist[pos];
		};

		auto getHDist = [&] (auto pos) {
			if (hDist.find(pos) == hDist.end())
				hDist[pos] = inf;
			return hDist[pos];
		};

		auto cmp = [&] (auto left, auto right) -> bool {
			return left.second > right.second;
		};

		auto squareNorm = [] (auto point) -> float {
			return (point.tr() * point).x;
		};

		std::priority_queue<std::pair<Point2i, float>,
				std::vector<std::pair<Point2i, float>>, decltype(cmp)> que(cmp);
		auto start = map.getTilePos(pos);
		auto closest = start;

		tDist[start] = 0;
		hDist[start] = (Point2f(dest.finish) - Point2f(start)).norm2();
		que.push(std::pair<Point2i, float>(start, tDist[start]));
		while (!que.empty() && squareNorm(closest - dest.finish) > 0.001 && iterLeft > 0) {
			if (que.top().second > getHDist(que.top().first)) {
				que.pop();
			}
			else {
				iterLeft--;
				auto node = que.top().first;
				que.pop();

				for (int i = 0; i < 9; i++) {
					auto neigh = node + Point2i(i % 3 - 1, i / 3 - 1);
					if (!map.inside(neigh) || i == 3 || !map.canAquire(neigh))
						continue;
					if (squareNorm(Point2f(closest - dest.finish)) >
							squareNorm(Point2f(neigh - dest.finish)))
						closest = neigh;
					float score = getTDist(node)
							+ (Point2f(node) - Point2f(neigh)).norm2();
					if (score < getTDist(neigh)) {
						hDist[neigh] = getTDist(node)
								+ (Point2f(node) - Point2f(neigh)).norm2()
								+ (Point2f(dest.finish) - Point2f(neigh)).norm2();
						tDist[neigh] = score;
						que.push(std::pair<Point2i, float>(neigh, hDist[neigh]));
						prev[neigh] = node;
					}
				}
			}
		}

		while (prev.find(closest) != prev.end()) {
			dest.path.push_back(closest);
			closest = prev[closest];
		}
		std::reverse(dest.path.begin(), dest.path.end());
	}

	virtual void render (DrawContext& drawContext,
			ShaderProgram &defaultShader)
	{
		defaultShader.useProgram();
		defaultShader.setMatrix("projectionMatrix", drawContext.proj);
		defaultShader.setMatrix("viewMatrix", drawContext.view);
		defaultShader.setMatrix("worldMatrix", 
				Math::translation<float>(pos) *
				drawContext.world);
		Util::drawLine(
			Math::Point3f(0, 0, 0),
			Math::Point3f(0, 10, 0),
			Math::Point4f(
				player % 2,
				(player / 2) % 2,
				(player / 4) % 2,
				1
			)
		);
	}
};

#endif