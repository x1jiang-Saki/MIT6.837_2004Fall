#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "_ray.h"
#include <GL/gl.h>
#include <GL/glu.h>

class Camera
{
protected:
	Vec3f _center;
	Vec3f _direction;
	Vec3f _up;
	Vec3f _horizontal;

public:
	Camera() {}
	virtual ~Camera() {}

	virtual Ray generateRay(Vec2f ssPos) = 0;
	// The getTMin() method will be useful when tracing rays through the scene.
	virtual float getTMin() const = 0;
	virtual void glInit(int w, int h) = 0;
	virtual void glPlaceCamera(void) = 0;
	virtual void dollyCamera(float dist) = 0;
	virtual void truckCamera(float dx, float dy) = 0;
	virtual void rotateCamera(float rx, float ry) = 0;
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
	virtual float getTMin() const;
	virtual void glInit(int w, int h);
	virtual void glPlaceCamera(void);
	virtual void dollyCamera(float dist);
	virtual void truckCamera(float dx, float dy);
	virtual void rotateCamera(float rx, float ry);
private:
	// orthobasis(u: horizental, v: up, w: -direction)
	float _screenSize;
};

class PerspectiveCamera : public Camera
{
public:
	PerspectiveCamera(Vec3f center, Vec3f direction, Vec3f up, float fov)
	{
		_center = center;
		_direction = direction;
		_up = up;
		_fov = fov;

		_direction.Normalize();
		// The input up vector might not be a unit vector or perpendicular to the direction.
		Vec3f::Cross3(_horizontal, direction, up);
		Vec3f::Cross3(_up, _horizontal, direction);

		_horizontal.Normalize();
		_up.Normalize();

	}
	~PerspectiveCamera(){}

	Ray generateRay(Vec2f ssPos);
	virtual float getTMin() const;
	virtual void glInit(int w, int h);
	virtual void glPlaceCamera(void);
	virtual void dollyCamera(float dist);
	virtual void truckCamera(float dx, float dy);
	virtual void rotateCamera(float rx, float ry);
private:
	float _fov;
};

#endif // _CAMERA_H_