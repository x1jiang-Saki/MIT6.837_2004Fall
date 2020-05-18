#include "Base_Camera.h"

Ray OrthographicCamera::generateRay(Vec2f ssPos)
{
	Vec3f origin = _center + (ssPos.x() - 0.5) * _screenSize * _horizontal + (ssPos.y() - 0.5) * _screenSize * _up;
	Vec3f direction = _direction;
	return Ray(origin, direction);
}


float OrthographicCamera::getTMin() const
{
	// For an orthographic camera, rays always start at infinity, so tmin will be a large negative value.
	return -INFINITY;
}

Ray PerspectiveCamera::generateRay(Vec2f ssPos)
{
	Vec3f origin = _center;
	Vec3f direction = 0.5 / tan(_fov / 2.0) * _direction + (ssPos.x() - 0.5) * _horizontal + (ssPos.y() - 0.5) * _up;
	direction.Normalize();
	
	return Ray(origin, direction);
}

float PerspectiveCamera::getTMin() const
{
	// the value of tmin will be zero to correctly clip objects behind the viewpoint.
	return 0.0f;
}

