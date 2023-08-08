#include "map_collider.h"
#include "raymath.h"

MapCollider::MapCollider(const Map& map)
	: WorldMap(map)
{

}

bool MapCollider::Move(EntityLocation& loc, const Vector2& desiredMotion, float radius)
{
	Vector2 newPos = Vector2Add(loc.Position, desiredMotion);
	bool collided = false;

	for (int y = int(loc.Position.y - 1); y <= int(loc.Position.y + 1); y++)
	{
		for (int x = int(loc.Position.x - 1); x <= int(loc.Position.x + 1); x++)
		{
			if (WorldMap.GetCell(x, y) != 0)
			{
				// check rectangle

				Vector2 hitPoint = { -100,-100 };
				Vector2 hitNormal = { 0, 0 };
				PointNearesGridPoint(x,y, newPos, &hitPoint, &hitNormal);

				Vector2 vectorToHit = Vector2Subtract(hitPoint, newPos);

				bool inside = Vector2LengthSqr(vectorToHit) < radius * radius;

				if (inside)
				{
					collided = true;
					// normalize the vector along the point to where we are nearest
					vectorToHit = Vector2Normalize(vectorToHit);

					// project that out to the radius to find the point that should be 'deepest' into the rectangle.
					Vector2 projectedPoint = Vector2Add(newPos, Vector2Scale(vectorToHit, radius));

					// compute the shift to take the deepest point out to the edge of our nearest hit, based on the vector direction
					Vector2 delta = { 0,0 };

					if (hitNormal.x != 0)
						delta.x = hitPoint.x - projectedPoint.x;
					else
						delta.y = hitPoint.y - projectedPoint.y;

					// shift the new point by the delta to push us outside of the rectangle
					newPos = Vector2Add(newPos, delta);
				}
			}
		}
	}

	loc.Position = newPos;

	return collided;
}

void MapCollider::PointNearesGridPoint(int x, int y, const Vector2 point, Vector2* nearest, Vector2* normal)
{
	// get the closest point on the vertical sides
	float hValue = float(x);
	float hNormal = -1.0f;
	if (point.x > x + 1)
	{
		hValue +=1;
		hNormal = 1;
	}

	Vector2 vecToPoint = Vector2Subtract(Vector2{ hValue, float(y) }, point);
	// get the dot product between the ray and the vector to the point
	float dotForPoint = Vector2DotProduct(Vector2{ 0, -1 }, vecToPoint);
	Vector2 nearestPoint = { hValue,0 };

	if (dotForPoint < 0)
		nearestPoint.y = float(y);
	else if (dotForPoint >= 1)
		nearestPoint.y = float(y + 1);
	else
		nearestPoint.y = y + dotForPoint;

	// get the closest point on the horizontal sides
	float vValue = float(y);
	float vNormal = -1;
	if (point.y > y + 1)
	{
		vValue = float(y + 1);
		vNormal = 1;
	}

	vecToPoint = Vector2Subtract(Vector2{ float(x), vValue }, point);
	// get the dot product between the ray and the vector to the point
	dotForPoint = Vector2DotProduct(Vector2{ -1, 0 }, vecToPoint);
	*nearest = Vector2{ 0,vValue };

	if (dotForPoint < 0)
		nearest->x = float(x);
	else if (dotForPoint >= 1)
		nearest->x = float(x + 1);
	else
		nearest->x = float(x + dotForPoint);

	if (Vector2LengthSqr(Vector2Subtract(point, nearestPoint)) < Vector2LengthSqr(Vector2Subtract(point, *nearest)))
	{
		*nearest = nearestPoint;

		if (normal)
		{
			normal->x = hNormal;
			normal->y = 0;
		}
	}
	else
	{
		if (normal)
		{
			normal->y = vNormal;
			normal->x = 0;
		}
	}
}
