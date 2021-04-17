#ifndef SUB_TILE_H
#define SUB_TILE_H

#include "Unit.h"

class SubTile {
public:
	bool empty = true;
	std::shared_ptr<Unit> unitPtr;

	// void render (DrawContext& drawContext, ShaderProgram &shader) {
	// 	if (unitPtr)
	// 		unitPtr->render(drawContext, shader);
	// }
};

#endif