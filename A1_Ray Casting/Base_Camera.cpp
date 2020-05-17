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

//a perspective camera and the value of tmin will be zero to correctly clip objects behind the viewpoint.