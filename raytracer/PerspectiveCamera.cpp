#include "Camera.h"

Ray PerspectiveCamera::generateRay(Vec2f point)
{
	Vec3f ro = _center;
	Vec3f rd = 0.5 / tan(_fov / 2.0) * _direction + (point.x() - 0.5) * _horizontal + (point.y() - 0.5) * _up;
	rd.Normalize();

	return Ray(ro, rd);
}

float PerspectiveCamera::getTMin() const
{
	return 0.0f;
}