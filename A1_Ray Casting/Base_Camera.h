#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "_ray.h"

class Camera
{
public:
	Camera() {}
	virtual ~Camera() {}

	virtual Ray generateRay(Vec2f ssPos) = 0;
	// The getTMin() method will be useful when tracing rays through the scene.
	virtual float getTMin() const = 0;
};

class OrthographicCamera : public Camera
{
public:
	OrthographicCamera(Vec3f center, Vec3f direction, Vec3f up, float screenSize)
	{
		_center = center;
		_direction = direction;
		_up = up;
		_screenSize = screenSize;

		_direction.Normalize();
		// The input up vector might not be a unit vector or perpendicular to the direction.
		Vec3f::Cross3(_horizontal, direction, up);
		Vec3f::Cross3(_up, _horizontal, direction);

		_horizontal.Normalize();
		_up.Normalize();
	}
	~OrthographicCamera() {}

	Ray generateRay(Vec2f ssPos);
	float getTMin() const;
private:
	// orthobasis(u: horizental, v: up, w: -direction)
	Vec3f _center;
	Vec3f _direction;
	Vec3f _up;
	Vec3f _horizontal;
	float _screenSize;
};

#endif // _CAMERA_H_