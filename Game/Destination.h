#ifndef DESTINATION_H
#define DESTINATION_H

class Destination {
public:
	static const int MAX_TRIES = 20;
	int next = 0;
	std::vector<Math::Point2i> path;
	
	Math::Point2i finish = Math::Point2i();
	int tries = MAX_TRIES;

	void clearPath() {
		next = 0;
		path.clear();
	}

	void setFinish (Math::Point2i dest) {
		clearPath();
		finish = dest;
		tries = MAX_TRIES;
	}

	Math::Point2i getNext() {
		if (next < path.size()) {
			return path[next];
		}
		return Math::Point2i();
	}

	Math::Point2i advance() {
		if (next < path.size()) {
			return path[next++];
		}
		return Math::Point2i();
	}

	bool finishedPath() {
		return next >= path.size();
	}
};

#endif