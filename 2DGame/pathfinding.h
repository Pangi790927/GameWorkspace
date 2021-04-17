#ifndef PATHFINDING_H
#define PATHFINDING_H

#include <queue>
#include <unordered_map>
#include "draw_utils.h"
#include "game_map.h"

/*
Some ideas:

1. New idea:
	all tiles are the same, water or dirt
	use clearance based pathing
	0. generate a final formation
	1. split selected units by groups, if they are too far away they separate
	2. choose a path comander for each group
	3. set speed as slowest unit for mini grup
	5. for each group compute commander path
	5.1. for each group compute travel order
	6. per group move troops:
		0. commander declares move intention: direction and position
		-1. each troop tries to do it's queued action
		1. each troop when done last action will try to move in the direction
		of the commander + the direction to the desired flock position
		normalized to group speed and with small negative or positive bonnus
		if not in vecinity of desired location.
		1.1 aditional check to see if can move backwards to achieve desired
		effect
		2. units are circles and take ownership of tiles bellow them when circle
			intersects tile circle
		3. if forward blocked by moving ally, pass update
		4. if forward is blocked by stationary ally, tell him to path away from
		you in the oposite direction of commander
		5. if forward path blocked and commander is in back, wait
		6. if forward path is blocked and commander is in front, path to
		commander, at most n squares
		7. if pathing to commander fails split from group and repath to
		formation
	7. for each unit not in final formation if close to objective path to
	objective
	8. else repath to formation
	9. pathing is a state of the troop, as is the state of following
	10. rotating is done by checking the rotation speed. Each troop has
	x time in an update to do it's stuff for each paralel part. Turret and
	wheels are one example of paralel parts. So wheels can turn and move forward
	to move forward to a 
	11. Paths should use regions if path is not in same or neghbour region


Troops can be passed by troops, but should prefer not to.

Troop groups should re-use the path computed by first troop and only on failure
to follow should they recompute their own path.

There are two types of troops:
	* foot troops and
	* heavy troops
Foot troops have 3 path stages while heavy troops have only 2.
The first stage is the foot stage, only for foot troops and in this stage the
troop will do the following:
	1. if there is a path to follow:
		1. if can advance on path, advance on path, if advanced on heavy path
			pop heavy node from stack, if advanced map path, pop
		2. if can't advance on path because it is blocked by moving ally unit
			wait
		3. if can't advance on path because it is blocked by targetable enemy
			attack enemy once
		4. if can't advance for any other reason: recompute foot path
			1. if recompute is still inside the two heavy tiles: curr and next
				all ok or inside same heavy if target is in same tile
			2. else: try to recompute heavy path(note this can happen also if
				target is inside same heavy tile, because it means there was
				no foot path otherwise)
	2. else if there is a heavy path:
		1. try to compute foot path as in 1.4 and if it fails note failure(
			remember that for this step there is no path directly from heavy1
			to heavy2) and try recompute heavy (optimization: if foot tile in
			heavy tile is not accessible from current heavy tile, try dfs to see
			if any heavy is accesible and if not fail heavy path)


*/

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