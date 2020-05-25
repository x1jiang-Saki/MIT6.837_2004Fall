#pragma once
#include "ray.h"

class Camera
{
protected:
	Vec3f _up;
	Vec3f _horizontal;
	Vec3f _direction;
	Vec3f _center;

public:
	Camera(){}
	~Camera(){}

	virtual Ray generateRay(Vec2f &point) = 0;
	virtual float getTMin() const = 0;
};

class OrthographicCamera : public Camera
{
	float _size;

public:
	// left hand
	// Orthobiasis(horizontal, up, -direction)
	OrthographicCamera(const Vec3f& center, const Vec3f& up, const Vec3f& direction, const float& size)
	{
		_center = center;
		_up = up;
		_direction = direction;
		_size = size;

		Vec3f::Cross3(_horizontal, _direction, _up);
		Vec3f::Cross3(_up, _horizontal, _direction);

		_up.Normalize();
		_direction.Normalize();
		_horizontal.Normalize();
	}
	~OrthographicCamera(){}

	virtual Ray generateRay(Vec2f &point);
	virtual float getTMin() const;
};