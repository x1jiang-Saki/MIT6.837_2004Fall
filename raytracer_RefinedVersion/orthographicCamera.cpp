#include "camera.h"

Ray OrthographicCamera::generateRay(Vec2f& point)
{
	Vec3f ro = _center + _horizontal * _size * (point.x() - 0.5) + _up * _size * (point.y() - 0.5);
	Vec3f rd = _direction;

	return Ray(ro, rd);
}

float OrthographicCamera::getTMin() const
{
	// a large negative value
	return INFINITY;
}