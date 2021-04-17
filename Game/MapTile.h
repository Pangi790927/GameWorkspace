#ifndef MAP_TILE_H
#define MAP_TILE_H

class MapTile {
public:
	bool empty = true;

	bool aquire() {
		empty = false;
		return empty;
	}

	bool canAquire() const {
		return empty;
	}

	void release() {
		empty = true;
	}
};

#endif