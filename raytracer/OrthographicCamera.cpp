#include "Camera.h"

Ray OrthographicCamera::generateRay(Vec2f point)
{
	Vec3f origin = _center + (point.x() - 0.5) * _size * _horizontal + (point.y() - 0.5) * _size * _up;
	Vec3f direction = _direction;
	direction.Normalize();
	return Ray(origin, direction);
}

float OrthographicCamera::getTMin() const
{
	// a large negative value
	return -INFINITY;
}
