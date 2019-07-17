#ifndef WORLD_H
#define WORLD_H

#include "MathLib.h"

namespace World
{
	const Math::Point3f up = Math::Point3f(0, 1, 0);
	const Math::Point3f down = Math::Point3f(0, -1, 0);
	const Math::Point3f north = Math::Point3f(0, 0, -1);
	const Math::Point3f south = Math::Point3f(0, 0, 1);
	const Math::Point3f west = Math::Point3f(-1, 0, 0);
	const Math::Point3f east = Math::Point3f(1, 0, 0);
}

#endif