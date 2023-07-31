#pragma once

#include "raymath.h"
#include "map.h"

class MapCollider
{
public:
	MapCollider(const Map& map);

	bool Move(EntityLocation& look, const Vector2& desiredMotion, float radius = 0.25f);

protected:
	void PointNearesGridPoint(int x, int y, const Vector2 point, Vector2* nearest, Vector2* normal);

	const Map& WorldMap;
};