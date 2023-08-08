#pragma once

#include "raylib.h"
#include <corecrt_math.h>

struct EntityLocation
{
	Vector2 Position = { 0 };
	Vector2 Facing = { 0 };

	inline float GetFacingAngle() { return atan2f(Facing.y, Facing.x) * RAD2DEG; }
};
