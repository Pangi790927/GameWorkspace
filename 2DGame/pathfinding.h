#ifndef PATHFINDING_H
#define PATHFINDING_H

#include <queue>
#include <unordered_map>
#include "draw_utils.h"
#include "game_map.h"

std::vector<Math::Point2i> animated_fill(GameMap &map, int i, int j) {
	using key_t = Math::Point2i;
	auto key_hash = [](const key_t& k) {
		return hash<int>()(k.x) ^ hash<int>()(k.y);
	};
	using key_hash_t = decltype(key_hash);
	using map_t = std::unordered_map<const key_t, bool, key_hash_t,
			std::equal_to<const key_t>>;

	map_t visited(std::unordered_map<int, int>{}.bucket_count(), key_hash);
	std::vector<Math::Point2i> visit_order;
	std::queue<Math::Point2i> to_walk;
	Math::Point2i neigh_list[] = {
		{-1,  1}, { 0,  1}, { 1,  1},
		{-1,  0},           { 1,  0},
		{-1, -1}, { 0, -1}, { 1, -1},
	};
	visited[{i, j}] = true;
	visit_order.push_back({i, j});
	to_walk.push({i, j});
	while (to_walk.size()) {
		auto elem = to_walk.front();
		to_walk.pop();
		for (auto &&neigh_dir : neigh_list) {
			auto neigh = neigh_dir + elem;
			if (neigh.x < 0 || neigh.y < 0 || neigh.x >= 256 || neigh.y >= 256)
				continue ;
			if (map(neigh.x, neigh.y) == '.' && !visited[neigh]) {
				visited[neigh] = true;
				visit_order.push_back(neigh);
				to_walk.push(neigh);
			}
		}
	}

	return visit_order;
}

auto animated_dijkstra(GameMap &map, int i, int j,
		int ti, int tj)
{
	std::vector<Math::Point2i> visit_order;
	std::vector<std::vector<Math::Point2i>> update_order;
	std::vector<Math::Point2i> path;

	using key_t = Math::Point2i;
	auto key_hash = [](const key_t& k) {
		return hash<int>()(k.x) ^ hash<int>()(k.y);
	};
	using key_hash_t = decltype(key_hash);
	using map_t = std::unordered_map<const key_t, int, key_hash_t,
			std::equal_to<const key_t>>;

	Math::Point2i neigh_dir[] = {
		{-1,  1}, { 0,  1}, { 1,  1},
		{-1,  0},           { 1,  0},
		{-1, -1}, { 0, -1}, { 1, -1},
	};

	int neigh_cost[] = {
		14, 10, 14,
		10,     10,
		14, 10, 14,
	};

	// g cost, h cost, x, y
	using prio_key_t = std::tuple<int, int, int>;
	std::priority_queue<prio_key_t, std::vector<prio_key_t>,
			std::greater<prio_key_t>> pq;
	map_t dist(std::unordered_map<int, int>{}.bucket_count(), key_hash);

	pq.emplace(0, i, j);
	dist[{i, j}] = 0;

	while (!pq.empty()) {
		std::vector<Math::Point2i> updates;
		auto node = pq.top();
		pq.pop();

		auto curr_pos = Math::Point2i(std::get<1>(node), std::get<2>(node));
		int curr_cost = std::get<0>(node);

		if (curr_pos == Math::Point2i(ti, tj))
			break;

		visit_order.push_back(curr_pos);

		for (int k = 0; k < 8; k++) {
			auto neigh = neigh_dir[k] + curr_pos;

			if (neigh.x < 0 || neigh.y < 0 || neigh.x >= 256 || neigh.y >= 256
					|| map(neigh.x, neigh.y) != '.')
				continue ;

			if (dist.find(neigh) == dist.end() ||
					dist[neigh] > dist[curr_pos] + neigh_cost[k])
			{
				dist[neigh] = dist[curr_pos] + neigh_cost[k];
				pq.push({dist[neigh], neigh.x, neigh.y});
				updates.push_back(neigh);
			}
		}
		update_order.push_back(updates);
	}

	return std::tuple{visit_order, update_order};
}

/* TO DO: Something here is broken, fix it */
auto animated_A_star(GameMap &map, int i, int j, int ti, int tj) {
	std::vector<Math::Point2i> visit_order;
	std::vector<std::vector<Math::Point2i>> update_order;
	std::vector<Math::Point2i> path;

	auto hcost = [&](Math::Point2i node){
		int di = abs(ti - node.x);
		int dj = abs(tj - node.y);
		// return (di + dj) * 10;
		if (di > dj)
			return (di - dj) * 10 + dj * 14;
		else
			return (dj - di) * 10 + di * 14;
	};

	using key_t = Math::Point2i;
	auto key_hash = [](const key_t& k) {
		return hash<int>()(k.x) ^ hash<int>()(k.y);
	};
	using key_hash_t = decltype(key_hash);
	using map_dist_t = std::unordered_map<const key_t, int, key_hash_t,
			std::equal_to<const key_t>>;
	using map_src_t = std::unordered_map<const key_t, Math::Point2i, key_hash_t,
			std::equal_to<const key_t>>;

	Math::Point2i neigh_dir[] = {
		{-1,  1}, { 0,  1}, { 1,  1},
		{-1,  0},           { 1,  0},
		{-1, -1}, { 0, -1}, { 1, -1},
	};

	int neigh_cost[] = {
		14, 10, 14,
		10,     10,
		14, 10, 14,
	};

	// f cost, h cost, g cost , x, y
	using prio_key_t = std::tuple<int, int, int, int>;
	std::priority_queue<prio_key_t, std::vector<prio_key_t>,
			std::greater<prio_key_t>> pq;
	map_dist_t dist(std::unordered_map<int, int>{}.bucket_count(), key_hash);
	map_src_t src(std::unordered_map<int, int>{}.bucket_count(), key_hash);

	pq.emplace(hcost({i, j}), 0, i, j);
	dist[{i, j}] = 0;

	Math::Point2i best;
	int best_h = 2000'000'000;
	while (!pq.empty()) {
		std::vector<Math::Point2i> updates;
		auto node = pq.top();
		pq.pop();

		auto curr_pos = Math::Point2i(std::get<2>(node), std::get<3>(node));
		int curr_cost = std::get<0>(node);

		if (curr_pos == Math::Point2i(ti, tj))
			break;

		visit_order.push_back(curr_pos);

		for (int k = 0; k < 8; k++) {
			auto neigh = neigh_dir[k] + curr_pos;

			if (map(neigh.x, neigh.y) != '.')
				continue ;

			if (dist.find(neigh) == dist.end() ||
					dist[neigh] > dist[curr_pos] + neigh_cost[k])
			{
				if (best_h > hcost(neigh)) {
					best_h = hcost(neigh);
					best = neigh;
				}
				src[neigh] = curr_pos;
				dist[neigh] = dist[curr_pos] + neigh_cost[k];
				pq.push({dist[neigh] + hcost(neigh), hcost(neigh),
						neigh.x, neigh.y});
				updates.push_back(neigh);
			}
		}
		update_order.push_back(updates);
	}

	auto origin = Math::Point2i(i, j);
	while (!(best == origin)) {
		printf("i[%d] j[%d]\n", best.x, best.y);
		path.push_back(best);
		if (src.find(best) == src.end()) {
			printf("Error finding path\n");
			break;
		}
		best = src[best];
	}

	return std::tuple{visit_order, update_order, path};
}

/* Pathing idea:
	- create a path object that is made out of <prio_q, dist, src>
	- have all units mooving along use the same path object, somehow
	- enable the unit to walk to the best solution so far if the path is
		not fully constructed
	- have a dedicated thread do all pathing computations */

#endif